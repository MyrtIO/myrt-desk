#pragma once

#include <PubSubClient.h>
#include <Print.h>

#ifndef PUBSTREAM_BUFFER_SIZE
	#define PUBSTREAM_BUFFER_SIZE 1024
#endif

class TopicStream : public Print {
public:
	TopicStream(PubSubClient* client, const char* topic):
		client_(client),
		topic_(topic) {}

	void print(const char* message) {
		if (size_ < sizeof(buffer_)) {
			strncat(buffer_, message, sizeof(buffer_) - size_);
			size_ += strlen(message);
		}
		flush();
	}

	size_t write(uint8_t value) {
		if (size_ < sizeof(buffer_)) {
			buffer_[size_] = value;
			size_ += 1;
		} else {
			return 0;
		}
		flush();
		return 1;
	}

	size_t write(const uint8_t *buffer, size_t size) {
		if (size_ + size < sizeof(buffer_)) {
			memcpy(buffer_ + size_, buffer, size);
			size_ += size;
		} else {
			return 0;
		}
		flush();
		return size;
	}

	int availableForWrite() {
		return sizeof(buffer_) - size_;
	}

	void flush() {
		if (size_ > 0 && client_->connected()) {
			client_->publish(topic_, buffer_, size_);
			size_ = 0;
		}
	}

private:
	PubSubClient* client_;
	const char* topic_;
	char buffer_[PUBSTREAM_BUFFER_SIZE];
	size_t size_ = 0;
};
