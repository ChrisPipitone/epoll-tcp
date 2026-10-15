#pragma once
#include <memory>
#include <netdb.h>
#include <stdexcept>

// Resolve required raw addrinfo result ptr into a controlled locally scoped
// ptr and return a smart ptr in its place

using AddrInfoPtr = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>;

inline AddrInfoPtr resolve(const char* host, const char* port,
                           const addrinfo& hints) {
  addrinfo* raw = nullptr;
  if (int result = getaddrinfo(host, port, &hints, &raw); result != 0)
    throw std::runtime_error(gai_strerror(result));
  return AddrInfoPtr(raw, freeaddrinfo);
}
