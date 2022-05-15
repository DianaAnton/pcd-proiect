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

#define SERV_TCP_PORT 7979
#define MAXLINE 1024
#define MAXHOSTNAME 100
#define BOOL int
#define TRUE 1
#define FALSE 0

using namespace std;
int main() {
    
    struct sockaddr_in cli_addr, serv_addr;
    int rc, sockfd, clilen, newsockfd, childpid;
    struct hostent *he;
    char msg[MAXLINE];
    char *NumeServer = "pcd proiect"; // numele serverului luat din argv[0]
    char NumeHostServer[MAXHOSTNAME];

    gethostname(NumeHostServer, MAXHOSTNAME);
    printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);

    he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1

    bcopy ( he->h_addr, &(serv_addr.sin_addr), he->h_length);
    printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
    inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
    /* numele procesului server luat de pe linia de comanda */

    int check = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(int)) < 0)
    //     perror("setsockopt(SO_REUSEADDR) failed");
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("EROARE server: nu pot sa deschid stream socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_TCP_PORT);    


    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        perror("EROARE server: nu pot sa asignez un nume adresei locale");

    printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer, ntohs(serv_addr.sin_port));

    listen(sockfd, 50);

    for (;;) {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      clilen = sizeof(cli_addr);
      socklen_t cli_addr_size = sizeof(cli_addr);
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_addr_size);
      //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)clilen);
      if (newsockfd < 0) {
        perror("EROARE server: accept() esuat");
        exit(-1);
      }

      rc = recv(newsockfd, &msg, MAXLINE,0);
      printf("<<<<<<<<<<<< _%s_ >>>>>>>>>>>", msg);
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