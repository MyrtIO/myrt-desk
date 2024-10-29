#pragma once

#include <Stream.h>

// #define SIZE 1024

template <int SIZE = 1024>
class RingBuffer : public Stream {
public:
    size_t write(uint8_t newVal) {
        size_t i = (head_ + 1) % SIZE;
        if (i != tail_) {
            buffer_[head_] = newVal;
            head_ = i;
            return 1;
        }
		return 0;
    }

    int availableForWrite() {
        return (head_ + 1) % SIZE != tail_;
    }

    int read() {
        if (head_ == tail_) {
			return 0;
		}
        uint8_t value = buffer_[tail_];
        tail_ = (tail_ + 1) % SIZE;
        return value;
    }

    int peek() {
        return buffer_[tail_];
    }

    int available() {
        return (SIZE + head_ - tail_) % SIZE;
    }

    void clear() {
        head_ = 0;
		tail_ = 0;
    }

private:
    uint8_t buffer_[SIZE];
    size_t head_ = 0;
	size_t tail_ = 0;
};
