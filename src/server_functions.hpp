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
void deleteAllData(int clientId)
{
  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
  string id = to_string(clientId);
  if (date.contains(to_string(clientId)))
  {
    date.erase(to_string(clientId));
  }

  std::ofstream data_write("data.json");

  data_write << date.dump(4) << std::endl;
  // close write file
  data_write.close();
}
//----------------------------------------------------------------------------//
void updateData(int clientId, char *key)
{

  ifstream dataFile("data.json");
  json date;
  dataFile >> date;
  dataFile.close();
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
//============================================================================//