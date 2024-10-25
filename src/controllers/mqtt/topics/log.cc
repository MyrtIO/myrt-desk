#include <PrintBuffer.h>
#include <PubSubServer.h>

const char* kLogTopic = "myrt/desk/log";
const size_t kLogReportInterval = 50;

PrintBuffer mqttLogStream = PrintBuffer<1024>();

void reportLog(PubSubClient* client) {
	if (mqttLogStream.available() == 0) {
		return;
	}
	client->beginPublish(kLogTopic, mqttLogStream.available(), true);
	while (mqttLogStream.available() > 0) {
		client->write(mqttLogStream.read());
	}
	client->endPublish();
}

void registerLogTopics(PubSubServer* server) {
	server->report(reportLog, kLogReportInterval);
}
