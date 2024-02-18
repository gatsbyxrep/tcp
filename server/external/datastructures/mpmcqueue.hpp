#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

template <typename T>
class MPMCBlockingQueue {
public:
	void push(T value);
    std::optional<T> pop();
    size_t size() const;
private:
	std::queue<T> queue; // Guarded by mutex
    mutable std::mutex mutex;
    mutable std::condition_variable dataPushed;
};

template<typename T>
void MPMCBlockingQueue<T>::push(T value) {
	std::lock_guard guard {mutex};
	queue.push(std::move(value));
	dataPushed.notify_one();
}

template<typename T>
std::optional<T> MPMCBlockingQueue<T>::pop() {
    auto const timeout = std::chrono::steady_clock::now() + std::chrono::seconds(1);
	std::unique_lock<std::mutex> lock{mutex};
	while(queue.empty()) {
        if(dataPushed.wait_until(lock, timeout) == std::cv_status::timeout) {
            return std::nullopt;
        }
	}
	T value = queue.front();
	queue.pop();
    return value;
}

template<typename T>
size_t MPMCBlockingQueue<T>::size() const {
    std::lock_guard guard {mutex};
    return queue.size();
}



#endif // BLOCKINGQUEUE_H
