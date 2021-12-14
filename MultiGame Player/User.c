#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

// constants
#define LENGTH 2048

// global variables
char* ROUND_STARTED_SIG = "ROUND_STARTED\0";
char* GAME_COMPLETED_SIG = "GAME_COMPLETED\0";
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
// static variables
static _Atomic unsigned int round = 0;

// define functions
void str_overwrite_stdout();
void str_trim_lf (char* arr, int length);
void catch_ctrl_c_and_exit(int sig);

/***
 * main method
 **/
int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);

	signal(SIGINT, catch_ctrl_c_and_exit);

	printf("Please enter your name: ");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));


	if (strlen(name) > 32 || strlen(name) < 2){
		printf("Name must be less than 30 and more than 2 characters.\n");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;

	/* socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);


    // connect to server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// send name
	send(sockfd, name, 32, 0);

	printf("=== WELCOME TO THE GAME ===\n");

    int stopped = 0; 
    int isGameCompleted = 0;
	char message[LENGTH] = {};
    while (1) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            if(strcmp(message, ROUND_STARTED_SIG) == 0) {
                str_overwrite_stdout();
                memset(message, 0, sizeof(message));
                receive = recv(sockfd, message, LENGTH, 0);
                if(receive > 0) {
                    round = atoi(message);
                    int numbers[round];
                    memset(message, 0, sizeof(message));
                    int i;
                    printf("*** *** Your Numbers : ");
                    for(i=0; i<round; i++) {
                        receive = recv(sockfd, message, LENGTH, 0);
                        if(receive <= 0) {
                            printf("Something went wrong when receiving number\n");
                            stopped = 1;
                            break;
                        }
                        numbers[i] = atoi(message);
                        printf("%d ", numbers[i]);
                        memset(message, 0, sizeof(message));
                    }
                    printf("\n");

                    int place;
                    char buffer[LENGTH] = {};
                    for(i=0; i<round; i++) { 
                        printf("Guess the place of card-%d : ", numbers[i]);
                        scanf("%d", &place);
                        sprintf(buffer, "%d", place);
                        send(sockfd, buffer, strlen(buffer), 0);
                        bzero(buffer, LENGTH);
                        sleep(1);
                    } 
                } else {
                    stopped = 1;
                }
            } else if(strcmp(message, GAME_COMPLETED_SIG) == 0) {
                // game completed
                isGameCompleted = 1;
                break;
            } else { 
                printf("%s", message);
                str_overwrite_stdout();
                memset(message, 0, sizeof(message));
            }
        } else {
            stopped = 1;
        }

        if(stopped == 1)
            break;
    }

    if(flag || isGameCompleted == 1) {
		printf("\nThank You!\n");
    }
    
    if(stopped == 1) {
        printf("Something went wrong!\n");
    }

	close(sockfd);
	return EXIT_SUCCESS;
}

/* overwrite stdout */
void str_overwrite_stdout() {
    printf("%s", "");
    fflush(stdout);
}

/* trim left of the string */
void str_trim_lf (char* arr, int length) {
    int i;
    for (i = 0; i < length; i++) { // trim \n
        if (arr[i] == '\n') {
        arr[i] = '\0';
        break;
        }
    }
}

/* ctrl + c and exit from program */
void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}