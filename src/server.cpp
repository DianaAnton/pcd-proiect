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

using namespace std;

// for convenience
using json = nlohmann::json;

#define SERV_TCP_PORT 7979
#define MAXLINE 50000
#define MAXHOSTNAME 100
#define NRTHREADS 100

pthread_mutex_t mutex;
bool admin_connection = false;
int connections = 0;
using namespace std;
int clientId = 1;

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
void *admin_handler(void *args)
{
  int recv_msg_len;
  char msg[MAXLINE];
  int *sock_fd = (int *)args;
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
int create_client_id()
{
  /* Create some unique ID. e.g. UNIX timestamp... */
  char ctsmp[12];
  time_t rawtime;
  struct tm *timeinfo;
  int uuid;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(ctsmp, 12, "%s", timeinfo);

  uuid = atoi(ctsmp);
  /* NOTICE: YOU NEED TO STORE THIS INFORMATION AT SERVER LEVEL !*/
  return uuid;
}
//----------------------------------------------------------------------------//
string get_user_data(char *key, int client_id)
{
  printf("[DEBUG] Get user data received: %s\n", key);
  // open read file
  ifstream data_read("data.json");
  // read data from file
  json data_json;
  data_read >> data_json;
  // close read file
  data_read.close();
  string key_str = key;
  cout << "key_str " << key_str << endl;

  json data_array = data_json[to_string(client_id)];
  // cout << data_array.dump(4) << endl;
  bool found = false;
  string value;

  // for (json::iterator it = data_array.begin(); it != data_array.end(); ++it)
  for (auto &el : data_array.items())
  {
    json el_json = el.value();
    cout << "el.value[key] " << el_json[key_str] << endl;
    if (el_json[key_str] != nullptr)
    {
      value = el_json[key_str];
      found = true;
      cout << "[DEBUG] FOUND key: " << key_str << ": " << value << endl;
      break;
    }
  }

  if (found == true)
  {
    return value;
  }
  else
  {
    string msg = "[ERROR] No data found with key " + key_str + "\n";
    return msg;
  }
}
//----------------------------------------------------------------------------//
bool add_user_data(char *key, char *value, int client_id)
{
  printf("[DEBUG] Add data received: %s %s\n", key, value);
  // open read file
  ifstream data_read("data.json");
  // read data from file
  json data_json;
  data_read >> data_json;
  // close read file
  data_read.close();

  // declare details
  json details;
  details[key] = value;
  // add details to object
  data_json[to_string(client_id)] += details;
  cout << data_json.dump(4) << endl;

  // open write file
  std::ofstream data_write("data.json");
  // data_write << std::setw(4) << data_json << std::endl;
  data_write << data_json << std::endl;
  // close write file
  data_write.close();
}
//----------------------------------------------------------------------------//
bool login_user(char *username, char *passwd)
{
  ifstream client_file("clients.json");
  json client_json;
  client_file >> client_json;
  client_file.close();

  if (client_json.contains(username))
  { // the user exists
    string passwd_temp = passwd;
    string stored_pass = client_json[username]["passwd"];
    // compare passwords
    if (stored_pass.compare(passwd) == 0)
    {
      return true;
    }
    return false;
  }
  return false;
}
//----------------------------------------------------------------------------//

string getAllData(int clientId)
{
  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
  char file[1000];
  json temp;
  string date_;

  if (date.contains(to_string(clientId)))
  {
    for (json::iterator it = date.begin(); it != date.end(); ++it)
    {
      // std::cout << it.key() << " : " << it.value() << "\n";
      if (it.key() == to_string(clientId))
      {

        temp = it.value();
        // date_ = temp.dump();
        //  temp.dump(4);
      }
    }
  }

  
  return temp.dump();
}
//----------------------------------------------------------------------------//

// void login_user_(char *username, char *passwd)
// {
//   ifstream client_file("clients.json");
//   json client_json;
//   client_file >> client_json;

//   for (json::iterator it = client_json.begin(); it != client_json.end(); ++it)
//   {
//     std::cout << it.key() << " : " << it.value() << "\n";

//   }
// }
//----------------------------------------------------------------------------//
void create_user(char *username, char *passwd)
{
  // create client id
  int id = create_client_id();
  // open read file
  std::ifstream client_read("clients.json");
  printf("[DEBUG] Create User received: %s %s\n", username, passwd);
  // read data from file
  json clients_json;
  client_read >> clients_json;
  // close read file
  client_read.close();

  // declare details
  json details;
  details["id"] = id;
  details["passwd"] = passwd;
  // add details to object
  clients_json[username] = details;
  cout << clients_json.dump(4) << endl;

  // open write file
  std::ofstream client_write("clients.json");
  // client_write << std::setw(4) << clients_json << std::endl;
  client_write << clients_json << std::endl;
  // close write file
  client_write.close();
}
//----------------------------------------------------------------------------//
void *client_handler(void *args)
{
  int recv_msg_len, option, error;
  int *sock_fd = (int *)args;
  char recv_msg[MAXLINE];
  bool login = false;
  int client_id = 0;

  // pthread_mutex_lock(&mutex);
  cout << "[INFO] Client connected!" << endl;
  char send_msg[MAXLINE] = "Te-ai conectat la server!";
  // pthread_mutex_unlock(&mutex);
  // bzero(mssg, MAXLINE);
  error = send(*sock_fd, send_msg, MAXLINE, 0);
  cout << "[DEBUG] Send: " << error << endl;
  cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;

  while (1)
  { // recv option not connected
    recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
    printf("[DEBUG] Message received: %s\n", recv_msg);
    error = send(*sock_fd, "ok", sizeof("ok"), 0);
    // cout << "[DEBUG] Send: " << error << endl;
    cout << "[ERROR] ERRNO: " << errno << " " << strerror(errno) << endl;
    // printf("[DEBUG] Message sent: %s\n");

    // convert to int
    option = atoi(recv_msg);
    bzero(recv_msg, sizeof(recv_msg));

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
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          username[i] = recv_msg[i];
        }
        printf("[DEBUG] Username :%s \n", username);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
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
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          username[i] = recv_msg[i];
        }
        printf("[DEBUG] Username :%s \n", username);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
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
            send(*sock_fd, "ok", sizeof("ok"), 0);
            // exit the child
            exit(0);
          }
          else
          {
            cout << "[ERROR] Credentials invalid!\n";
            send(*sock_fd, "[ERROR] Credentials invalid!",
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
        send(*sock_fd, "Te-ai deconectat de la server!",
             sizeof("Te-ai deconectat de la server!"), 0);
        close(*sock_fd);
        break;
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
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
        // store the username
        for (int i = 0; i < recv_msg_len; i++)
        {
          key[i] = recv_msg[i];
        }
        printf("[DEBUG] Key: %s \n", key);
        // reset buffer
        bzero(recv_msg, sizeof(recv_msg));

        // recv passwd
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
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
            cout << "[INFO] Logged in!\n";
            send(*sock_fd, "ok", sizeof("ok"), 0);
            // exit the child
            exit(0);
          }
          else
          {
            cout << "[ERROR] Credentials invalid!\n";
            send(*sock_fd, "[ERROR] Credentials invalid!",
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
            cout << "[INFO] Data written" << endl;
          }
        }
        break;
      }
      case 2:
      { // read all data
        string date;
        date = getAllData(client_id);
        //cout << date << endl;
        int n = date.length();
        char dataToSend[n+1];
        strcpy(dataToSend, date.c_str());
        int s = write(*sock_fd, dataToSend, n+1);
       // send(*sock_fd, dataToSend, sizeof(dataToSend), 0);
        break;
      }
      case 3:
      { // read specific data
        char key[100];
        bzero(recv_msg, sizeof(recv_msg));

        // recv key
        recv_msg_len = recv(*sock_fd, &recv_msg, MAXLINE, 0);
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
          send(*sock_fd, data.c_str(), sizeof(data.c_str()), 0);
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

        break;
      }
      case 5:
      { // delete data

        break;
      }
      case 6:
      { // exit
        client_id = 0;
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
  char *NumeServer = (char *)"pcd proiect"; // numele serverului luat din argv[0]
  char NumeHostServer[MAXHOSTNAME];

  // pthread_mutex_init(&mutex, NULL);

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
    new_sock_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_addr_size);

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
                       (void *)&new_sock_fd);
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
      pthread_create(&thds[connections++], NULL, client_handler,
                     (void *)&new_sock_fd);
      cout << "aici2\n";
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