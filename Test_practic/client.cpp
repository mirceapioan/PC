#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <time.h>
#include <string.h>

using namespace std;

template <class Container>
void split1(const std::string& str, Container& cont)
{
    std::istringstream iss(str);
    std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(cont));
}

void usage(char *file)
{
	fprintf(stderr, "Usage: %s id server_address server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	char receive[BUFLEN];

	if (argc < 4) {
		usage(argv[0]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	strcpy(buffer, argv[1]);
	argv[1][strlen(argv[1])] = '\0';
	n = send(sockfd, buffer, strlen(buffer), 0);
	DIE(n < 0, "ERROR SENDING ID");

	fd_set read_fds;
	fd_set tmp_fds;
	int fdmax, i;

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	FD_SET(sockfd, &read_fds);
	FD_SET(0, & read_fds);
	fdmax = sockfd;

	while (1) {

		tmp_fds = read_fds;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");
		
		if(FD_ISSET(0, &tmp_fds)) { 
  				// se citeste de la tastatura
				memset(buffer, 0, BUFLEN);
				fgets(buffer, BUFLEN - 1, stdin);

				if (strncmp(buffer, "quit", 4) == 0) {
					break;
				} else if (strncmp(buffer, "set", 3) == 0) {
                    
                } else if (strncmp(buffer, "guess", 5) == 0) {
                    
                } else if (strncmp(buffer, "status", 6) == 0) {
                    int status = 3;
                    string temp;
                    //strcpy(temp.c_str(), buffer);
                    sprintf(buffer, " %s", buffer);

				    n = send(sockfd, buffer, strlen(buffer), 0);
				  
				    cout << status << endl;

				memset(buffer, 0, BUFLEN);
				n = recv(sockfd, buffer, sizeof(buffer), 0);
				cout << buffer << endl;
                }
				string buffer_str(buffer);
								
				vector<string> tokens;
				split1(buffer_str, tokens);




		} else if(FD_ISSET(sockfd, &tmp_fds)) {	
				memset(buffer, 0,BUFLEN);
				n = recv(sockfd, buffer, sizeof(buffer), 0);
				if(n == 0) {
					goto END;
				}
				printf("%s\n", buffer);
		}
    }
END:
	close(sockfd);
	return 0;
}