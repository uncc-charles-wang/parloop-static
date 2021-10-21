#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>
#include "seq_loop.hpp"


#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

using namespace std::chrono;

float get_function_value(int f,float x, int intensity) {
    switch(f) {
      case 1:
          return f1(x, intensity);
          break;
      case 2:
          return f2(x, intensity);
          break;
      case 3:
          return f3(x, intensity);
          break;
      case 4:
          return f4(x, intensity);
          break;
  }
  
  std::cout << "Error: f is not valid.\n";
  return 0.0f;
}

int main (int argc, char* argv[]) {

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }
  
  int fuctionID    = atoi(argv[1]);
  float lowerBound = atof(argv[2]); // This is a
  float upperBound = atof(argv[3]); // This is b
  int n            = atoi(argv[4]);
  int intensity    = atoi(argv[5]);
  int nbthreads    = atoi(argv[6]);
  
  auto startTime = system_clock::now();
  
  float result = 0;
  float start = (upperBound - lowerBound) / static_cast<float>(n);

  float temp = 0.0f;
  
  SeqLoop sl;
  
  sl.set_thread_count(nbthreads);
  
  sl.parfor_parallel<float>(0, n, 1,
    [&](float& tls) -> void{ // Before
      tls = 0;
    },
    [&](int i, float& tls) -> void{
      float x_value = lowerBound + (i + 0.5f) * start;
      tls += get_function_value(fuctionID, x_value, intensity);
    },
    [&](float& tls) -> void{ // After
      temp += tls;
    }
  );
  result = start * temp;
  
  std::cout << result;
  
  auto stopTime = system_clock::now();
  
  std::chrono::duration<double> diff = stopTime - startTime;
  
  
  std::cerr << diff.count();
  
  
  return 0;
}
