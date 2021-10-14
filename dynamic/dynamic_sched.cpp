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

void worker_func(int functionid, int lowerBound, int n, float start, int intensity, float *temps, int thread_start, int thread_end) {
  float temp = 0.0f;
  
  SeqLoop sl;
  // parfor (beg , end, inc)
  sl.parfor(thread_start, thread_end, 1, {
      [&](int i) -> void{
	      float x_value = lowerBound + (i + 0.5f) * start;
        temp += get_function_value(functionid, x_value, intensity);
	    }
    }
  );
  *temps =  temp;
}

int main (int argc, char* argv[]) {

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }
  
  int fuctionID  = atoi(argv[1]);
  int lowerBound = atoi(argv[2]); // This is a
  int upperBound = atoi(argv[3]); // This is b
  int n          = atoi(argv[4]);
  int intensity  = atoi(argv[5]);
  int nbthreads  = atoi(argv[6]);
  
  auto startTime = system_clock::now();
  
  float result = 0;
  float start = (upperBound - lowerBound) / static_cast<float>(n);
  

  float temp = 0.0f;
  
  std::vector<std::thread> threads;
  float temps[nbthreads] = {0};
  int loop_i = n / nbthreads;
  
  for (int i = 0; i < nbthreads; i++) {
      int thread_start = (i) * loop_i;
      int thread_end  = ((i + 1) * loop_i) - 1;
      
      if (i == nbthreads - 1) {
          thread_end++;
      }
    
      std::thread minion_thread (worker_func, fuctionID, lowerBound, n, start, intensity, &temps[i], thread_start, thread_end);
      threads.push_back(std::move(minion_thread));
  }
  
  for (auto & t : threads) {
      if (t.joinable()) {
          t.join();
    }
  }
  
  for (int i = 0; i < nbthreads; i++) {
    //printf("temps[i]: %f\n", temps[i]);
    temp = temp + temps[i];
  }
  //printf("temp: %f\n", temp);
  //printf("start: %f\n", start);
  result = start * temp;
  //printf("%f", result);
  //printf("result: %f\n", result);
  std::cout << result;
  
  auto stopTime = system_clock::now();
  
  std::chrono::duration<double> diff = stopTime - startTime;
  
  
  std::cerr << diff.count();
  
  
  return 0;
}
