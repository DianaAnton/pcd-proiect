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
#include <pthread.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include "../lib/json/single_include/nlohmann/json.hpp"
#include "server_functions.hpp"

//#include <strings.h> /* e mai nou decat string.h, dar nu merge */
/* Definitii pentru programul server TCP */
/* 1. Porturtul TCP utilizat. */
#define ADMIN_TCP_PORT 7979
/* 2. Alte constante */
#define MAXLINE 1000000   /* nr. max. octeti de citit cu recv() */
#define MAXHOSTNAME 100 /* nr. max. octeti nume host */
using namespace std;
using json = nlohmann::json;
//pthread_mutex_t file_mutex;

/**
 *  COMPLETAT FUNCTII DE TRASNMITERE FISIER JSON CATRE SERVER
 *  CREEARE FISIER JSON
 *  STABILIRE DATE DE CONECTARE LA SERVER
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE ADMIN
 */
//============================================================================//
void echoToServer(int sockfd)
{
    // mesajul
    char line[MAXLINE];
    int check, option, size;
    bool connected = false;
    int client_id = 0;

    bzero(line, sizeof(line));
    // send admin type
    send(sockfd, "admin", sizeof("admin"), 0);
    // bzero(line, sizeof(line));
    recv(sockfd, &line, MAXLINE, 0);
    cout << line << endl;

    while (1)
    {
        if (connected == false)
        {
            cout << "1. Creare cont\n";
            cout << "2. Conectare\n";
            cout << "3. Adio\n";
            cout << "Optiune: ";
            cin >> option;

            switch (option)
            {
                case 1:
                {
                    // send the server the option that was selected
                    if (send_option_to_server((char *)"1", sockfd))
                    {
                        // Register
                        string username, passwd;
                        cout << "Introduceti nume de utilizator: ";
                        getline(cin >> ws, username);
                        send(sockfd, username.c_str(), strlen(username.c_str()), 0);
                        cout << "Introduceti parola: ";
                        getline(cin >> ws, passwd);
                        send(sockfd, passwd.c_str(), strlen(passwd.c_str()), 0);
                        // string info = username + "|" + passwd;
                        printf("Dupa send: %s %s \n", username.c_str(), passwd.c_str());
                        // send(sockfd, info.c_str(), sizeof(info.c_str()), 0);
                    }
                    else
                    {
                        printf("[ERROR]\n");
                    }
                    break;
                }
                case 2:
                {
                    // send the server the option that was selected
                    if (send_option_to_server((char *)"2", sockfd))
                    {
                        string username, passwd;
                        cout << "Introduceti nume de utilizator: ";
                        getline(cin >> ws, username);
                        send(sockfd, username.c_str(), strlen(username.c_str()), 0);
                        cout << "Introduceti parola: ";
                        getline(cin >> ws, passwd);
                        send(sockfd, passwd.c_str(), strlen(passwd.c_str()), 0);

                        // gets ok from server
                        bzero(line, sizeof(line));
                        size = recv(sockfd, line, MAXLINE, 0);
                        line[size] = '\0';
                        if (strcmp(line, "ok\0") == 0)
                        {
                            cout << "Logged in!" << endl;
                            connected = true;
                        }
                        else
                        {
                            cout << line << endl;
                        }
                    }
                    else
                    { // EROARE
                        cout << "[ERROR] Server: " << line << endl;
                        // send(sockfd, "adio", sizeof("adio"), 0);
                    }
                    break;
                }
                case 3:
                { // Exit
                    if (send_option_to_server((char *)"3", sockfd))
                    {
                        bzero(line, sizeof(line));
                        size = recv(sockfd, line, MAXLINE, 0);
                        line[size] = '\0';
                        printf("%s\n", line);
                        // close(sockfd);
                        exit(0);
                    }
                    break;
                }
            }
        }
        else
        {
            cout << "1. Meniu pentru dumneavoastra\n";
            cout << "2. Meniu pentru alt client\n";
            cout << "3. Log out\n";
            cout << "4. Exit\n";
            cin >> option;
            switch (option)
            {
                case 1:
                { // Admin
                    if (send_option_to_server((char *)"1", sockfd))
                    {
                        admin_menu(sockfd);
                    }
                    else
                    {
                        printf("[ERROR]\n");
                    }
                    break;
                }
                case 2:
                { // Other client
                    if (send_option_to_server((char *)"2", sockfd))
                    {
                        string username;
                        cout << "Introdu numele utilizatorului: ";
                        getline(cin >> ws, username);

                        int s = write(sockfd, username.c_str(), username.length() + 1);

                        admin_menu(sockfd);
                    }
                    else
                    {
                        printf("[ERROR]\n");
                    }
                    break;                
                }
                case 3:
                { //  Back
                    if (send_option_to_server((char *)"3", sockfd))
                    {
                        break;
                    }
                    else
                    {
                        printf("[ERROR]\n");
                    }
                    break; 
                }
                case 4:
                { // Exit
                    if (send_option_to_server((char *)"4", sockfd))
                    {
                        bzero(line, sizeof(line));
                        size = recv(sockfd, line, MAXLINE, 0);
                        line[size] = '\0';
                        printf("%s\n", line);
                        close(sockfd);
                        exit(0);
                    }
                    break;

                }             
            }                    
        }
        
            
            
        
    }
    close(sockfd);
}

//============================================================================//
int main(int argc, char *argv[])
{
    int sockfd, rc;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        fprintf(stderr, "Eroare la crearea de socket!");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));

    // initializare socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(ADMIN_TCP_PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        fprintf(stderr, "Eroare la conectare!");
        exit(1);
    }
    echoToServer(sockfd);
    close(sockfd);
}
//============================================================================//