#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// Global variables for synchronization
std::mutex mtx; // Mutex to ensure only one thread accesses shared data at a time
std::condition_variable cv; // Condition variable to control the flow between threads
bool finishedCountingUp = false; // Flag to signal when the first thread has finished counting up

// Function to count up from 1 to 20
void countUp() {
    for (int i = 1; i <= 20; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex to prevent other threads from interfering
        std::cout << "Counting Up: " << i << std::endl; // Print the current count
    }

    // After counting up, signal that the first task is complete
    {
        std::lock_guard<std::mutex> lock(mtx);
        finishedCountingUp = true; // Set the flag to true to indicate completion
    }
    cv.notify_one(); // Notify the waiting thread that it can proceed
}

// Function to count down from 20 to 0
void countDown() {
    std::unique_lock<std::mutex> lock(mtx); // Lock the mutex and prepare to wait
    cv.wait(lock, [&]() { return finishedCountingUp; }); // Wait until finishedCountingUp is true

    // Begin counting down once notified
    for (int i = 20; i >= 0; --i) {
        std::cout << "Counting Down: " << i << std::endl; // Print the current count
    }
}

int main() {
    // Create two threads: one for counting up and one for counting down
    std::thread t1(countUp); // Start the first thread to count up
    std::thread t2(countDown); // Start the second thread to count down

    // Wait for both threads to finish before exiting the program
    t1.join(); // Wait for the countUp thread to complete
    t2.join(); // Wait for the countDown thread to complete

    return 0; // End of the program
}