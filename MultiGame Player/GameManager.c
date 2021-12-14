// required libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

// constants
#define MAX_USERS 3
#define BUFFER_SZ 2048
#define NO_ROUNDS 2
#define PLAYERS_REQUIRED 2
#define MIN 1
#define MAX 100

// static values
static _Atomic unsigned int cli_count = 0;
static _Atomic unsigned int rounds = 0;
static _Atomic unsigned int joined_users = 0;
static int uid = 10;

// global
char* ROUND_STARTED_SIG = "ROUND_STARTED\0";
char* GAME_COMPLETED_SIG = "GAME_COMPLETED\0";

/* user structure */
typedef struct {
    // socket address of user
	struct sockaddr_in address;
    // socket id
	int sockfd;
    // user id
	int uid;
    // name of user
	char name[32];
    // numbers
    int nums[NO_ROUNDS];
    // places
    int places[NO_ROUNDS];
    // current position
    int position;
} user_t;

// define array to store users
user_t *clients[MAX_USERS];

// thread init
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// function definitions
void *handle_player(void *arg);
void str_overwrite_stdout();
void str_trim_lf (char* arr, int length);
void print_client_addr(struct sockaddr_in addr);
void queue_add(user_t *cl);
void queue_remove(int uid);
void send_message(char *msg);
void send_message_uid(char *msg, int user_id);
void startGame(int listenfd);
void swap(int* xp, int* yp);
void selectionSort(int arr[], int n);
int isPlaceOccupied(int place, int round);
int checkPlace(int arr[], int number, int place);

/***
 * main method
 **/
int main(int argc, char **argv) {

    // check for PORT
    if(argc != 2){
		printf("Usage : <port>\n");
		return EXIT_FAILURE;
	}

    // server configurations
	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);
	int option = 1;
	int listenfd = 0;
    struct sockaddr_in serv_addr;

    /* socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    /* ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR , &option, sizeof(option))) {
		perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
	}

	/* bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    /* listen */
    if (listen(listenfd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

	printf("=== SERVER STARTED ===\n");

    while(1) {
        startGame(listenfd);
    }

	return EXIT_SUCCESS;
}

/* start game */
void startGame(int listenfd) {

    struct sockaddr_in cli_addr;
    pthread_t tid;
    int connfd = 0;
    int i,j;

    printf("New game instance started ...\n");
    printf("Wait for two players to connect...\n");
    while(PLAYERS_REQUIRED > cli_count) {

		socklen_t client = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &client);

		/* check if max users is reached */
		if((cli_count + 1) == MAX_USERS){
			printf("Max users reached.\n Try again with different port or time.\n");
			print_client_addr(cli_addr);
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* client settings */
		user_t *cli = (user_t *)malloc(sizeof(user_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_player, (void*)cli);

		/* reduce CPU usage */
		sleep(1);
	}

    // start game
    char buff_out[BUFFER_SZ];
    sprintf(buff_out, "\n=== INSTRUCTIONS ===\n\n---> You guys will receive cards with numbers\n---> You must placed cards in ascending order to win this game\n---> This game has %d rounds\n---> If players failed to laydown the cards in ascending order, \n\t\tplayers have to go back to the previous round or remain in the first round\n---> This will begin now\n\n*** *** *** *** *** ****\n\n", NO_ROUNDS);
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);

    sprintf(buff_out,"=== PLAYERS LIST ===\n");
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);
    
    for(i=0;i<cli_count;i++) {
        sprintf(buff_out,"---> Player - %s\n", clients[i]->name);
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
    }

    sprintf(buff_out,"---> Player - %s\n", "Robot");
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);

    sprintf(buff_out,"\nGame Started ...\n\n");
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);
    sleep(1);

    while (NO_ROUNDS > rounds)
    {
        rounds++;
        sprintf(buff_out, "=== Round %d ===\n", rounds);
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
        sprintf(buff_out, "\n---> Every user has %d numbers\n", rounds);
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
        printf("\nStarted Round : %d\n", rounds);
        sleep(1);

        // give signal to indicate round has been started
        send_message(ROUND_STARTED_SIG);
        sleep(1);
        
        sprintf(buff_out, "%d",rounds);
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
        sleep(1);

        // init client numbers and places
        for(i=0;i<cli_count;i++) {
            for(j=0;j<NO_ROUNDS;j++) {
                clients[i]->nums[j] = 0;
                clients[i]->places[j] = 0;
            }
            clients[i]->position = -1;
        }

        int index = 0;
        int numbers[(cli_count + 1) * rounds];
        for(i=0; i<cli_count; i++) {
            for(j=0; j<rounds; j++) {
                numbers[index] = (rand() % (MAX - MIN + 1)) + MIN;
                sprintf(buff_out, "%d", numbers[index]);
                send_message_uid(buff_out, clients[i]->uid);
                bzero(buff_out, BUFFER_SZ);
                clients[i]->nums[j] = numbers[index];
                index++;
                sleep(1);
            }
        }

        // give numbers for bot player
        int bot_places[rounds];
        int bot_numbers[rounds];
        for(j=0; j<rounds; j++) {
            numbers[index + j] = (rand() % (MAX - MIN + 1)) + MIN;
            bot_places[j] = 0;
            bot_numbers[j] = numbers[index + j];
        }

        selectionSort(bot_numbers, rounds);

        int isDone = 0;
        while (isDone == 0)
        {
            isDone = 1;
            for(i=0; i<cli_count; i++) {
                for(j=0; j<rounds; j++) {
                    if(clients[i]->places[j] == 0) {
                        isDone = 0;
                        break;
                    }
                }
            }
        }

        printf("Bot placing his/her places now...\n");
        sprintf(buff_out, "Bot placing his/her places now...\n");
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
        sleep(1);

        int botIndex = 0;
        for(i=1; i<= (cli_count+1) * rounds; i++) {
            if(isPlaceOccupied(i, rounds) == 1) {
                bot_places[botIndex] = i;
                botIndex++;
            }
        }

        printf("All players placed their card places \n");
        sprintf(buff_out, "All players placed their card places \n");
        send_message(buff_out);
        bzero(buff_out, BUFFER_SZ);
        sleep(1);

        for(i=0; i<cli_count; i++) {
            for(j=0;j<rounds; j++) {
                sprintf(buff_out, "Player : %s placed no. %d for card : %d\n", clients[i]->name, clients[i]->places[j], clients[i]->nums[j]);
                send_message(buff_out);
                bzero(buff_out, BUFFER_SZ);
                sleep(1);
            }
        }

        for(j=0;j<rounds; j++) {
            sprintf(buff_out, "Player : %s placed no. %d for card : %d\n", "Robot", bot_places[j], bot_numbers[j]);
            send_message(buff_out);
            bzero(buff_out, BUFFER_SZ);
            sleep(1);
        }

        // sort numbers in asc
        selectionSort(numbers, (cli_count + 1) * rounds);

        int isSuccess = 1;
        for(i=0; i<cli_count; i++) {
            for(j=0;j<rounds; j++) {
                if(checkPlace(numbers, clients[i]->nums[j], clients[i]->places[j]) == -1) {
                    isSuccess = -1;
                    break;
                }
            }
        }

        if(isSuccess == 1) {
            printf("\nCompleted Round : %d\n", rounds);
            sprintf(buff_out, "\n*** Completed Round %d ***\n\n",rounds);
            send_message(buff_out);
            bzero(buff_out, BUFFER_SZ);
        } else {
            printf("\nIncorrect Order\n");
            sprintf(buff_out, "\n*** Incorrect Order %d ***\n\n",rounds);
            send_message(buff_out);
            bzero(buff_out, BUFFER_SZ);
            if(rounds == 1) {
                rounds--;
                sprintf(buff_out, "\n*** You have to remain in first round ***\n\n");
                send_message(buff_out);
                bzero(buff_out, BUFFER_SZ);
            } else {
                rounds -= 2;
                sprintf(buff_out, "\n*** You have to go back to round %d ***\n\n",rounds + 1);
                send_message(buff_out);
                bzero(buff_out, BUFFER_SZ);
            }
        }
        
        sleep(2);
    }

    sleep(2);
    printf("Completed Game\n");
    sprintf(buff_out,"Congradulations!\n");
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);

    send_message(GAME_COMPLETED_SIG);
    sleep(1);
}

/* handle all communication with the users/players */
void *handle_player(void *arg) {

	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	cli_count++;
	user_t *cli = (user_t *)arg;

	// enter name
	if(recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1){
		printf("didn't enter the name.\n");
		leave_flag = 1;
	} else {
		strcpy(cli->name, name);
		sprintf(buff_out, "%s has joined\n", cli->name);
        joined_users++;
		printf("%s", buff_out);
        if(joined_users != PLAYERS_REQUIRED) {
            send_message_uid("\nWait for another user to connect\n", cli->uid);
        }
	}

	bzero(buff_out, BUFFER_SZ);

	while(1) {

		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0) {
			if(strlen(buff_out) > 0) {
                cli->position += 1;
                int place = atoi(buff_out);
				printf("User : %s | Entered Placed : %d For Card : %d\n", cli->name, place, cli->nums[cli->position]);
                cli->places[cli->position] = place;
			}
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

    /* delete client from queue and yield thread */
    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());
	return NULL;
}

/* overwrite stdout */
void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

/* trim \n from a string */
void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { 
    if (arr[i] == '\n') { // trim \n
      arr[i] = '\0';
      break;
    }
  }
}

/* print client address */
void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* add user/player to queue */
void queue_add(user_t *cl){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_USERS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* remove user/player from queue */
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_USERS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* send message to all users/players */
void send_message(char *msg){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_USERS; ++i){
		if(clients[i]){
			if(write(clients[i]->sockfd, msg, strlen(msg)) < 0){
				perror("ERROR: write to descriptor failed");
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* send message to user/player for user */
void send_message_uid(char *msg, int user_id) {
	pthread_mutex_lock(&clients_mutex);
    for(int i=0; i<MAX_USERS; ++i){
		if(clients[i]){
            if(clients[i]->uid == user_id) {
                if(write(clients[i]->sockfd, msg, strlen(msg)) < 0){
                    perror("ERROR: write to descriptor failed");
                    break;
                }
                break;
            }
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

/* swap two elements */
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

/* selection sort */
void selectionSort(int arr[], int n)
{
    int i, j, min_idx;
 
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {
 
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
 
        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}

/* check place occupied already by anothe user */
int isPlaceOccupied(int place, int round) {
    int i,j;
    for(i=0; i<cli_count; i++) {
        for(j=0; j<rounds; j++) {
            if(clients[i]->places[j] != 0 && clients[i]->places[j] == place) {
                return -1;
            }
        }
    }
    return 1;
}

/* check place is correct */
int checkPlace(int arr[], int number, int place) {
    int i;
    for(i=0; i < (cli_count + 1) * rounds; i++) {
        if(arr[i] == number && (i+1) == place) {
            return 1;
        }
    }
    return -1;
}