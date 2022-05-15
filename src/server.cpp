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


#define SERV_TCP_PORT 7979
#define MAXLINE 1024
#define MAXHOSTNAME 100
#define NRTHREADS 100

pthread_mutex_t mutex;
bool connection = false;


/**
 *  COMPLETAT FUNCTII DE PARSARE FISIERE JSON, apoi stocate local
 *  IN FUNCTIE DE OPTIUNEA SELECTATA, SE VOR STERGE, CREEA, MODIFICA 
 *  fisiere JSON
 *  STABILIRE CLARA A FUNCTIILOR DE CATRE SERVER  
 */


void* serverHandler(int sockfd) {

    int msg_recv;
    pthread_mutex_lock(&mutex);
    connection = true;
    pthread_mutex_unlock(&mutex);

    char msg[MAXLINE];

    send(sockfd, "cv", 1, 0);

    while (msg_recv = recv(sockfd, &msg, MAXLINE, 0)) {
      // aici ar trebui sa se intample magie cu transmitere de fisiere bla bla
    }

    pthread_mutex_lock(&mutex);
    connection = false;
    pthread_mutex_unlock(&mutex);
} 

using namespace std;
int main() {
    
    struct sockaddr_in cli_addr, serv_addr;
    int rc, sockfd, clilen, newsockfd, childpid;
    struct hostent *he;
    char msg[MAXLINE];
    pthread_t thread[NRTHREADS];
    char *NumeServer = "pcd proiect"; // numele serverului luat din argv[0]
    char NumeHostServer[MAXHOSTNAME];

    //pthread_mutex_init(&mutex, NULL);

    gethostname(NumeHostServer, MAXHOSTNAME);
    printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);

    he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1

    bcopy ( he->h_addr, &(serv_addr.sin_addr), he->h_length);
    printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
    inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
    /* numele procesului server luat de pe linia de comanda */

    int check = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(int)) < 0)
    //     fprintf("setsockopt(SO_REUSEADDR) failed");
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        fprintf(stderr,"EROARE server: nu pot sa deschid stream socket \n");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_TCP_PORT);    


    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        fprintf(stderr, "EROARE server: nu pot sa asignez un nume adresei locale \n");

    printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer, ntohs(serv_addr.sin_port));

    if (listen(sockfd, 100) < 0) {
      fprintf(stderr, "Eroare!");
      exit(0);
    }

    while(1) {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      clilen = sizeof(cli_addr);
      socklen_t cli_addr_size = sizeof(cli_addr);
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_addr_size);
      if (newsockfd < 0) {
        fprintf(stderr, "EROARE server: accept() esuat \n");
        exit(-1);
      }

      while (rc = recv(newsockfd, &msg, MAXLINE,0)){
      printf("De la client: %s \n", msg);
      msg[MAXLINE] = '\0';
      send(newsockfd, msg, strlen(msg), 0);
      if (strstr(msg,"adio")) // adio este in line ???
      { /* la introducere "adio" iesire din ciclu */
      close(newsockfd); /* proces copil / close socket nou*/
      printf("\n---TCPServer ___client PID= %ld deconectat\n", (long)getpid());
      close(newsockfd);
      exit(0); /* copilul ramane "defunct" si nu dispare daca parintele nu zice wait(NULL)*/
      }
      else
      {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      };
      }
    }
}