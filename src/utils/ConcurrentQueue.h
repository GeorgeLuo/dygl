#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ConcurrentQueue {
private:
    mutable std::mutex mtx;
    std::queue<T> queue;
    std::condition_variable cv;

public:
    ConcurrentQueue() = default;

    ConcurrentQueue(const ConcurrentQueue& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        queue = other.queue;
    }

    ConcurrentQueue& operator=(const ConcurrentQueue& other) {
        if (this != &other) {
            std::lock(mtx, other.mtx);
            std::lock_guard<std::mutex> lockThis(mtx, std::adopt_lock);
            std::lock_guard<std::mutex> lockOther(other.mtx, std::adopt_lock);
            queue = other.queue;
        }
        return *this;
    }

    void Push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
        cv.notify_one();
    }
    
    bool TryPop(T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) {
            return false;
        }
        value = queue.front();
        queue.pop();
        return true;
    }

    std::size_t Size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};
