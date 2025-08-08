#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

class Node {
private:
  std::unordered_map<std::string, std::string> mappings;

  void handleClient(int clientSocket) {
    std::string greeting =
      std::string(YELLOW) +
      "Welcome to MiniKV - a key-value store server!\n"
      "Commands:\n"
      "  PUT <key> <value>   - Store a value\n"
      "  GET <key>           - Retrieve a value\n"
      "  DELETE <key>        - Remove a key-value pair\n"
      "  QUIT                - Quit the program (as client)\n" +
      std::string(RESET);
    write(clientSocket, greeting.c_str(), greeting.size());
    char buffer[1024];
    while (true) {
      const char* prompt = BLUE "> " RESET;
      write(clientSocket, prompt, strlen(prompt));
      memset(buffer, 0, sizeof(buffer));
      ssize_t bytesReceived = read(clientSocket, buffer, sizeof(buffer));
      if (bytesReceived == 0) {
        std::cout << YELLOW << "Client disconnected" << RESET << std::endl;
        break;
      }
      if (bytesReceived < 0) {
        std::cerr << RED << "ERROR: Read failed" << RESET << std::endl;
        break;
      }
      std::vector<std::string> tokens = split(buffer);
      if (tokens.empty()) {
        continue;
      }
      std::string response;
      if (tokens[0] == "QUIT" && tokens.size() == 1) {
        std::string goodbye = std::string(YELLOW) + "Goodbye!\n" + RESET;
        write(clientSocket, goodbye.c_str(), goodbye.size());
        std::cout << YELLOW << "Client has requested to quit" << RESET << std::endl;
        break;
      } else if (tokens[0] == "PUT" && tokens.size() == 3) {
        mappings[tokens[1]] = tokens[2];
        response = std::string(GREEN) + "OK\n" + RESET;
      } else if (tokens[0] == "GET" && tokens.size() == 2) {
        auto it = mappings.find(tokens[1]);
        if (it != mappings.end()) {
          response = std::string(GREEN) + it->second + "\n" + RESET;
        } else {
          response = std::string(RED) + "NOT FOUND\n" + RESET;
        }
      } else if (tokens[0] == "DELETE" && tokens.size() == 2) {
        auto it = mappings.find(tokens[1]);
        if (it != mappings.end()) {
          mappings.erase(it);
          response = std::string(GREEN) + "DELETED\n" + RESET;
        } else {
          response = std::string(RED) + "NOT FOUND\n" + RESET;
        }
      } else {
        response = std::string(RED) + "ERROR: Invalid command/ incorrect number of arguments\n" + RESET;
      }
      write(clientSocket, response.c_str(), response.size());
    }
    close(clientSocket);
  }

  std::vector<std::string> split(const std::string &input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
      tokens.push_back(token);
    }
    return tokens;
  }

public:
  void start(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      std::cerr << RED << "ERROR: Socket creation failed" << RESET << std::endl;
      return;
    }
    const int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) < 0) {
      std::cerr << RED << "ERROR: setsockopt" << RESET << std::endl;
      close(server_fd);
      return;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      std::cerr << RED << "ERROR: Bind failed" << RESET << std::endl;
      close(server_fd);
      return;
    }
    if (listen(server_fd, 3) < 0) {
      std::cerr << RED << "ERROR: Listen failed" << RESET << std::endl;
      close(server_fd);
      return;
    }
    std::cout << "Server listening on port " << port << "..." << std::endl;
    while (true) {
      socklen_t addrlen = sizeof(address);
      int clientSocket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
      if (clientSocket < 0) {
        std::cerr << RED << "ERROR: Accept failed" << RESET << std::endl;
        continue;
      }
      std::cout << GREEN << "Accepted new client" << RESET << std::endl;
      pid_t pid = fork();
      if (pid == 0) {
        close(server_fd);
        handleClient(clientSocket);
        exit(0);
      } else if (pid > 0) {
        close(clientSocket);
      } else {
        std::cerr << RED << "ERROR: Fork failed" << RESET << std::endl;
      }
    }
    close(server_fd);
  }
};

int main (int argc, char *argv[]) {
  Node node;
  node.start(8080);
  return 0;
}