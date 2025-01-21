#include <bits/stdc++.h>

const int NUM_PRODUCERS = 1;
const int NUM_CONSUMERS = 2;
const int BUFFER_SIZE = 10;


std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;

void producer(int id) {
    std::random_device rd;
    std::mt19937 randM(rd());
    std::uniform_int_distribution<> distrib(100, 999);
    int item = distrib(randM);
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return buffer.size() < BUFFER_SIZE; }); 
        buffer.push(item);
        std::cout << "Producer " << id << " produced: " << item << std::endl;
    }
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
}

void consumer(int id) {
    while (true) {
        int item;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return !buffer.empty(); });
            item = buffer.front();
            buffer.pop();
            std::cout << "Consumer " << id << " consumed: " << item << std::endl;
        }
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (item == 0) break; 
    }
}

int main() {
    std::clock_t start = clock();

    std::vector<std::thread> producers;
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer, i);
    }
    std::vector<std::thread> consumers;
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(consumer, i);
    }

    for (auto& thread : producers) {
        thread.join();
    }
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            buffer.push(0);
        }
        cv.notify_one();
    }


    for (auto& thread : consumers) {
        thread.join();
    }
     delete new int(1);

    std::clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "The time: " << seconds << ", seconds)\n";
    return 0;
}