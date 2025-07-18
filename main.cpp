#include <cstring>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

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
  void start(int port);
};
