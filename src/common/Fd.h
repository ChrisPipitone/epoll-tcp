#pragma once
#include <unistd.h>
#include <utility>

class Fd {
public:
  Fd() = default;
  explicit Fd(int fd) : fd_(fd) {}
  ~Fd() {
    if (fd_ >= 0) close(fd_);
  }

  Fd(Fd&& other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

  Fd& operator=(Fd&& rhs) noexcept {
    if (this != &rhs) {
      if (fd_ >= 0) close(fd_);
      fd_ = std::exchange(rhs.fd_, -1);
    }
    return *this;
  }

  // Delete copying to avoid duplicating of fd's
  // and accidental closes
  Fd(const Fd&) = delete;
  Fd& operator=(const Fd&) = delete;

  [[nodiscard]] int get() const { return fd_; }

private:
  int fd_ = -1; // 0 is stdin, -1 == empty
};
