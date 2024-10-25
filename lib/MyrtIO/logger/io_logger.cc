#include "io_logger.h"

IOStringBuilder ioMessageBuilder;

Print* ioLogOutput = &Serial;

void IOLogger::print(const char* message) {
	builder()
		->append(message)
		->append("\n");
	flush();
}

void IOLogger::flush() {
	auto length = ioMessageBuilder.size();
	if (length > 0) {
		ioMessageBuilder.append("\n");
		ioLogOutput->write(ioMessageBuilder.buffer(), length);
		ioLogOutput->flush();
	}
}

IOStringBuilder* IOLogger::builder() {
	ioMessageBuilder.clear();
	appendPrefix_(&ioMessageBuilder);
	return &ioMessageBuilder;
}

void IOLogger::appendPrefix_(IOStringBuilder* message) {
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
	message
		->append(prefixBuffer)
		->append(" ");
	if (moduleName_ != nullptr) {
		message
			->append("[")
			->append(moduleName_)
			->append("] ");
	}
}

void IOLogger::setOutput(Print* stream) {
	ioLogOutput = stream;
}

