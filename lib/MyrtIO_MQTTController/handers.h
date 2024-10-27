#pragma once

class ITopicHandler {
public:
	virtual void handle(const char* topic, const char* payload) = 0;
	virtual ~ITopicHandler() { }
};
