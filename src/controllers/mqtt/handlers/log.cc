#include <PubSubServer.h>

const char* kLogTopic = "myrt/desk/log";
const size_t kLogReportInterval = 50;

Stream* logStream;

void reportLog(PubSubClient* client) {
	if (logStream == nullptr || logStream->available() == 0) {
		return;
	}
	client->beginPublish(kLogTopic, logStream->available(), true);
	while (logStream->available() > 0) {
		client->write(logStream->read());
	}
	client->endPublish();
}

void registerLogTopics(PubSubServer* server, Stream* stream) {
	server->report(reportLog, kLogReportInterval);
	logStream = stream;
}
