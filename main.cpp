#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>

class Node {
private:
  std::unordered_map<std::string, std::string> mappings;

  void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
      memset(buffer, 0, sizeof(buffer));
      ssize_t bytesReceived = read(clientSocket, buffer, sizeof(buffer));
      if (bytesReceived <= 0) {
        perror("ERROR: Read failed\n");
        break;
      }
      std::vector<std::string> tokens = split(buffer);
      if (tokens.empty()) {
        continue;
      }
      std::string response;
      if (tokens[0] == "PUT" && tokens.size() == 3) {
        mappings[tokens[1]] = tokens[2];
        response = "OK\n";
      } else if (tokens[0] == "GET" && tokens.size() == 2) {
        auto it = mappings.find(tokens[1]);
        if (it != mappings.end()) {
          response = it->second + "\n";
        } else {
          response = "NOT FOUND\n";
        }
      } else if (tokens[0] == "DELETE" && tokens.size() == 2) {
        auto it = mappings.find(tokens[1]);
        if (it != mappings.end()) {
          mappings.erase(it);
          response = "DELETED\n";
        } else {
          response = "NOT FOUND\n";
        }
      } else {
        response = "ERROR: Invalid command/ incorrect number of arguments\n";
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
      perror("ERROR: Socket creation failed");
      return;
    }
    const int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) < 0) {
      perror("setsockopt");
      close(server_fd);
      return;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("ERROR: Bind failed");
      close(server_fd);
      return;
    }
    if (listen(server_fd, 3) < 0) {
      perror("ERROR: Listen failed");
      close(server_fd);
      return;
    }
    std::cout << "Server listening on port " << port << " ..." << std::endl;
    while (true) {
      socklen_t addrlen = sizeof(address);
      int clientSocket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
      if (clientSocket < 0) {
        perror("ERROR: Accept failed");
      }
      std::cout << "Accepted new client" << std::endl;
      handleClient(clientSocket);
    }
    close(server_fd);
  }
};

int main (int argc, char *argv[]) {
  Node node;
  node.start(8080);
  return 0;
}
