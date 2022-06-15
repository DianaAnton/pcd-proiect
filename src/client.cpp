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
#include "../lib/json/single_include/nlohmann/json.hpp"
#include "server_functions.hpp"
//#include <strings.h> /* e mai nou decat string.h, dar nu merge */
/* Definitii pentru programul server TCP */
/* 1. Porturtul TCP utilizat. */
#define CLIENT_TCP_PORT 7979
/* 2. Alte constante */
#define MAXLINE 1000000   /* nr. max. octeti de citit cu recv() */
#define MAXHOSTNAME 100 /* nr. max. octeti nume host */
int sockfd;
using namespace std;
// for convenience
using json = nlohmann::json;
/**
 *  COMPLETAT FUNCTII DE TRASNMITERE FISIER JSON CATRE SERVER
 *  CREEARE FISIER JSON
 *  STABILIRE DATE DE CONECTARE LA SERVER
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE CLIENT
 */
//============================================================================//
void signal_sigint(int signal)
{
    printf("Adio!\n");
    close(sockfd);
    exit(0);
}
//----------------------------------------------------------------------------//
void echoToServer(int sockfd)
{
    // mesajul
    char line[MAXLINE];
    int check, option, size;
    bool connected = false;

    bzero(line, sizeof(line));
    // send client type
    send(sockfd, "client", sizeof("client"), 0);
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
                        // send to server
                        send(sockfd, passwd.c_str(), strlen(passwd.c_str()), 0);
                        // reset buffer and wait for messge
                        bzero(line, sizeof(line));
                        // e bun, primeste ok din functie
                        recv(sockfd, &line, MAXLINE, 0);
                        if (strstr(line, "ok") == 0)
                        {
                            cout << "Utilizator creat cu succes!" << endl;
                        }
                        else
                        {
                            cout << line;
                        }
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

                        sleep(1);
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
                {
                    if (send_option_to_server((char *)"3", sockfd))
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
        else
        {
            cout << "1. Adaugare date noi\n";
            cout << "2. Vizualizare date\n";
            cout << "3. Cauta o valoare specifica\n";
            cout << "4. Modificare date\n";
            cout << "5. Stergere TOATE datele\n";
            cout << "6. Stergere o pereche anume\n";
            cout << "7. Log out\n";
            cout << "8. Exit\n";
            cout << "Optiune: ";
            cin >> option;
            switch (option)
            {
            case 1:
            { // Add new data
                if (send_option_to_server((char *)"1", sockfd))
                {
                    string key, value;
                    cout << "Introduceti numele secretului: ";
                    getline(cin >> ws, key);
                    //send(sockfd, key.c_str(), sizeof(key.c_str()), 0);
                    int n = write(sockfd, key.c_str(), key.length() + 1);
                    cout << "Introduceti valoarea: ";
                    getline(cin >> ws, value);
                    sleep(1);
                    //send(sockfd, value.c_str(), sizeof(value.c_str()), 0);
                    int s = write(sockfd, value.c_str(), value.length() + 1);
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 2:
            { // Read all data
                if (send_option_to_server((char *)"2", sockfd))
                {
                    bzero(line, MAXLINE);
                    int n = read(sockfd, &line, MAXLINE);
                    cout << line << endl;
                    // line[n] = "\n";
                    json data = json::parse(line);
                    cout << endl;
                    cout << data.dump(4) << endl;
                    cout << endl;
                    // cout << "[INFO] Your data is gonna be printed --->" << line << endl;
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 3:
            { // Read specific data
                if (send_option_to_server((char *)"3", sockfd))
                {
                    string key;
                    cout << "Introduceti numele secretului: ";
                    getline(cin >> ws, key);
                    cout << endl;
                    send(sockfd, key.c_str(), sizeof(key.c_str()), 0);
                    sleep(2);
                    bzero(line, sizeof(line));
                    recv(sockfd, &line, MAXLINE, 0);
                    json obj;
                    obj[key] = line;
                    // cout << key <<": " << line << endl;
                    cout << obj.dump(4) << endl;
                    cout << endl;
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 4:
            { // Update data
                if (send_option_to_server((char *)"4", sockfd))
                {
                    string key, value;
                    cout << "Introduceti numele secretului: ";
                    getline(cin >> ws, key);
                    //send(sockfd, key.c_str(), sizeof(key.c_str()), 0);
                    int n = write(sockfd, key.c_str(), key.length() + 1);
                    cout << "Introduceti valoarea: ";
                    getline(cin >> ws, value);
                    sleep(1);
                    int s = write(sockfd, value.c_str(), value.length() + 1);
                    sleep(2);
                    bzero(line, sizeof(line));
                    read(sockfd, &line, MAXLINE);
                    cout << line;
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 5:
            { // Delete data
                if (send_option_to_server((char *)"5", sockfd))
                {
                    printf("[INFO] You are going to delete all data....\n");
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 6:
            { // Delete specific pair
                if (send_option_to_server((char *)"6", sockfd))
                {
                    string key;
                    cout << "Introduceti numele secretului: ";
                    getline(cin >> ws, key);
                    cout << endl;
                    send(sockfd, key.c_str(), sizeof(key.c_str()), 0);
                    sleep(2);
                    bzero(line, sizeof(line));
                    read(sockfd, &line, MAXLINE);
                    cout << line;
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 7:
            { // Log out
                if (send_option_to_server((char *)"7", sockfd))
                {
                    connected = false;
                }
                else
                {
                    printf("[ERROR]\n");
                }
                break;
            }
            case 8:
            { // Exit
                if (send_option_to_server((char *)"8", sockfd))
                {
                    connected = false;
                    close(sockfd);
                    exit(0);
                }
                else
                {
                    printf("[ERROR]\n");
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
    int rc;
    struct sockaddr_in serv_addr, cli_addr;
    signal(SIGINT, signal_sigint);

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
    serv_addr.sin_port = htons(CLIENT_TCP_PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        fprintf(stderr, "Eroare la conectare!");
        exit(1);
    }
    echoToServer(sockfd);
    close(sockfd);
}
//============================================================================//