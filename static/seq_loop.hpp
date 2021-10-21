#ifndef __SEQ_LOOP_H
#define __SEQ_LOOP_H

#include <functional>

#include <thread>
#include <vector>

class SeqLoop {
private:
  size_t threads;
  std::vector<std::thread> worker_threads;

    
public:
  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  template<typename TLS>
  void parfor (size_t beg, size_t end, size_t increment,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    TLS tls;
    before(tls);    
    for (size_t i=beg; i<end; i+= increment) {
      f(i, tls);
    }
    after(tls);
  }
  
  template<typename TLS>
  static void f_helper(size_t id_thread, std::function<void(int, TLS&)> f, TLS& tls, size_t part_size, size_t total_n, size_t total_threads, std::function<void(TLS&)> before) {
    before(tls);  
    int thread_end = (id_thread + 1) * part_size;
    if (id_thread == total_threads) { // Last part
      thread_end = total_n;
    }
    
    for (size_t i = id_thread * part_size; i < thread_end; i++) {
      f(i, tls);
    }
  }
  
  // New version of parfor
  template<typename TLS>
  void parfor_parallel (size_t beg, size_t end, size_t increment,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    std::vector<TLS> tlss (threads);
    
      
    
    size_t total_n = end - beg;
    size_t part_size = total_n / threads; // least number of parts each thread calculates. Last part may be larger
    
    // Create threads
    for (size_t id=0; id < threads; id+= 1) {
      std::thread new_thread (f_helper<TLS>, id, f, std::ref(tlss[id]), part_size, total_n, threads, before);
      worker_threads.push_back(std::move(new_thread));
    }
    
    // Wait for all worker threads to finish
    for (auto & t : worker_threads) {
      if (t.joinable()) {
          t.join();
      }
    }
    
    for (TLS tls : tlss) {
      after(tls);
    }
    
  }
  
  void set_thread_count(int count) { // Sets the number of threads for parallel parfor
    this -> threads = count;
  }
  
};

#endif
