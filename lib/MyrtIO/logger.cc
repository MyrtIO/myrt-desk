#include "logger.h"

void IOLogger::print(const char* message) {
    printPrefix_();
    stream_->print(message);
    stream_->print("\n");
}

void IOLogger::debug(const char* message) {
    this->print(message);
}

void IOLogger::append(const char* message) {
    for (int i = 0; i < strlen(message); i++) {
        buffer_[size_] = message[i];
        size_++;
    }
}

void IOLogger::flush() {
    if (size_ > 0) {
        printPrefix_();
        stream_->write(buffer_, size_);
        stream_->print("\n");
        stream_->flush();
        for (int i = 0; i < size_; i++) {
            buffer_[i] = 0;
        }
        size_ = 0;
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
    seconds          = seconds % 60;
    uint32_t hours   = minutes / 60;
    minutes          = minutes % 60;
    sprintf(prefixBuffer, "%02d:%02d:%02d", hours, minutes, seconds);
    stream_->print(prefixBuffer);
    stream_->print(" ");
    if (moduleName_ != nullptr) {
        stream_->print("[");
        stream_->print(moduleName_);
        stream_->print("] ");
    }
}

IOLogger IOLog = IOLogger(&Serial);
