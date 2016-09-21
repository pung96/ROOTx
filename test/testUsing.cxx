#include <iostream>

using std::cout;
using std::endl;
using std::min;

  void print(){
    std::cout<<"print()"<<std::endl;
  }

namespace ROOTX{
  void print(){
    std::cout<<"ROOTX::print()"<<std::endl;
  }

}

int main(){
  using ROOTX::print;
  ROOTX::print();
  print();
}
