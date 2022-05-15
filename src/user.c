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
//#include <strings.h> /* e mai nou decat string.h, dar nu merge */
/* Definitii pentru programul server TCP */
/* 1. Porturtul TCP utilizat. */
#define SERV_TCP_PORT 5000
/* 2. Alte constante */
#define MAXLINE 512 /* nr. max. octeti de citit cu recv() */
#define MAXHOSTNAME 100 /* nr. max. octeti nume host */
int main(int argc, char *argv[])
{
int rc, sockfd, clilen, newsockfd, childpid;
char line [512];
struct sockaddr_in cli_addr, serv_addr;
/** Structura sockaddr_in este definita in <netinet/in.h> astfel:
struct sockaddr_in {
sa_family_t sin_family; // (short) - AF_xxx, indica familia de protocoale * ex. AF_INET pt. IP4
in_port_t sin_port; // (unsigned short) - port in ordinea octetilor retelei * ex. htons(3490)
struct in_addr sin_addr; // adresa Internet * vezi struct in_addr, de mai jos
// ex. 127.0.0.1
char sin_zero[8]; // neutilizat * nimic daca vrei
};
struct in_addr {
uint32 s_addr; // (unsigned long) - adresa Internet in ordinea octetilor retea
// * incarcare cu inet_aton() *
};
**/

struct hostent *he; // gethostbyname()
/** Structura hostent in este definita in <netdb.h> astfel:
struct hostent {
char *h_name; // official name of host
char **h_aliases; // alias list
int h_addrtype; // host address type (AF_INET or AF_INET6)
int h_length; // length of address
char **h_addr_list; // list of addresses
}
este facut #define h_addr h_addr_list[0] pentru compabilitate in jos
**/
char *NumeServer; // numele serverului luat din argv[0]
char NumeHostServer[MAXHOSTNAME];
/*** Preiau informatii despre Server, NumeHostServer si INET ADDRESS(IP) ***/
/*** (este doar pentru a afla IP server, utilizat de catre clientul TCP la conectare) ***/
gethostname(NumeHostServer, MAXHOSTNAME); // aflam numele serverului
printf("\n----TCPServer startat pe hostul: %s\n", NumeHostServer);
he = gethostbyname(NumeHostServer); // aflam adresa de IP server/ probabil 127.0.0.1
bcopy ( he->h_addr, &(serv_addr.sin_addr), he->h_length);
printf(" \t(TCPServer INET ADDRESS (IP) este: %s )\n",
inet_ntoa(serv_addr.sin_addr)); // conversie adresa binarea in ASCII (ex. "127.0.0.1")
/* numele procesului server luat de pe linia de comanda */
NumeServer = argv[0];
/*
** Open pentru un socket TCP
(AF_INET: este din familia de protocoale Internet)
(SOCK_STREAM: este de tipul stream socket)
socket - crează un soclu fără nume cu caracteristicile cerute de tipul de comunicare dorit şi returnează
un descripror de soclu
*/

if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
perror("EROARE server: nu pot sa deschid stream socket");
/*
** Asignarea unei adrese locale si port protocol spre care clientul poate trimite date.
*/
bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY binds the socket to all available interfaces.
serv_addr.sin_port = htons(SERV_TCP_PORT); // SERV_TCP_PORT = 5000
/**
bind - realizează legătura între un descriptor de soclu anterior creat şi o adresă locală finală de
comunicare. Adresa locală finală este specificată atât prin adresa de IP cît şi prin numărul de port.
**/
if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
perror("EROARE server: nu pot sa asignez un nume adresei locale");


printf("---TCPServer %s: ++++ astept conexiune clienti pe PORT: %d++++\n\n", NumeServer,
ntohs(serv_addr.sin_port));
/*
** Server gata de conexiune pentru orice client
listen - plasează soclul serverului intr-un mod pasiv şi îl face pregătit pentru a accepta conexiunile care îi
vin de la client
*/
listen(sockfd, 5); // max. 5 conexiuni
for ( ; ; ) /* server cu conexiune – Server TCP*/
{
/*
* Bucla infinita.
* Asteptarea conexiunea cu un client.
*/
bzero((char *)&cli_addr, sizeof(cli_addr));
clilen = sizeof(cli_addr);
/*
accept - o conexiune actuală a procesului client este aşteptată
*/
newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)clilen); // Asteptare conexiune din partea unui client
//newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)sizeof(cli_addr));
if (newsockfd < 0)
{
perror("EROARE server: accept() esuat");
exit(-1);
}
/* creare proces copil pentru coxiunea acceptata */
if ( (childpid = fork()) < 0) {perror("EROARE server: fork() esuat"); exit(1);}
else
{
if (childpid == 0)
{ /* proces copil */
close(sockfd); /* close socket original */
printf("\n---TCPServer%s ___client PID= %ld conectat\n", NumeServer, (long)getpid());
/* procesare cerere:
serverul citeste date de la client pe care afiseaza
dupa care serverul trimite datele in ecou catre client */
while
(rc = recv(newsockfd, &line, MAXLINE,0)) // Singura diferenta dintre recv() si read()
// este prezenta unui indicator, care daca e pus pe 0 cele

// doua functii sunt echivalente
// read(newsockfd, &line, MAXLINE);

{
line[rc] = '\0'; // set EOB - end buffer
printf("---TCPServer. Receptionat de la client [adio=term]: %s", line); /* afiseaza linie receptionata */
// trimit in ecou mesajul
send(newsockfd, line, strlen(line), 0); // merge si cu write
//if (strcmp(line, "adio\n") == 0 )
if (strstr(line,"adio")) // adio este in line ???

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
}; // end while- se reia ciclul infinit de recv
} // end tratare copil
{
// tratare parinte
wait(NULL); /* NU asteapta codul de retur al copilului
- “defunctii” nu dispar decat asa */
} // end tratare parinte
} // end tratare parinte/copil
} // end for - se reia ciclul infinit
} // end main