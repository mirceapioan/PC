/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "helpers.h"

void usage(char*file)
{
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];


	/*Deschidere socket*/
	int s = socket(PF_INET, SOCK_DGRAM, 0);
	if(s < 0)
		perror("Socket creation failed");
	
	/* Deschidere fisier pentru citire */
	to_station.sin_family = AF_INET;
	to_station.sin_port = htons(1999);
	inet_aton("127.0.0.1", &to_station.sin_addr);
	DIE((fd=open(argv[3],O_RDONLY))==-1,"open file");
	
	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
	int size_of_data = 0;
	while((size_of_data = read(fd, buf, BUFLEN))){
		int se = sendto(s, buf, size_of_data, 0, (struct sockaddr*)
				&to_station, sizeof(to_station));
		if(se < 0){
			perror("Sending error");
		}
		usleep( 10 );
	}
	
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	


	/*Inchidere socket*/
	close(s);
	
	/*Inchidere fisier*/
	close(fd);
	return 0;
}
