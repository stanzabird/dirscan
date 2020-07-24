#include <iostream>
#include <future>
#include <thread>


int main(int argc, char* argv[]) {
  auto n = std::thread::hardware_concurrency();
  
  std::cout
    << n << " concurrent threads are supported.\n";
  
  return 0;
}
