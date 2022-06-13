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

pthread_mutex_t file_mutex;
bool admin_connection = false;
int connections = 0;
using namespace std;
int clientId = 1;

//============================================================================//
//                                                                            //
//                                 COMMON                                     //
//                                                                            //
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
bool verify_if_user_exists(string username)
{
  pthread_mutex_lock(&file_mutex);
  ifstream client_file("clients.json");
  json client_json;
  client_file >> client_json;
  client_file.close();
  pthread_mutex_unlock(&file_mutex);

  if (client_json.contains(username))
  {
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------//
string create_user(char *username, char *passwd)
{
  // check if the username is in use or not
  if (verify_if_user_exists(username))
  { // the username already exists
    return "[ERROR] The username already exists!\n";
  }
  // create client id
  int id = create_client_id();
  // open read file
  pthread_mutex_lock(&file_mutex);
  std::ifstream client_read("clients.json");
  printf("[DEBUG] Create User received: %s %s\n", username, passwd);
  // read data from file
  json clients_json;
  client_read >> clients_json;
  // close read file
  client_read.close();
  pthread_mutex_unlock(&file_mutex);

  // declare details
  json details;
  details["id"] = id;
  details["passwd"] = passwd;
  // add details to object
  clients_json[username] = details;
  cout << clients_json.dump(4) << endl;

  // open write file
  pthread_mutex_lock(&file_mutex);
  std::ofstream client_write("clients.json");
  // client_write << std::setw(4) << clients_json << std::endl;
  client_write << clients_json << std::endl;
  // close write file
  client_write.close();
  pthread_mutex_unlock(&file_mutex);
  return "ok";
}
//----------------------------------------------------------------------------//
bool login_user(char *username, char *passwd)
{
  pthread_mutex_lock(&file_mutex);
  ifstream client_file("clients.json");
  json client_json;
  client_file >> client_json;
  client_file.close();
  pthread_mutex_unlock(&file_mutex);

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
bool send_option_to_server(char option[10], int sockfd)
{
    char line[MAXLINE];
    int size;
    bzero(line, sizeof(line));
    send(sockfd, option, sizeof(&option), 0);
    size = recv(sockfd, &line, sizeof("ok"), 0);
    cout << "recv: " << size << endl;
    cout << "ERRNO: " << errno << " " << strerror(errno) << endl;
    if (strncmp(line, "ok\0", sizeof("ok\0")) == 0)
    {
        return true;
    }
    bzero(line, sizeof(line));
    return false;
}
//============================================================================//
//                                                                            //
//                                 CLIENT                                     //
//                                                                            //
//============================================================================//
//----------------------------------------------------------------------------//
bool add_user_data(char *key, char *value, int client_id)
{
  printf("[DEBUG] Add data received: %s %s\n", key, value);
  // open read file
  pthread_mutex_lock(&file_mutex);
  ifstream data_read("data.json");
  // read data from file
  json data_json;
  data_read >> data_json;
  // close read file
  data_read.close();
  pthread_mutex_unlock(&file_mutex);

  // declare details
  json details;
  details[key] = value;
  // add details to object
  data_json[to_string(client_id)].push_back(details);
  cout << data_json.dump(4) << endl;

  // open write file
  pthread_mutex_lock(&file_mutex);
  std::ofstream data_write("data.json");
  // data_write << std::setw(4) << data_json << std::endl;
  data_write << data_json << std::endl;
  // close write file
  data_write.close();
  pthread_mutex_unlock(&file_mutex);
}
//----------------------------------------------------------------------------//
string get_user_data(char *key, int client_id)
{
  printf("[DEBUG] Get user data received: %s\n", key);
  // open read file
  pthread_mutex_lock(&file_mutex);
  ifstream data_read("data.json");
  // read data from file
  json data_json;
  data_read >> data_json;
  // close read file
  data_read.close();
  pthread_mutex_unlock(&file_mutex);

  string key_str = key;
  cout << "key_str " << key_str << endl;

  json data_array = data_json[to_string(client_id)];
  bool found = false;
  string value;

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
string getAllData(int clientId)
{
  pthread_mutex_lock(&file_mutex);
  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
  pthread_mutex_unlock(&file_mutex);

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
void updateData(int clientId, char *key)
{
  pthread_mutex_lock(&file_mutex);
  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
  pthread_mutex_unlock(&file_mutex);

  string id = to_string(clientId);
  json temp;

  if (date.contains(to_string(clientId)))
  {
    for (json::iterator it = date.begin(); it != date.end(); ++it)
    {
      if (it.key() == to_string(clientId))
      {
        temp = it.value();
      }
    }
  }

  for (auto &el : temp.items())
  {
    cout << el << endl;
  }
}
//----------------------------------------------------------------------------//
void deleteAllData(int clientId)
{
  pthread_mutex_lock(&file_mutex);
  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
  pthread_mutex_unlock(&file_mutex);

  string id = to_string(clientId);
  if (date.contains(to_string(clientId)))
  {
    date.erase(to_string(clientId));
  }

  pthread_mutex_lock(&file_mutex);
  std::ofstream data_write("data.json");
  data_write << date.dump(4) << std::endl;
  // close write file
  data_write.close();
  pthread_mutex_unlock(&file_mutex);
}
//----------------------------------------------------------------------------//
string delete_specific_data(string key, int client_id)
{
  printf("[DEBUG] Delete pair with key: %s\n", key);
  // open read file
  pthread_mutex_lock(&file_mutex);
  ifstream data_read("data.json");
  // read data from file
  json data_json;
  data_read >> data_json;
  // close read file
  data_read.close();
  pthread_mutex_unlock(&file_mutex);

  string key_str = key;
  cout << "key_str " << key_str << endl;

  json data_array = data_json[to_string(client_id)];
  bool found = false;
  string value;

  for (auto &el : data_array.items())
  {
    json el_json = el.value();
    // cout << "el.value[key] " << el_json[key_str] << endl;
    if (el_json[key_str] != nullptr)
    {
      value = el_json[key_str];
      // el.value().erase(key_str);
      data_array.erase(el);
      found = true;
      cout << "[DEBUG] Delete key: " << key_str << ": " << value << endl;
      break;
    }
  }

  if (found == true)
  {
    data_json[to_string(client_id)] = data_array;
    cout << data_array.dump(4) << endl;
    // open write file
    pthread_mutex_lock(&file_mutex);
    std::ofstream data_write("data.json");
    // data_write << std::setw(4) << data_json << std::endl;
    data_write << data_json << std::endl;
    // close write file
    data_write.close();
    pthread_mutex_unlock(&file_mutex);
    return "[INFO] Pair deleted!\n";
  }
  else
  {
    string msg = "[ERROR] No data found with key " + key_str + "\n";
    return msg;
  }
}
//============================================================================//
//                                                                            //
//                                 ADMIN                                      //
//                                                                            //
//============================================================================//
void admin_menu(int sockfd, char *username)
{
    bool connected = false;
    int option;
    char line[MAXLINE];
    cout << "1. Adaugare date noi\n";
    cout << "2. Vizualizare date\n";
    cout << "3. Cauta o valoare specifica\n";
    cout << "4. Modificare date\n";
    cout << "5. Stergere TOATE datele\n";
    cout << "6. Stergere o pereche anume\n";
    cout << "7. Mergi inapoi\n";
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
              int n = read(sockfd, line, MAXLINE);
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
      { // Go Back
          if (send_option_to_server((char *)"7", sockfd))
          {
              return;
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
//============================================================================//