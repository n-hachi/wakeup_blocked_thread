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
};

Worker::Worker(const char *ip, const int port) {
  sock_ = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  bind(sock_, (struct sockaddr *)&addr, sizeof(addr));

  efd_ = eventfd(0, 0);
}

void Worker::ThreadFunc() {
  fd_set fds, rfds;
  FD_ZERO(&rfds);

  int max_fd = -1;
  FD_SET(sock_, &rfds);
  max_fd = (sock_ > max_fd ? sock_ : max_fd);
  FD_SET(efd_, &rfds);
  max_fd = (efd_ > max_fd ? efd_ : max_fd);

  while (1) {
    std::memcpy(&fds, &rfds, sizeof(fd_set));
    select(max_fd, &fds, NULL, NULL, NULL);

    if (FD_ISSET(sock_, &fds)) {
      char buf[256];
      memset(buf, 0, sizeof(buf));
      recv(sock_, buf, sizeof(buf), 0);
      std::cout << "recv from socket: " << buf << std::endl;
    }
    if (FD_ISSET(efd_, &fds)) {
      std::cout << "recv from eventfd" << std::endl;
      break;
    }
  }
}

void Worker::Start() {
  th_ = std::thread([this] { ThreadFunc(); });
}

void Worker::Shutdown() {
  int num = 1;
  write(efd_, &num, sizeof(num));
}

int main(int argc, char const *argv[]) {
  Worker worker("0.0.0.0", 10000);
  worker.Start();
  getchar();
  std::cout << "getchar" << std::endl;
  return 0;
}
