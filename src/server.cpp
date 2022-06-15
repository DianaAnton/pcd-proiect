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
#include "../lib/json/single_include/nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include "server_functions.hpp"

using namespace std;
// for convenience
using json = nlohmann::json;

#define UNIX_PORT 7979
#define SERV_TCP_PORT 7979
#define INET_PORT 7980
#define MAXLINE 50000
#define MAXHOSTNAME 100
#define NRTHREADS 100
pthread_mutex_t admin_mutex;
int sock_fd;

// pthread_mutex_t mutex;
// bool admin_connection = false;
// int connections = 0;
using namespace std;
// int clientId = 1;
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
//============================================================================//
//----------------------------------------------------------------------------//
// void *create_unix()
// {
//    struct sockaddr_un name;
//   int sock; /* UNIX Socket descriptor */
//   size_t size;
  
//   /* Create the socket. */
//   sock = socket (PF_LOCAL, SOCK_DGRAM, 0);
//   if (sock < 0) {
// //    perror ("unix-common: socket error");
//     pthread_exit (NULL);
//   }
  
//   name.sun_family = AF_LOCAL; /* Set ADDRESS Family */
//   strncpy (name.sun_path, filename, sizeof (name.sun_path));
//   /* Create SOCKET Path info */
//   name.sun_path[sizeof (name.sun_path) - 1] = '\0';
  
//   size = (offsetof (struct sockaddr_un, sun_path)
// 	  + strlen (name.sun_path) + 1);
//   /* You can use size = SUN_LEN (&name) ; instead */
  
//   /* Now BIND the socket */
//   if (bind (sock, (struct sockaddr *) &name, size) < 0) {
// //    perror ("bind");
//     pthread_exit (NULL);
//   }
  
//   /* And RETURN success :) */
//   return sock;
// }

// void *unix_main (void *args) {
//   char *socket = (char *) args ;

//   if (unix_socket (socket)) { 
//  /*
//     pthread_mutex_lock (&curmtx) ; // Protect CURSES usage!!!
//     attron (COLOR_PAIR(1)) ;
//     mvwprintw (mainwnd, LINES-4, 2, "Socket UNIX (%s) created", socket) ;
//     attroff (COLOR_PAIR(1)) ;
// //    wrefresh (mainwnd) ;
//     pthread_mutex_unlock (&curmtx) ;
//     */
//   }

//   pthread_exit (NULL) ;
// }
//----------------------------------------------------------------------------//
// void *create_inet()
// {
//   struct sockaddr_in cli_addr, serv_addr;
//   int msg_len, sockfd, new_sockfd, connections;
//   struct hostent *he;
//   char msg[MAXLINE];
//   pthread_t thds[NRTHREADS];
//   pthread_t unix_thread, inet_thread;
//   char *NumeServer = (char *)"pcd proiect"; // numele serverului luat din argv[0]
//   char NumeHostServer[MAXHOSTNAME];

//   // pthread_mutex_init(&mutex, NULL);

//   gethostname(NumeHostServer, MAXHOSTNAME);
//   printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);

//   he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1

//   bcopy(he->h_addr, &(serv_addr.sin_addr), he->h_length);
//   printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
//          inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
//   /* numele procesului server luat de pe linia de comanda */

//   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//     fprintf(stderr, "EROARE server: nu pot sa deschid stream socket \n");

//   int check = 1;
//   if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(int)) < 0)
//     fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");

//   bzero((char *)&serv_addr, sizeof(serv_addr));
//   serv_addr.sin_family = AF_INET;
//   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//   serv_addr.sin_port = htons(INET_PORT);

//   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//     fprintf(stderr, "EROARE server: nu pot sa asignez un nume adresei locale \n");

//   printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer, ntohs(serv_addr.sin_port));

//   if (listen(sockfd, 100) < 0)
//   {
//     fprintf(stderr, "Eroare!");
//     exit(0);
//   }

//   while (1)
//   {
//     bzero((char *)&cli_addr, sizeof(cli_addr));
//     socklen_t cli_addr_size = sizeof(cli_addr);
//     new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addr_size);

//     if (new_sockfd < 0)
//     {
//       fprintf(stderr, "[EROARE SERVER]: accept()\n");
//       exit(1);
//     }
//     msg_len = recv(new_sockfd, &msg, MAXLINE, 0);
//     printf("[INFO] Client Type: %s\n", msg);
//     msg[msg_len] = '\0';

//     if (strncmp(msg, "admin\0", sizeof("admin\0")) == 0)
//     { // start thread cu functie admin
//       if (admin_connection == false)
//       { // daca nu mai e conectat unul deja
//         pthread_create(&thds[connections], NULL, admin_handler,
//                        (void *)&new_sockfd);
//         connections++;
//       }
//       else
//       { // daca e conectat, send error to client
//         char temp_msg[100] = "[ERROR] Un admin e deja conectat.\n";
//         send(new_sockfd, temp_msg, sizeof(temp_msg), 0);
//         close(new_sockfd);
//       }
//     }
//     else if (strncmp(msg, "client\0", sizeof("client\0")) == 0)
//     {
//       // cout << "aici\n";
//       pthread_create(&thds[connections++], NULL, client_handler,
//                      (void *)&new_sockfd);
//       // cout << "aici2\n";
//     }
//     else
//     {
//       printf("[ERROR] Unkown connection. Closing...\n");
//       close(new_sockfd);
//     }
//   }
//   close(sockfd);
//   close(new_sockfd);
// }
//----------------------------------------------------------------------------//
void signal_sigint(int signal)
{
    printf("Adio!\n");
    close(sock_fd);
    exit(0);
}
//----------------------------------------------------------------------------//
void *admin_handler(void *args)
{
  pthread_mutex_lock(&admin_mutex);
  int recv_msg_len, option, error;
  int *sockfd = (int *)args;
  char recv_msg[MAXLINE];
  bool login = false;
  int client_id = 0;
  int admin_id = 0;
  // pthread_mutex_lock(&mutex);
  cout << "[INFO] Admin connected!" << endl;
  char send_msg[MAXLINE] = "Te-ai conectat la server!";
  // pthread_mutex_unlock(&mutex);
  // bzero(mssg, MAXLINE);
  error = send(*sockfd, send_msg, MAXLINE, 0);
  cout << "[DEBUG] Send: " << error << endl;
  cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;

  while (1)
  { // recv option not connected
    recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
    printf("[DEBUG] Message received: %s\n", recv_msg);
    error = send(*sockfd, "ok\0", sizeof("ok\0"), 0);
    // cout << "[DEBUG] Send: " << error << endl;
    cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;
    // printf("[DEBUG] Message sent: %s\n");

    // convert to int
    option = atoi(recv_msg);
    bzero(recv_msg, sizeof(recv_msg));

    if (login == false)
    {
      switch (option)
      { //-----------------------Switch not logged in ------------------------//
        case 1:
        {
          char username[100];
          char passwd[100];
          bzero(recv_msg, sizeof(recv_msg));

          // recv username
          recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
          // store the username
          for (int i = 0; i < recv_msg_len; i++)
          {
            username[i] = recv_msg[i];
          }
          printf("[DEBUG] Username :%s \n", username);
          // reset buffer
          bzero(recv_msg, sizeof(recv_msg));

          // recv passwd
          recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
          // store the passwd
          for (int i = 0; i < recv_msg_len; i++)
          {
            passwd[i] = recv_msg[i];
          }
          printf("[DEBUG] Paswd:%s \n", passwd);

          // create child to execute function
          pid_t child = fork();
          if (child == 0)
          { // child
            create_user(username, passwd);
            exit(0);
          }
          else if (child > 1)
          { // parent wait for child
            wait(NULL);
          }
          break;
        }
        case 2:
        {
          char username[100];
          char passwd[100];
          bzero(recv_msg, sizeof(recv_msg));

          // recv username
          recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
          // store the username
          for (int i = 0; i < recv_msg_len; i++)
          {
            username[i] = recv_msg[i];
          }
          printf("[DEBUG] Username :%s \n", username);
          // reset buffer
          bzero(recv_msg, sizeof(recv_msg));

          // recv passwd
          recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
          // store the passwd
          for (int i = 0; i < recv_msg_len; i++)
          {
            passwd[i] = recv_msg[i];
          }
          printf("[DEBUG] Paswd:%s \n", passwd);

          int status;
          // create child to execute function
          pid_t child = fork();
          if (child == 0)
          { // child
            if (login_user(username, passwd))
            {
              cout << "[INFO] Logged in!\n";
              send(*sockfd, "ok\0", sizeof("ok\0"), 0);
              // exit the child
              exit(0);
            }
            else
            {
              cout << "[ERROR] Credentials invalid!\n";
              send(*sockfd, "[ERROR] Credentials invalid!",
                  sizeof("[ERROR] Credentials invalid!"), 0);
              // exit the child
              exit(1);
            }
          }
          else if (child > 1)
          { // parent wait for child
            wait(&status);

            if (status == 0)
            {
              // open read file
              ifstream clients_read("clients.json");
              // read data from file
              json clients_json;
              clients_read >> clients_json;

              admin_id = clients_json[username]["id"];
              clients_read.close();
              login = true;
            }
          }
          break;
        }
        case 3:
        {
          admin_id = 0;
          send(*sockfd, "Te-ai deconectat de la server!",
              sizeof("Te-ai deconectat de la server!"), 0);
          sleep(1);
          close(*sockfd);
          break;
        }
      }//-----------------------Switch not logged in -------------------------//
    }
    else
    {//-------------------------Logged in ------------------------------------//
      // select if ops for admin or client
      bzero(recv_msg, sizeof(recv_msg));
      recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
      printf("[DEBUG] Message received: %s\n", recv_msg);
      error = send(*sockfd, "ok\0", sizeof("ok\0"), 0);
      cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;

      // convert to int
      option = atoi(recv_msg);
      bzero(recv_msg, sizeof(recv_msg));

      switch (option)
      {
        case 1:
        { // For admin
          // call the common menu for the admin with admin id
          server_admin_menu(sockfd, admin_id);
          break;
        }
        case 2:
        { // For client
          char username[100];
          bzero(recv_msg, sizeof(recv_msg));

          // recv username
          recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
          // store the username
          for (int i = 0; i < recv_msg_len; i++)
          {
            username[i] = recv_msg[i];
          }
          printf("[DEBUG] Username :%s \n", username);
          // reset buffer
          bzero(recv_msg, sizeof(recv_msg));
          // open read file
          ifstream clients_read("clients.json");
          // read data from file
          json clients_json;
          clients_read >> clients_json;

          client_id = clients_json[username]["id"];
          clients_read.close();
          // call the common menu for the admin with client id
          server_admin_menu(sockfd, client_id);
          break;
        }
        case 3:
        { // Log out
          admin_id = 0;
          client_id = 0;
          login = false;
          break;
        }
        case 4:
        { // Exit
          admin_id = 0;
          send(*sockfd, "Te-ai deconectat de la server!",
              sizeof("Te-ai deconectat de la server!"), 0);
          sleep(1);
          close(*sockfd);
          break;
        }
      }
    }//-------------------------Logged in ------------------------------------//
  }
  pthread_mutex_unlock(&admin_mutex);
}
//----------------------------------------------------------------------------//
void *client_handler(void *args)
{
  int recv_msg_len, option, error;
  int *sockfd = (int *)args;
  char recv_msg[MAXLINE];
  bool login = false;
  int client_id = 0;

  // pthread_mutex_lock(&mutex);
  cout << "[INFO] Client connected!" << endl;
  char send_msg[MAXLINE] = "Te-ai conectat la server!";
  // pthread_mutex_unlock(&mutex);
  // bzero(mssg, MAXLINE);
  // send welcome message
  error = send(*sockfd, send_msg, MAXLINE, 0);
  cout << "[DEBUG] Send: " << error << endl;
  cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;

  while (1)
  { // recv option not connected
    bzero(&recv_msg, sizeof(recv_msg));
    option = receive_option_from_client(*sockfd);

    if (login == false)
    {
      switch (option)
      {
      case 1:
      {

        char username[100];
        char passwd[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv username
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          username[i] = recv_msg[i];
        }
        printf("[DEBUG] Username :%s \n", username);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the passwd
        for (int i = 0; i < recv_msg_len; i++)
        {
          passwd[i] = recv_msg[i];
        }
        printf("[DEBUG] Paswd:%s \n", passwd);

        // create child to execute function
        pid_t child = fork();
        if (child == 0)
        { // child
          string msg = create_user(username, passwd);
          char msg_char[100];
          strcpy(msg_char, msg.c_str());
          if (strncmp(msg_char, "ok\0", sizeof("ok\0")) == 0)
          {
            cout << "[INFO] New user created! Username: " << username << endl;
          }
          else
          {
            cout << msg;
          }
          // e bun
          send(*sockfd, msg_char, sizeof(msg_char), 0);
          exit(0);
        }
        else if (child > 1)
        { // parent wait for child
          wait(NULL);
        }
        break;
      }
      case 2:
      {
        bzero(recv_msg, sizeof(recv_msg));
        char username[100];
        char passwd[100];
        //bzero(recv_msg, sizeof(recv_msg));

        // recv username
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          username[i] = recv_msg[i];
        }
        printf("[DEBUG] Username :%s \n", username);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the passwd
        for (int i = 0; i < recv_msg_len; i++)
        {
          passwd[i] = recv_msg[i];
        }
        printf("[DEBUG] Paswd:%s \n", passwd);

        int status;
        // create child to execute function
        pid_t child = fork();
        if (child == 0)
        { // child
          if (login_user(username, passwd))
          {
            cout << "[INFO] Logged in!\n";
            // e bun, se sincronizeaza
            send(*sockfd, "ok\0", sizeof("ok\0"), 0);
            // exit the child
            exit(0);
          }
          else
          {
            cout << "[ERROR] Credentials invalid!\n";
            send(*sockfd, "[ERROR] Credentials invalid!",
                 sizeof("[ERROR] Credentials invalid!"), 0);
            // exit the child
            exit(1);
          }
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);

          if (status == 0)
          {
            // open read file
            ifstream clients_read("clients.json");
            // read data from file
            json clients_json;
            clients_read >> clients_json;

            client_id = clients_json[username]["id"];
            clients_read.close();
            login = true;
          }
        }
        break;
      }
      case 3:
      {
        client_id = 0;
        send(*sockfd, "Te-ai deconectat de la server!",
             sizeof("Te-ai deconectat de la server!"), 0);
        close(*sockfd);
        // break;
        pthread_exit(0);
      }
      default:
      {
        exit(1);
      }
      }
    }
    else
    {
      // aici ar trebui sa se intample magie cu transmitere de fisiere bla bla
      switch (option)
      {
      case 1:
      { // add data
        char key[100];
        char value[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv username
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          key[i] = recv_msg[i];
        }
        printf("[DEBUG] Key: %s \n", key);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the passwd
        for (int i = 0; i < recv_msg_len; i++)
        {
          value[i] = recv_msg[i];
        }
        printf("[DEBUG] Value:%s \n", value);

        // create child to execute function
        int status;
        pid_t child = fork();
        if (child == 0)
        { // child
          if (add_user_data(key, value, client_id))
          {
            cout << "[INFO] Added data " << key << " : " << value << "!\n";
            send(*sockfd, "ok\0", sizeof("ok\0"), 0);
            // exit the child
            exit(0);
          }
          else
          {
            cout << "[ERROR] Can't add data!\n";
            send(*sockfd, "[ERROR] Can't add data!\n",
                 sizeof("[ERROR] Can't add data!\n"), 0);
            // exit the child
            exit(1);
          }
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);
          if (status == 0)
          {
            cout << "[INFO] Data written for client " << client_id << endl;
          }
        }
        break;
      }
      case 2:
      { // read all data
        int status;
        pid_t child = fork();
        if (child == 0)
        { // child
          string date;
          date = getAllData(client_id);
          // cout << date << endl;
          int n = date.length();
          char dataToSend[n + 1];
          strcpy(dataToSend, date.c_str());
          int s = write(*sockfd, dataToSend, n + 1);
          // send(*sockfd, dataToSend, sizeof(dataToSend), 0);
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);
          if (status == 0)
          {
            cout << "[INFO] Data written for client " << client_id << endl;
          }
        }
        break;
      }
      case 3:
      { // read specific data
        char key[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv key
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the key
        for (int i = 0; i < recv_msg_len; i++)
        {
          key[i] = recv_msg[i];
        }
        printf("[DEBUG] Key: %s \n", key);
        // create child to execute function
        int status;
        pid_t child = fork();
        if (child == 0)
        { // child
          string data = get_user_data(key, client_id);
          cout << data << endl;
          send(*sockfd, data.c_str(), sizeof(data.c_str()), 0);
          // exit the child
          exit(0);
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);
          if (status == 0)
          {
            cout << "[INFO] Search done" << endl;
          }
        }
        break;
      }
      case 4:
      { // update data
        char key[100];
        char value[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv key
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          key[i] = recv_msg[i];
        }
        printf("[DEBUG] Key: %s \n", key);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));
  
        // recv value
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the passwd
        for (int i = 0; i < recv_msg_len; i++)
        {
          value[i] = recv_msg[i];
        }
        printf("[DEBUG] Value:%s \n", value);

        // create child to execute function
        int status;
        pid_t child = fork();
        if (child == 0)
        { // child
          string message = updateData(client_id, key, value); 
          if (strstr(message.c_str(), "[INFO] Pair deleted!\n") == 0)
          {
            cout << "[INFO] Updated data " << key << " : " << value << "!\n";
            // char *to_send_char;
            // strcpy(to_send_char, to_send.c_str());
            write(*sockfd, "[INFO] Updated data!\n", 
                  sizeof("[INFO] Updated data!\n"));
            // exit the child
            exit(0);
          }
          else
          {
            cout << "[ERROR] Can't update data!\n";
            send(*sockfd, "[ERROR] Can't update data!\n",
                 sizeof("[ERROR] Can't update data!\n"), 0);
            // exit the child
            exit(1);
          }
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);
          if (status == 0)
          {
            cout << "[INFO] Data written for client " << client_id << endl;
          }
        }
        break;
      }
      case 5:
      { // delete data
        deleteAllData(client_id);
        break;
      }
      case 6:
      { // delete specific pair
        char key[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv key
        recv_msg_len = recv(*sockfd, &recv_msg, MAXLINE, 0);
        // store the key
        for (int i = 0; i < recv_msg_len; i++)
        {
          key[i] = recv_msg[i];
        }
        printf("[DEBUG] Delete key: %s \n", key);
        // create child to execute function
        int status;
        pid_t child = fork();
        if (child == 0)
        { // child
          string data = delete_specific_data(key, client_id);
          cout << data << endl;
          write(*sockfd, data.c_str(), sizeof(data.c_str()));
          // exit the child
          exit(0);
        }
        else if (child > 1)
        { // parent wait for child
          wait(&status);
          if (status == 0)
          {
            cout << "[INFO] Search done" << endl;
          }
        }
        break;
      }
      case 7:
      { // log out
        client_id = 0;
        login = false;
        break;
      }
      case 8:
      { // exit
        client_id = 0;
        break;
      }
      default:
      {
        exit(1);
      }
      }
    }
  }
}
//============================================================================//
int main()
{
  struct sockaddr_in cli_addr, serv_addr;
  int msg_len, new_sockfd, connections;
  struct hostent *he;
  char msg[MAXLINE];
  pthread_t thds[NRTHREADS];
  pthread_t unix_thread, inet_thread;
  char *NumeServer = (char *)"pcd proiect"; // numele serverului luat din argv[0]
  char NumeHostServer[MAXHOSTNAME];

  signal(SIGINT, signal_sigint);

  gethostname(NumeHostServer, MAXHOSTNAME);
  printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);

  he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1

  bcopy(he->h_addr, &(serv_addr.sin_addr), he->h_length);
  printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
         inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
  /* numele procesului server luat de pe linia de comanda */

  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    fprintf(stderr, "EROARE server: nu pot sa deschid stream socket \n");

  int check = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(int)) < 0)
    fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(SERV_TCP_PORT);

  if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    fprintf(stderr, "EROARE server: nu pot sa asignez un nume adresei locale \n");

  printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer, ntohs(serv_addr.sin_port));

  if (listen(sock_fd, 100) < 0)
  {
    fprintf(stderr, "Eroare!");
    exit(0);
  }

  while (1)
  {
    bzero((char *)&cli_addr, sizeof(cli_addr));
    socklen_t cli_addr_size = sizeof(cli_addr);
    new_sockfd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_size);

    if (new_sockfd < 0)
    {
      fprintf(stderr, "[EROARE SERVER]: accept()\n");
      exit(1);
    }
    msg_len = recv(new_sockfd, &msg, MAXLINE, 0);
    printf("[INFO] Client Type: %s\n", msg);
    msg[msg_len] = '\0';

    if (strncmp(msg, "admin\0", sizeof("admin\0")) == 0)
    { // start thread cu functie admin
      if (admin_connection == false)

      { // daca nu mai e conectat unul deja
        pthread_create(&thds[connections], NULL, admin_handler,
                       (void *)&new_sockfd);
        connections++;
      }
      else
      { // daca e conectat, send error to client
        char temp_msg[100] = "[ERROR] Un admin e deja conectat.\n";
        send(new_sockfd, temp_msg, sizeof(temp_msg), 0);
        close(new_sockfd);
      }
    }
    else if (strncmp(msg, "client\0", sizeof("client\0")) == 0)
    {
      // cout << "aici\n";
      pthread_create(&thds[connections++], NULL, client_handler,
                     (void *)&new_sockfd);
      // cout << "aici2\n";
    }
    else
    {
      printf("[ERROR] Unkown connection. Closing...\n");
      close(new_sockfd);
    }
  }
  close(sock_fd);
  close(new_sockfd);
}
//============================================================================//