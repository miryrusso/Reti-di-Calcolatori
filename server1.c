// Il server si occupa di reindirizzare i messaggi ricevuti da un client mittente ad un client destinatario.
/*
Il messaggio inviato al server avrà il seguente formato:
<ip destinatario> <porta destinatario> <messaggio>

IL SERVER INVIA SOLO IL MESSAGGIO AL DESTINATARIO INTESO

La sintassi di avvio del programma è ./server <porta di ascolto server>
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 1024
#define MAX_PORT_SIZE 8
#define MAX_MESSAGE_SERVER MAX_MESSAGE_SIZE + MAX_PORT_SIZE + INET_ADDRSTRLEN

int main(int argc, char *argv[])
{
    int sockfd, n;
    // server addr facciamo la bind, client quella dove riceviamo
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char message[MAX_MESSAGE_SERVER];

    if (argc < 2 || atoi(argv[1]) == 0)
    {
        fprintf(stderr, "use: %s <porta di ascolto client> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, len);
    memset(&client_addr, 0, len);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    if ((bind(sockfd, (struct sockaddr *)&server_addr, len)) < 0)
    {
        perror("Errore durante la bind...\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        recvfrom(sockfd, message, MAX_MESSAGE_SERVER, 0, (struct sockaddr *)&client_addr, &len);
        char *ip = strtok(message, " ");
        char *port = strtok(NULL, " ");
        char *message = strtok(NULL, " ");

        struct sockaddr_in dest_addr;
        // ci serve per fare il forwarding al client
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(atoi(port));
        inet_pton(AF_INET, ip, &dest_addr.sin_addr);

        sendto(sockfd, message, strlen(message) + 1, 0, (struct socckaddr_in *)dest_addr, sizeof(dest_addr));
    }

    return 0;
}