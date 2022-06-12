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
#define MAXLINE 1000000   /* nr. max. octeti de citit cu recv() */
#define MAXHOSTNAME 100 /* nr. max. octeti nume host */
using namespace std;
/**
 *  COMPLETAT FUNCTII DE TRASNMITERE FISIER JSON CATRE SERVER
 *  CREEARE FISIER JSON
 *  STABILIRE DATE DE CONECTARE LA SERVER
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE CLIENT
 */
//============================================================================//
bool send_option_to_server(char option[10], int sockfd)
{
    char line[MAXLINE];
    int size;
    bzero(line, sizeof(line));
    send(sockfd, option, sizeof(&option), 0);
    size = recv(sockfd, &line, sizeof("ok"), 0);
    cout << "recv: " << size << endl;
    cout << "ERRNO: " << errno << " " << strerror(errno) << endl;
    // line[size] = '\0';
    // printf("line %s %c %c %c\n", line, line[0], line[1], line[2]);
    if (strncmp(line, "ok\0", sizeof("ok\0")) == 0)
    {
        // printf("if true %s\n", line);
        return true;
    }
    else
    {
        // printf("if false %s\n", line);
    }
    bzero(line, sizeof(line));
    return false;
}
//============================================================================//
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
            cout << "5. Stergere date\n";
            cout << "6. Exit\n";
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
                    //recv(sockfd, line, MAXLINE, 0);
                    //line[size] =  '\0';
                    int n = read(sockfd, line, MAXLINE);
                    cout << "[INFO] Your data is gonna be printed --->" << line << endl;
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
                    cout << key <<": " << line << endl;
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
            { // Exit
                if (send_option_to_server((char *)"6", sockfd))
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