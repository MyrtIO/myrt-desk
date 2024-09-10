#include "logger.h"

IOLogBuilder* IOLogBuilder::append(const char* message) {
    for (int i = 0; i < strlen(message); i++) {
        buffer_[size_] = message[i];
        size_++;
    }
    return this;
}

IOLogBuilder* IOLogBuilder::append(const uint8_t message) {
    char buffer[4];
    itoa(message, buffer, 10);
    return append(buffer);
}

IOLogBuilder* IOLogBuilder::append(const uint16_t message) {
    char buffer[10];
    itoa(message, buffer, 10);
    return append(buffer);
}

IOLogBuilder* IOLogBuilder::append(const uint32_t message) {
    char buffer[10];
    itoa(message, buffer, 10);
    return append(buffer);
}

void IOLogBuilder::prepare(bool debug) {
    debug_ = debug;
    for (int i = 0; i < 256; i++) {
        buffer_[i] = 0;
    }
    size_ = 0;
}

void IOLogBuilder::flush() {
    flusher_->flush(buffer_, size_);
}

char* IOLogBuilder::buffer() {
    return buffer_;
}

uint8_t IOLogBuilder::size() {
    return size_;
}

void IOLogger::print(const char* message) {
    printPrefix_();
    stream_->print(message);
    stream_->print("\n");
}

void IOLogger::debug(const char* message) {
    this->print(message);
}

void IOLogger::flush(const char* message, uint8_t length) {
    if (length > 0) {
        printPrefix_();
        stream_->write(message, length);
        stream_->print("\n");
        stream_->flush();
    }
}

void IOLogger::printPrefix_() {
    char buffer[12];
    for (int i = 0; i < 12; i++) {
        buffer[i] = ' ';
    }
    auto now = millis();
    char prefixBuffer[10];
    uint32_t seconds = now / 1000;
    uint32_t minutes = seconds / 60;
    seconds = seconds % 60;
    uint32_t hours = minutes / 60;
    minutes = minutes % 60;
    sprintf(prefixBuffer, "%02d:%02d:%02d", hours, minutes, seconds);
    stream_->print(prefixBuffer);
    stream_->print(" ");
    if (moduleName_ != nullptr) {
        stream_->print("[");
        stream_->print(moduleName_);
        stream_->print("] ");
    }
}

IOLogBuilder* IOLogger::builder() {
    builder_.prepare(false);
    return &builder_;
}

IOLogBuilder* IOLogger::debugBuilder() {
    builder_.prepare(true);
    return &builder_;
}

IOLogger IOLog = IOLogger(&Serial);
