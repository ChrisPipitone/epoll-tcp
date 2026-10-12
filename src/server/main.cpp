#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

int get_listener_socket() {
  constexpr const char* PORT = "8080";
  addrinfo hints{};
  addrinfo* ai = nullptr;
  addrinfo* p = nullptr;
  int yes = 1;

  // Get us a socket and bind it
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int result = getaddrinfo(nullptr, PORT, &hints, &ai);
  if (result != 0) {
    const auto* err = gai_strerror(result);
    std::cerr << "server: %s\n" << err << "\n";
    throw std::runtime_error(err);
  }

  int listen_fd = -1;
  for (p = ai; p != nullptr; p = p->ai_next) {
    listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listen_fd < 0) {
      continue;
    }

    // Lose the pesky "address already in use" error message
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if (bind(listen_fd, p->ai_addr, p->ai_addrlen) < 0) {
      close(listen_fd);
      continue;
    }
    break;
  }

  // If we got here, it means we didn't get bound
  if (p == nullptr) {
    return -1;
  }

  freeaddrinfo(ai); // All done with this

  // Listen
  if (listen(listen_fd, 10) == -1) {
    return -1;
  }

  return listen_fd;
}

int main() {
  try {
    int listener = get_listener_socket();

    if (listener == -1) {
      fprintf(stderr, "error getting listening socket\n");
      return 1;
    }

    // accepting connection request
    int clientSocket = accept(listener, nullptr, nullptr);
    if (clientSocket == -1) {
      fprintf(stderr, "error accepting client socket\n");
      return 1;
    }

    // recieving data
    char buffer[1024] = {0};
    ssize_t n = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (n > 0) std::cout.write(buffer, n);

    // closing the socket.
    close(listener);

  } catch (const std::exception& e) {
    std::cerr << "fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
