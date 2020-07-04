#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

class Worker {
private:
  int sock_, efd_;
  std::thread th_;
  void ThreadFunc();

public:
  Worker(const char *ip, const int port);
  void Start();
  void Shutdown();
  void Join();
};

Worker::Worker(const char *ip, const int port) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  if ((sock_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw "socket() error";
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  if (bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    throw "bind() error";
  }

  if ((efd_ = eventfd(0, 0)) == -1) {
    throw "eventfd() error";
  }

  std::cout << "  "
            << "Wait by " << ip << ":" << port << std::endl;
}

void Worker::ThreadFunc() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  fd_set fds, rfds;
  FD_ZERO(&rfds);

  int max_fd = -1;
  FD_SET(sock_, &rfds);
  max_fd = (sock_ > max_fd ? sock_ : max_fd);
  FD_SET(efd_, &rfds);
  max_fd = (efd_ > max_fd ? efd_ : max_fd);

  while (1) {
    std::memcpy(&fds, &rfds, sizeof(fd_set));
    if (select(max_fd + 1, &fds, NULL, NULL, NULL) == -1) {
      throw "select() error";
    }

    if (FD_ISSET(sock_, &fds)) {
      char buf[256];
      memset(buf, 0, sizeof(buf));
      recv(sock_, buf, sizeof(buf), 0);
      std::cout << "  "
                << "recv from socket: " << buf << std::endl;
    }
    if (FD_ISSET(efd_, &fds)) {
      uint64_t num = 0;
      read(efd_, &num, sizeof(num));
      std::cout << "  "
                << "recv from eventfd" << std::endl;
      break;
    }
  }
}

void Worker::Start() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  th_ = std::thread([this] { ThreadFunc(); });
}

void Worker::Shutdown() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  uint64_t num = 1;
  if (write(efd_, &num, sizeof(num)) == -1) {
    std::cout << "efd_ = " << efd_ << std::endl;
    throw "write() error";
  }
}

void Worker::Join() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  if (th_.joinable()) {
    th_.join();
  }

  std::cout << "end" << std::endl;
}

int main(int argc, char const *argv[]) {
  Worker worker("0.0.0.0", 10000);
  worker.Start();
  getchar();
  std::cout << "getchar" << std::endl;
  worker.Shutdown();
  worker.Join();
  return 0;
}
