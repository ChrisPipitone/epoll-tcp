#include "common/AddrInfo.h"
#include "common/Fd.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <system_error>

Fd get_listener_socket() {
  constexpr const char* kPort = "8080";
  addrinfo hints{};
  int yes = 1;
  int last_err = 0;

  // Get us a socket and bind it
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  auto addr_info = resolve(nullptr, kPort, hints);

  for (auto* p = addr_info.get(); p != nullptr; p = p->ai_next) {
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
    // SOMAXCONN prob later, idk what so see how capping would affect things
    // later
    if (listen(listen_fd.get(), 10) == -1) {
      last_err = errno;
      throw std::system_error(last_err, std::generic_category(), "listen");
    }
    return listen_fd;
  }
  throw std::system_error(last_err, std::generic_category(), "bind");
}

int main() {
  try {
    Fd listener = get_listener_socket();

    while (true) {
      // accepting connection request
      Fd client_fd(accept(listener.get(), nullptr, nullptr));
      if (client_fd.get() == -1) {
        if (errno == EINTR || errno == ECONNABORTED) continue;
        throw std::system_error(errno, std::generic_category(), "accept");
      }

      // recieving data
      char buffer[1024] = {0};
      ssize_t n = recv(client_fd.get(), buffer, sizeof(buffer), 0);
      if (n > 0) std::cout.write(buffer, n);
    }

  } catch (const std::exception& e) {
    std::cerr << "fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
