/*
                                                             TESTO ESAME
Un gruppo di 5 host (A, B, C, D, E) comunica attraverso un meccanismo di broadcast circolare unidirezionale: A verso B, B verso C, .... E verso A. I messaggi scambiati sono su base UDP.
Chi invia un messaggio specifica il mittente ed il destinatario (a livello applicativo). Quando il destinatario riceve il messaggio, non lo inoltra più ma manda un ack di ricezione, che tornerà al mittente seguendo l'anello. Non è
prevista la ritrasmissione in caso di perdita.
I messaggi confermati vengono visualizzati come tali sulla shell del mittente (scegliere il modo ritenuto più opportuno/semplice)
Definire liberamente tutte le caratteristiche che non sono state specificate.
Usare C o C++. Niente grafica
*/

/*                                                 SPIEGAZIONE CODICE
Affinchè il codice funzioni tutte le macchine devono ricevere sulla stessa porta.
Lo stardard del Messaggio da inviare è il seguente -> "Lettera Mittente Lettera Destinatario Messaggio"
Ogni macchina comunica con la successiva cambiando l'indirizzo IP che si trova nella riga 49 (hard coding)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{

    int sockfd, n;
    struct sockaddr_in recive_addr, local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[100];
    char reciveline[100];
    char self = 'C';

    if (argc != 2)
    {
        printf("Errore! Inserire la porta...\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Errore durante la creazione della socket...\n");
        exit(0);
    }

    memset(&local_addr, 0, len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));

    memset(&remote_addr, 0, len);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.104.8", &(remote_addr.sin_addr));
    remote_addr.sin_port = htons(atoi(argv[1]));

    if ((bind(sockfd, (struct sockaddr *)&local_addr, len)) < 0)
    {
        printf("Errore durante la bind...\n");
        exit(0);
    }

    if (fork() != 0)
    {
        // processo padre in ascolto
        for (;;)
        {
            n = recvfrom(sockfd, reciveline, 99, 0, (struct sockaddr *)&recive_addr, &len);
            reciveline[n] = 0;

            if (reciveline[2] == self && strstr(reciveline, "Ack") != NULL)
            {
                printf("Arrivato ack\n");
                continue;
            }

            if (reciveline[2] == self)
            {

                printf("Messaggio: %s\n", reciveline);
                printf("Messaggio arrivato \n");
                strcpy(sendline, "A   Ack\0");
                sendline[2] = reciveline[0];
                sendto(sockfd, sendline, 99, 0, (struct sockaddr *)&remote_addr, len);
            }
            else if (strchr("ABCDE", reciveline[2]) != NULL)
            {
                sendto(sockfd, reciveline, 99, 0, (struct sockaddr *)&remote_addr, len);
            }
        }

        close(sockfd);
    }
    else
    {
        // processo figlio in invio
        for (;;)
        {
            printf("Scrivi il tuo messaggio in questo formato \"X Y MESSAGE\":\t");
            fgets(sendline, 99, stdin);
            sendline[strcspn(sendline, "\n")] = 0;
            sendto(sockfd, sendline, 99, 0, (struct sockaddr *)&remote_addr, len);
        }

        close(sockfd);
    }
}