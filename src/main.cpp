// #include <iostream>

// int main() {
//     std::cout << "Hello Easy C++ project!" << std::endl;
//      std::cout << "Hello Easy C++ project!" << std::endl;
// }
#include <iostream>

#include "http_server.h"
#include "served/multiplexer.hpp"

int main() {
  served::multiplexer multiplexer;
  learning::HttpServer http_server(multiplexer);

  http_server.InitializeEndpoints();
  http_server.StartServer();

  return (EXIT_SUCCESS);
}