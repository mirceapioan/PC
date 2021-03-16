#include <stdio.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include "helpers.h"
#include <unordered_map>
#include <iostream>
#include <map>

using namespace std;

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int check_prime(int n) {
	for(int d = 2; d < n/2; d++) {
		if(n % d == 0)
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	string current_mail;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, ret, connected_clients = 0;
	unordered_map<int, string> users;
	multimap<string, string> sent_mails;
	socklen_t clilen;
    int ID = 1;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");

	ret = listen(sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(sockfd, &read_fds);
	FD_SET(0, & read_fds);
	fdmax = sockfd;

	while (1) {
		tmp_fds = read_fds; 
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {

				if (i == sockfd) {

					clilen = sizeof(cli_addr);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					DIE(newsockfd < 0, "accept");
					
					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax) { 
						fdmax = newsockfd;
					}

					memset(buffer, 0, strlen(buffer));
					n = recv(newsockfd, buffer, sizeof(buffer), 0);
                    cout << n;
					users[newsockfd] = buffer;

					connected_clients++;
					printf("S-a conectat un nou client pe socketul %d cu id-ul %d\n", newsockfd, connected_clients);
				} //SE FACE O CERERE DE CONECTARE DE LA CLIENT TCP

				else if (i == 0) {
					char command[100];
					fgets(command, 100, stdin);
					if(strcmp(command, "status") == 0) {
						printf ("Connected clients: %d\n", connected_clients); 
						continue;
					}
					else if(strcmp(command, "quit") == 0 || connected_clients == 0) {
						goto END;
					}
					
				}

				else { //SE PRIMESC DATE DE LA UN CLIENT TCP
					// s-au primit date pe unul din socketii de client,
					// asa ca serverul trebuie sa le receptioneze

					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, sizeof(buffer), 0);
					DIE(n < 0, "recv");
                    cout << recv;
					if (n == 0) {
						// conexiunea s-a inchis
						connected_clients--;
						close(i);
						// se scoate din multimea de citire socketul inchis 
						FD_CLR(i, &read_fds);
					} 
					
				}
			}
		}
	}
END:
	close(sockfd);
	return 0;
}