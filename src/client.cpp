#include <stdio.h>
#include <stdlib.h> // pt. functiile exit()/ abort()
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <iostream>
#include <string.h>
#include <cstring>
//#include <strings.h> /* e mai nou decat string.h, dar nu merge */
/* Definitii pentru programul server TCP */
/* 1. Porturtul TCP utilizat. */
#define CLIENT_TCP_PORT 7979
/* 2. Alte constante */
#define MAXLINE 512 /* nr. max. octeti de citit cu recv() */
#define MAXHOSTNAME 100 /* nr. max. octeti nume host */
using namespace std;
/**
 *  COMPLETAT FUNCTII DE TRASNMITERE FISIER JSON CATRE SERVER
 *  CREEARE FISIER JSON
 *  STABILIRE DATE DE CONECTARE LA SERVER
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE CLIENT
 */


void echoToServer(int sockfd) {
    // mesajul
    char line[MAXLINE];
    int check, option;
    // bucla infinita, se opreste cand clientul a trimis mesajul "adio"
    bzero(line, sizeof(line));
    // send client type
    send(sockfd, "client", sizeof("client"), 0);
    recv(sockfd, &line, MAXLINE, 0);
    cout << line << endl;

    while (1) 
    {            
        cout << "1. Creare cont\n";
        cout << "2. Conectare\n";
        cout << "Optiune: "; cin >> option;
        
        switch (option)
        {
            case 1:
            {
                // Register
                string username, passwd;
                cout << "Introduceti nume de utilizator: ";
                getline(cin >> ws, username);
                cout << "Introduceti parola: ";
                getline(cin >> ws, passwd);

                send(sockfd, username.c_str(), sizeof(username.c_str()), 0);
                send(sockfd, passwd.c_str(), sizeof(passwd.c_str()), 0);
                break;
            }
            case 2:
            {
                // Connect
                string username, passwd;
                
                cout << "Introduceti nume de utilizator: ";
                getline(cin >> ws, username);
                cout << "Introduceti parola: ";
                getline(cin >> ws, passwd);

                send(sockfd, username.c_str(), sizeof(username.c_str()), 0);
                send(sockfd, passwd.c_str(), sizeof(passwd.c_str()), 0);
                // gets ok from server
                recv(sockfd, &line, MAXLINE, 0);
                if (strcmp(line, "ok") == 0)
                {
                    cout << "Conectat!\n";
                    send(sockfd, "adio", sizeof("adio"), 0);
                    // TODO meniu options
                }
                else
                {
                    // EROARE
                    cout << "[ERROR] Server: " << line << endl;
                    send(sockfd, "adio", sizeof("adio"), 0);
                }
                break;
            }
        }



        // printf("Introdu mesajul care va fi trimis: ");
        // scanf("%s", line);
        // // trimitere mesaj
        // write(sockfd, line, sizeof(line));
        // bzero(line, sizeof(line));
        // read(sockfd, line, sizeof(line));
        // // verificare mesaj de deconectare client
        // if (strstr(line,"adio")) {
        //     printf("Deconectare client!");
        //     close(sockfd);
        //     exit(1);

        // }
    }
    close(sockfd);
}



int main(int argc, char *argv[])
{
    int sockfd, rc;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        fprintf(stderr, "Eroare la crearea de socket!");
        exit(0);
    }

    bzero(&serv_addr, sizeof(serv_addr));

    // initializare socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(CLIENT_TCP_PORT);
    
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) 
    {
        fprintf(stderr, "Eroare la conectare!");
        exit(0);
    }

    echoToServer(sockfd);

    close(sockfd);


}