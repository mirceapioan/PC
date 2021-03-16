#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/tcp.h>

#include "helpers.h"

void usage(char *file) {
	fprintf(stderr,"Usage %s <ID_Client> <IP_Server> <Port_Server>\n", file);
	exit(0);
}

int main(int argc, char *argv[]) {

    int sockfd, n, ret;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN], sent[BUFLEN];
    char *piece, *piece2, *checker;
    char finish[] = " \n";

    fd_set read_fds;
    fd_set tmp_fds;

    int fdmax;

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    if(argc < 2) {
        usage(argv[0]);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");
    //Deschid un socket

    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
    FD_SET(0, &read_fds);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    ret = inet_aton(argv[1], &serv_addr.sin_addr);
    DIE(ret == 0, "inet_aton");

    ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    DIE(ret < 0, "connect");

    while (1) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        if (FD_ISSET(0, &tmp_fds)) { // citeste de la tastatura

            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);

            piece = strtok(buffer, finish);
            
            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            }

            if ( strcmp(piece, "subscribe") == 0) {
                piece = strtok(NULL, finish);
                
                if (piece != NULL && strlen(piece) < 50)  {
                    piece2 = strtok(NULL, finish);

                    if (piece2 != NULL) {
                        checker = strtok(NULL, finish);

                        if (checker == NULL) {
                            memset(sent, 0 , BUFLEN);
							sprintf(sent, "%s subscribe %s %s ", argv[1], piece, piece2);
							/* Trimitere mesaj*/
							ret = send(sockfd, sent, strlen(sent), 0);
							DIE(ret < 0, "send");
							/* Afisare feedback*/
							printf("Sending command:%s\n", sent + strlen(argv[1]));
                        }
                    }
                }
            } else if (strcmp(piece, "unsubscribe") == 0) {
                piece = strtok(NULL, finish);
                if (piece != NULL && strlen(piece) < 50)  {
                    checker = strtok(NULL, finish);

                    if (checker == NULL) {
                        memset(sent, 0 , BUFLEN);
						sprintf(sent, "%s unsubscribe %s ", argv[1], piece);
						/* Trimitere mesaj*/
						ret = send(sockfd, sent, strlen(sent), 0);
						DIE(ret < 0, "send");
                    }

                }

            } else if (strcmp(piece, "exit") == 0) {
                checker = strtok(NULL, finish);

                if (checker == NULL) {
                    memset(sent, 0 , BUFLEN);
					sprintf(sent, "%s unsubscribe %s ", argv[1], piece);
					/* Trimitere mesaj*/
					ret = send(sockfd, sent, strlen(sent), 0);
					DIE(ret < 0, "send");
                }

                
            } else {
                //Comanda inexistenta
                printf("Comanda nu exista");
            }

            // se trimite mesaj la server
            n = send(sockfd, buffer, strlen(buffer), 0);
            DIE(n < 0, "send");

        } else {
            //raspuns server
            memset(buffer, 0, BUFLEN);
            recv(sockfd, buffer, BUFLEN, 0);
            printf("%s\n", buffer);
        }
    }
    close(sockfd);
    return 0;
}
