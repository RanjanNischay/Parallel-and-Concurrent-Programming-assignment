#include <iostream>
#include <atomic>
#include "Lock.h"
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>
#include <unistd.h>
#include <random>
#include <fstream>

using namespace std;

class base{
 private:
  atomic<bool> flag;
  public:
  void print(){
      flag.store(true);
      flag.store(false);
      cout<<flag.load()<<endl;
  }
};

int main(){
    base b;
    b.print();
    return 0;
}