#pragma once

#include <Print.h>

template <int SIZE>
class PrintBuffer: public Print {
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

    uint8_t read() {
        if (head_ == tail_) return 0;   // буфер пуст
        uint8_t val = buffer_[tail_];      // берём с хвоста
        tail_ = (tail_ + 1) % SIZE;     // хвост двигаем
        return val;                   // возвращаем
    }

    uint8_t peek() {
        return buffer_[tail_];
    }

    size_t available() {
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
