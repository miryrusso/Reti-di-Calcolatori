// Il client vuole parlare con un altro client
// Invece di comunicare direttamente, avvia una connessione al suo server di riferimento.
/*Questo si occuperà di ridirigere il messaggio al client destinatario.
Il messaggio inviato al server avrà il seguente formato:
<ip destinatario> <porta destinatario> <messaggio>

La sintassi di avvio del programma è ./client <porta di ascolto> <ip server di riferimento> <porta server di riferimento>
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 1024
#define MAX_PORT_SIZE 8
#define MAX_MESSAGE_SERVER 1024 + 8 + INET_ADDRSTRLEN

void padre(int sockfd, struct sockaddr_in *server_addr)
{
    // si occupa di mandare il messaggio
    while (1)
    {
        // iplo uso per leggere e message per scrivere il messaggio e un altro per inviarlo
        char ip[INET_ADDRSTRLEN], message[MAX_MESSAGE_SIZE], msg_to_server[MAX_MESSAGE_SERVER], port[MAX_PORT_SIZE];

        printf("A chi vuoi mandare il messaggio? Indica indirizzo ip\n");
        fgets(ip, INET_ADDRSTRLEN, stdin);
        ip[strlen(ip) - 1] = '\0';

        printf("su quale porta ascolterà il client destinatario?\n");
        fgets(port, MAX_PORT_SIZE, stdin);
        port[strlen(port) - 1] = '\0';

        printf("Quale messaggio?\n");
        fgets(message, MAX_MESSAGE_SERVER, stdin);
        message[strlen(message) - 1] = '\0';

        sendto(sockfd, msg_to_server, strlen(msg_to_server) + 1, 0, (struct socckaddr_in *)server_addr, sizeof(struct sockaddr_in));

        // formatto la stringa come ip spazio messaggio
        sprintf(msg_to_server, "%s %s", ip, message);
    }
}

void figlio(int sockfd)
{
    char message[MAX_MESSAGE_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    while (1)
    {
        recvfrom(sockfd, message, MAX_MESSAGE_SIZE, 0, (struct sockaddr_in *)&sender_addr, &len);
        printf("messaggio ricevuto: %s\n", message);
        if (strcmp(message, "exit") == 0)
            return;
    }
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if (argc < 4 || atoi(argv[3]) == 0)
    {
        fprintf(stderr, "use: %s <porta di ascolto client> <ip server di riferimento> <porta server >\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, len);
    memset(&client_addr, 0, len);

    server_addr.sin_family = client_addr.sin_family = AF_INET;

    // settiamo l'indirizzo del server
    inet_pton(AF_INET, argv[2], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[3]));

    // settiamo l'indirizzo del client in ascolto
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    if ((bind(sockfd, (struct sockaddr *)&client_addr, len)) < 0)
    {
        printf("Errore durante la bind...\n");
        exit(0);
    }

    int pid = fork();

    if (pid == 0)
    {
        figlio(sockfd);
        close(sockfd);
    }
    else if (pid > 0)
    {
        padre(sockfd, &server_addr);
        close(sockfd);
    }
    else
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
}