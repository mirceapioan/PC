#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    // Ex 1.2: POST dummy and print response from main server
    // Ex 2: Login into main server
    // Ex 3: GET weather key from main server
    /// - Bonusuri
    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/dummy", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    char **from_data = malloc(2 * sizeof(char*));
    from_data[0] = malloc(BUFLEN);
    from_data[1] = malloc(BUFLEN);
    strcpy(from_data[0], "username=student");
    strcpy(from_data[1], "password=student");

    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/dummy", "application/x-www-form-urlencoded", from_data, 2, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/auth/login", "application/x-www-form-urlencoded", from_data, 2, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    char **cookie = malloc(1 * sizeof(char*));
    cookie[0] = malloc(2000 * sizeof(char));
    strcpy(cookie[0], "connect.sid=s%3ArSbqZUGXL1BB4_JIz2TpSOQHlkr9mYMX.Hv7FXOqZ0ig8rQXeP8cLbu1WSJMdrKkBOh59wg3hwpM");

    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/weather/key", NULL, cookie, 1);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/auth/logout", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    close_connection(sockfd);

    return 0;
}
