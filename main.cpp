#include <iostream>

class Worker {
private:
public:
  Worker(const char *ip, const int port);
};

Worker::Worker(const char *ip, const int port) {}

int main(int argc, char const *argv[]) {
  Worker worker("0.0.0.0", 10000);
  return 0;
}
