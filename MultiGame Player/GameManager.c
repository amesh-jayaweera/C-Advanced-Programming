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

// static values
static _Atomic unsigned int cli_count = 0;
static _Atomic unsigned int rounds = 0;
static _Atomic unsigned int joined_users = 0;
static int uid = 10;

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

    /* Socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    /* Ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR , &option, sizeof(option))) {
		perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
	}

	/* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    /* Listen */
    if (listen(listenfd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

	printf("=== SERVER STARTED ===\n");

    startGame(listenfd);

	return EXIT_SUCCESS;
}

/* start game */
void startGame(int listenfd) {

    struct sockaddr_in cli_addr;
    pthread_t tid;
    int connfd = 0;

    printf("New game instance has been started ...\n");
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
    sprintf(buff_out, "You guys will receive cards with numbers and you must placed cards in ascending order to win this game.\nThis game has %d rounds\nIf players failed to laydown the cards in ascending order, players have to go back to previous round or remain in the first round.\nThis will begin after after 5 secs\n", NO_ROUNDS);
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);
    sprintf(buff_out,"Game has been started\n");
    send_message(buff_out);
    bzero(buff_out, BUFFER_SZ);
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
            send_message_uid("Welcome to the game\nWait for another user to connect", cli->uid);
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
				send_message(buff_out);
				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s -> %s\n", buff_out, cli->name);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0) {
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			send_message(buff_out);
			leave_flag = 1;
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