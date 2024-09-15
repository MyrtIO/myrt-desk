#include "io_logger.h"

void IOLogger::print(const char* message) {
    printPrefix_();
    stream_->print(message);
    stream_->print("\n");
}

void IOLogger::debug(const char* message) {
#if IO_DEBUG
    this->print(message);
#endif
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

IStringBuilder* IOLogger::builder() {
    builder_.prepare();
    return &builder_;
}

IStringBuilder* IOLogger::debugBuilder() {
#if IO_DEBUG
    return &builder_;
#else
    return &dummyBuilder_;
#endif
}

IOLogger IOLog = IOLogger(&Serial);
