#include "common/Fd.h"
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

Fd get_listener_socket() {
  constexpr const char* PORT = "8080";
  addrinfo hints{};
  addrinfo* ai = nullptr;
  int yes = 1;
  int last_err = 0;

  // Get us a socket and bind it
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int result = getaddrinfo(nullptr, PORT, &hints, &ai);
  if (result != 0) {
    throw std::runtime_error(gai_strerror(result));
  }

  for (auto p = ai; p != nullptr; p = p->ai_next) {
    Fd listen_fd(socket(p->ai_family, p->ai_socktype, p->ai_protocol));
    if (listen_fd.get() < 0) {
      last_err = errno;
      continue;
    }

    // Lose the pesky "address already in use" error message
    if (setsockopt(listen_fd.get(), SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(yes)) < 0) {
      std::cerr << "warn: SO_REUSEADDR: " << std::strerror(errno) << "\n";
    }

    if (bind(listen_fd.get(), p->ai_addr, p->ai_addrlen) < 0) {
      last_err = errno;
      continue;
    }

    if (listen(listen_fd.get(), 10) == -1) { // ik ik
      last_err = errno;
      freeaddrinfo(ai);
      throw std::system_error(last_err, std::generic_category(), "listen");
    } else {
      freeaddrinfo(ai);
      return listen_fd;
    }
  }

  freeaddrinfo(ai);
  throw std::system_error(last_err, std::generic_category(), "bind");
}

int main() {
  try {
    Fd listener = get_listener_socket();

    while (true) {
      // accepting connection request
      Fd clientSocket(accept(listener.get(), nullptr, nullptr));
      if (clientSocket.get() == -1) {
        if (errno == EINTR || errno == ECONNABORTED) continue;
        throw std::system_error(errno, std::generic_category(), "accept");
      }

      // recieving data
      char buffer[1024] = {0};
      ssize_t n = recv(clientSocket.get(), buffer, sizeof(buffer), 0);
      if (n > 0) std::cout.write(buffer, n);
    }

  } catch (const std::exception& e) {
    std::cerr << "fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
