#pragma once

#include "device.h"
#include "json_document.h"

namespace HomeAssistant {
	class Entity {
	public:
		const char* name;
		const char* icon;
		Device device;

		Entity(
			const char* identifier,
			const char* name,
			const char* icon,
			bool writable,
			const Device& device
		) : name(name),
			icon(icon),
			device(device),
			idSuffix_(identifier),
			writable_(writable) {}

		template <typename TDestination>
		unsigned int serializeConfig(TDestination& buffer) {
			json.clear();
			json["name"] = name;
			json["unique_id"] = id();
			if (icon != nullptr) {
				json["icon"] = icon;
			}
			json["state_topic"] = stateTopic();
			if (commandTopic_ != nullptr) {
				json["command_topic"] = commandTopic();
			}
			device.fillConfig();
			fillConfig();
			return serializeJson(json, buffer);
		}

		char* id() {
			if (uniqueID_ == nullptr) {
				size_t idLen = strlen(device.id) + strlen(idSuffix_) + 2;
				uniqueID_ = (char*)malloc(idLen);
				snprintf(uniqueID_, idLen, "%s_%s", device.id, idSuffix_);
			}
			return uniqueID_;
		}

		char* stateTopic() {
			if (stateTopic_ == nullptr) {
				size_t stateTopicLen = strlen(device.mqttNamespace) + strlen(idSuffix_) + 2;
				stateTopic_ = (char*)malloc(stateTopicLen);
				snprintf(stateTopic_, stateTopicLen, "%s/%s", device.mqttNamespace, idSuffix_);
				if (writable_) {
					stateTopicLen += 5;
					commandTopic_ = (char*)malloc(stateTopicLen);
					snprintf(commandTopic_, stateTopicLen, "%s/set", stateTopic_);
				}
			}
			return stateTopic_;
		}
		char* commandTopic() {
			if (writable_) {
				if (commandTopic_ == nullptr) {
					stateTopic();
				}
				return commandTopic_;
			}
			return nullptr;
		}

		char* configTopic() {
			if (configTopic_ == nullptr) {
				size_t configTopicLen = strlen(component()) + strlen(id()) + 23;
				configTopic_ = (char*)malloc(configTopicLen);
				snprintf(configTopic_, configTopicLen, "test/%s/%s/config", component(), id());
			}
			return configTopic_;
		}

		virtual const char* component();
		virtual void fillConfig() = 0;

	private:
		const char* idSuffix_;
		char* uniqueID_;
		char* stateTopic_;
		char* commandTopic_;
		char* configTopic_;
		bool writable_ = false;
	};
};
