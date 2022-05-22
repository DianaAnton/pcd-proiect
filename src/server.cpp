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
#include <pthread.h>
// #include "../lib/json/include/nlohmann/json.hpp"
#include <iostream>

// for convenience
// using json = nlohmann::json;
using namespace std;

#define SERV_TCP_PORT 7979
#define MAXLINE 1024
#define MAXHOSTNAME 100
#define NRTHREADS 100

pthread_mutex_t mutex;
bool admin_connection = false;
int connections = 0;
using namespace std;

/**
 *  COMPLETAT FUNCTII DE PARSARE FISIERE JSON, apoi stocate local
 *  IN FUNCTIE DE OPTIUNEA SELECTATA, SE VOR STERGE, CREEA, MODIFICA 
 *  fisiere JSON
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE SERVER  
 */
 
//  struct json {
//      "Book": {
//             "Width":  int,
//             "Height": int,
//             "Title":  string,
//             "isBiography": bool,
//             "NumOfCopies": int,
//             "LibraryIDs": int[]
//         }
//     }

//  {[
//     "client_1": {
//       "iban": "sgdaga",
//       "key": "sdga"
//     },
//     "client_2": {
//       "iban": "sgdaga",
//       "key": "sdga"
//     }
//    ]
//  }

// int parseJson() {
//   json = testJson
// }
//============================================================================//
void* admin_handler(void* args)
{
    int recv_msg_len;
    char msg[MAXLINE];
    int *sock_fd =(int *)args;
    pthread_mutex_lock(&mutex);
    admin_connection = true;
    pthread_mutex_unlock(&mutex);

    cout << "Admin conectat!" << endl;
    char mssg[10] = "admin";
    send(*sock_fd, mssg, 10, 0);

    while (recv_msg_len = recv(*sock_fd, &msg, MAXLINE, 0)) 
    {
      cout << "MSG:" << msg << endl;
      if (strcmp(msg, "adio"))
      {
        close(*sock_fd);
      }
      // aici ar trebui sa se intample magie cu transmitere de fisiere bla bla
    }

    pthread_mutex_lock(&mutex);
    admin_connection = false;
    pthread_mutex_unlock(&mutex);
    return nullptr;
}
//============================================================================//
void create_user(char *username, char *passwd)
{
  printf("[DEBUG] Create User received: %s %s\n", username, passwd);
  // TODO salvare in fisier + creare id?
}
//----------------------------------------------------------------------------//
void login_user(char *username, char *passwd)
{
  printf("[DEBUG] Login User received: %s %s\n", username, passwd);
  // TODO verificare din fisier
}
//----------------------------------------------------------------------------//
void* client_handler(void* args) 
{
    int recv_msg_len, option, error; 
    int *sock_fd = (int *) args;
    char recv_msg[MAXLINE];
    bool login = false;

    cout << "[INFO] Client connected!" << endl;
    char send_msg[MAXLINE] = "te-ai conectat la server!";
    //bzero(mssg, MAXLINE);
    error = send(*sock_fd, send_msg, MAXLINE, 0);
    cout << "send: " << error << endl;
    cout << "ERRNO: " << errno << " " << strerror(errno) << endl; 
    
    while(1) 
    { // recv option not connected
      recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
      printf("[DEBUG] Message received: %s\n", recv_msg);
      error = send(*sock_fd, "ok", sizeof("ok"), 0);
      cout << "send: " << error << endl;
      cout << "ERRNO: " << errno << " " << strerror(errno) << endl; 
      printf("[DEBUG] Message sent: ok\n");
      option = atoi(recv_msg);
      if (!login)
      {
        switch (option)
        {
          case 1:
          {
            bzero(recv_msg, sizeof(recv_msg));
            recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
            recv_msg[recv_msg_len] = '\0';
            
            // split message
            char *username;
            username = strtok(recv_msg, "|");
            char *passwd = strtok(NULL, "\0");
            create_user(username, passwd);
            break;
          }
          case 2:
          {
            bzero(recv_msg, sizeof(recv_msg));
            recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
            recv_msg[recv_msg_len] = '\0';
            printf("[DEBUG] Message received: %s\n", recv_msg);
            
            // split message
            char *username;
            username = strtok(recv_msg, "|");
            char *passwd = strtok(NULL, "\0");
            login_user(username, passwd);
            break;
          }
          case 3:
          {
            send(*sock_fd, "Te-ai deconectat de la server!", 
                sizeof("Te-ai deconectat de la server!"), 0);
            close(*sock_fd);
            break;
          }
        }
      }
      else if (login)
      {
        // aici ar trebui sa se intample magie cu transmitere de fisiere bla bla
        switch (option)
        {
          case 1:
          { // add data
            
            break;
          }
          case 2:
          { // read data
            
            break;
          }
          case 3:
          { // update data
            
            break;
          }
          case 4:
          { // delete data
            
            break;
          }
          case 5:
          { // exit
            
            break;
          }
        }
      }
      

      
    }
} 
//============================================================================//
int main() 
{    
    struct sockaddr_in cli_addr, serv_addr;
    int msg_len, sock_fd, new_sock_fd, connections;
    struct hostent *he;
    char msg[MAXLINE];
    pthread_t thds[NRTHREADS];
    char *NumeServer = (char *) "pcd proiect"; // numele serverului luat din argv[0]
    char NumeHostServer[MAXHOSTNAME];

    //pthread_mutex_init(&mutex, NULL);

    gethostname(NumeHostServer, MAXHOSTNAME);
    printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);

    he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1

    bcopy ( he->h_addr, &(serv_addr.sin_addr), he->h_length);
    printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
    inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
    /* numele procesului server luat de pe linia de comanda */

    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        fprintf(stderr,"EROARE server: nu pot sa deschid stream socket \n");

    int check = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(int)) < 0)
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");
    


    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_TCP_PORT);    


    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        fprintf(stderr, "EROARE server: nu pot sa asignez un nume adresei locale \n");

    printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer, ntohs(serv_addr.sin_port));

    if (listen(sock_fd, 100) < 0) 
    {
      fprintf(stderr, "Eroare!");
      exit(0);
    }

    while(1) 
    {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      socklen_t cli_addr_size = sizeof(cli_addr);
      new_sock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_addr_size);
    
      if (new_sock_fd < 0)
      {
        fprintf(stderr, "[EROARE SERVER]: accept()\n");
        exit(1);
      }
      msg_len = recv(new_sock_fd, &msg, MAXLINE, 0);
      printf("[INFO] Client Type: %s\n", msg);
      msg[msg_len] = '\0';
      
      if (strncmp(msg, "admin\0", sizeof("admin\0")) == 0)
      { // start thread cu functie admin
        if (admin_connection == false)
        { // daca nu mai e conectat unul deja
          pthread_create(&thds[connections], NULL, admin_handler, 
                        (void *) &new_sock_fd);
          connections++;
        }
        else
        { // daca e conectat, send error to client
          char temp_msg[100] = "[ERROR] Un admin e deja conectat.\n";
          send(new_sock_fd, temp_msg, sizeof(temp_msg), 0);
          close(new_sock_fd);
        }
      }
      else if (strncmp(msg, "client\0", sizeof("client\0")) == 0)
      {
      cout << "aici\n";
        pthread_create(&thds[connections], NULL, client_handler, 
                        (void *) &new_sock_fd); 
      cout << "aici2\n";
        pthread_join(thds[connections], NULL);
        connections++;
      }
      else
      {
        printf("[ERROR] Unkown connection. Closing...\n");
        close(new_sock_fd);
      }



      // while (msg_len = recv(new_sock_fd, &msg, MAXLINE,0))
      // {
      //   printf("De la client: %s \n", msg);
      //   msg[MAXLINE] = '\0';
      //   send(new_sock_fd, msg, strlen(msg), 0);

      //   if (strstr(msg, "adio")) // adio este in line ???
      //   { /* la introducere "adio" iesire din ciclu */
      //     close(new_sock_fd); /* proces copil / close socket nou*/
      //     printf("\n---TCPServer ___client PID= %ld deconectat\n", (long)getpid());
      //     close(new_sock_fd);
      //     exit(0); /* copilul ramane "defunct" si nu dispare daca parintele nu zice wait(NULL)*/
      //   }
      //   else
      //   {
      //     bzero((char *)&cli_addr, sizeof(cli_addr));
      //   };
      // }
    
    }
    close(sock_fd);
    close(new_sock_fd);
    
}