# include <functional>
# include <iostream>
# include <list>
# include <mutex>
# include <thread>
# include <vector>
# include <atomic>
# include <condition_variable>

using namespace std;

class Workers {
  vector<thread> workerThreads;
  mutex tasksMutex;
  list<function<void()>> tasks;
  int numOfThreads;
  atomic<bool> run{true};
  condition_variable cv;

public:
  Workers(int amountOfThreads) {
    numOfThreads = amountOfThreads;
  }

  void start() {
    run = true;
    for (int i = 0; i < numOfThreads; i++) {
      workerThreads.emplace_back([&] {
        while (true) {
          function<void()> task;
          {
            unique_lock<mutex> lock(tasksMutex);

            while (tasks.empty()) {
              if (!run) {
                return;
              }
              cv.wait(lock);
            }

            task = *tasks.begin();
            tasks.pop_front();
          }
          if (task)
            task();
        }
      });
    }
  }
  void post(function<void()> func) {
    tasks.emplace_back(func);
    cv.notify_one();
  }
  void post_timeout(function<void()> func, int ms) {
    tasks.emplace_back([ms, func] {
      this_thread::sleep_for(chrono::milliseconds(ms));
      func();
    });
  }
  void join() {
    for (auto &thread : workerThreads) {
      thread.join();
    }
  }
  void stop() {
    run.exchange(false);
    cv.notify_all();
  }
};

int main() {
  Workers workerThreads(4);
  Workers eventLoop(1);

  workerThreads.start(); // Create 4 internal threads
  eventLoop.start();     // Create 1 internal thread

  for (int i = 0; i < 5; i++) {
    workerThreads.post([i, &eventLoop] {
      this_thread::sleep_for(2s);

      eventLoop.post([i] {
        cout << "Result: " << i << ';' << endl;
      });
    });
  }

  workerThreads.post_timeout([] {
    cout << "delayed" << endl;
  }, 3000);

  this_thread::sleep_for(chrono::seconds(10));

  workerThreads.stop();
  eventLoop.stop();

  workerThreads.join(); // Calls join() on the worker threads
  eventLoop.join();
}