#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "usage: client <host> <port>\n";
    return 1;
  }

  // creating socket
  const char* HOST_NAME = argv[1];
  const char* PORT = argv[2];

  addrinfo* ai = nullptr;
  addrinfo* p = nullptr;
  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int result = getaddrinfo(HOST_NAME, PORT, &hints, &ai);
  if (result != 0) {
    const auto* err = gai_strerror(result);
    std::cerr << "client: %s\n" << err << "\n";
  }

  int clientSocket = -1;
  for (p = ai; p != nullptr; p = p->ai_next) {
    clientSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (clientSocket < 0) {
      continue;
    }
    if (connect(clientSocket, p->ai_addr, p->ai_addrlen) < 0) {
      close(clientSocket);
      continue;
    }
    break;
  }

  if (p == nullptr) {
    return -1;
  }
  freeaddrinfo(ai);

  // sending data
  const char* message = "Hello from client!";
  send(clientSocket, message, strlen(message), 0);

  // closing socket
  close(clientSocket);

  return 0;
}
