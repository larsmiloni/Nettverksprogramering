#include <iostream>
#include <thread>
#include <vector>

using namespace std;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    // Start with the lowest 6k+1 greater than sqrt(n)
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

void primeNumbers2(int min, int max, int numThreads, vector<int>& sortedPrimes) {
  vector<thread> threads;
  vector<vector<int>> allPrimeNumbers(numThreads);

  int split = (max - min) / numThreads + ((max - min) % numThreads != 0);

  for (int i = 0; i < numThreads; i++) {
    threads.emplace_back([i, min, split, max, numThreads, &allPrimeNumbers] {
      int start = min + i * split;
      int end = min + (i + 1) * split;
      if (i == numThreads - 1) {
          end = max + 1;
      }

      for (int j = start; j < end; j++) {
        if (isPrime(j)) {
        allPrimeNumbers[i].push_back(j);
        }
      }
    });
  }

  for (auto &thread : threads)
    thread.join();

  // Concatenate vectors into one sorted vector
  for (const auto& primes : allPrimeNumbers) {
    sortedPrimes.insert(sortedPrimes.end(), primes.begin(), primes.end());
  }
}



int main() {
    auto start = chrono::steady_clock::now(); // Start timing

    vector<int> sortedPrimes;
    primeNumbers2(5, 1000000, 4, sortedPrimes);

    auto end = chrono::steady_clock::now(); // End timing

    for (int num : sortedPrimes) {
        cout << num << " ";
    }
    cout << endl;

    //Calculate and print the duration
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "Execution time: " << elapsed_seconds.count() << endl;

    return 0;
}
