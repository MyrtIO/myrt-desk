#include <ArduinoJson.h>
#include <MyrtIO_MQTTController.h>
#include <platforms.h>

#include "config_data.h"

// TODO: add generation of config topic
const char* kLightOutTopic = "myrt/desk/light";
const char* kLightInTopic = "myrt/desk/light/set";
const char* kLightFPSInTopic = "myrt/desk/light/fps/set";
const char* kLightConfigTopic = "homeassistant/light/myrt_desk_light/config";

const size_t kLightReportInterval = 60000;
const size_t kLightConfigReportInterval = 60000 * 5;

JsonDocument lightStateDoc;
char lightStateBuffer[256];

const uint8_t kLightEffectCount = 4;
const char* effectNames[kLightEffectCount] = {
	"static",
	"rainbow",
	"loading",
	"fill",
};

void reportLightState(PubSubClient* client) {
	auto light = DI_INJECT(IOLight);
	if (light->getMode() == LightMode::RGBMode) {
		lightStateDoc["color_mode"] = "rgb";
		auto color = light->getColor();
		auto colorProp = lightStateDoc["color"].to<JsonObject>();
		colorProp["r"] = color.r;
		colorProp["g"] = color.g;
		colorProp["b"] = color.b;
	} else {
		lightStateDoc["color_mode"] = "color_temp";
		lightStateDoc["color_temp"] = light->getTemperature();
	}
	lightStateDoc["effect"] = effectNames[light->getEffect()];
	lightStateDoc["brightness"] = light->getBrightness();
	lightStateDoc["state"] = light->getPower() ? "ON" : "OFF";

	serializeJson(lightStateDoc, lightStateBuffer);
	client->publish(kLightOutTopic, &lightStateBuffer[0]);
}

void updateLightState(PubSubClient* client, byte* payload, uint length) {
	auto light = DI_INJECT(IOLight);
	deserializeJson(lightStateDoc, payload);

	auto isEnabled = light->getPower();
	auto color = light->getColor();
	auto brightness = light->getBrightness();

	bool reqIsEnabled = strcmp(lightStateDoc["state"], "ON") == 0;
	uint8_t reqBrightness = lightStateDoc["brightness"];
	auto colorProp = lightStateDoc["color"].as<JsonObject>();
	RGBColor reqColor = RGBColor::Black;

	if (colorProp != nullptr) {
		reqColor = RGBColor(colorProp["r"], colorProp["g"], colorProp["b"]);
	}

	uint16_t reqColorTemp = lightStateDoc["color_temp"];
	const char* effectValue = lightStateDoc["effect"];
	uint8_t reqEffect = 255;

	if (effectValue != nullptr) {
		for (uint8_t i = 0; i < kLightEffectCount; i++) {
			if (strcmp(effectValue, effectNames[i]) == 0) {
				reqEffect = i;
				break;
			}
		}
	}

	bool hasChanges = false;

	if (reqIsEnabled != isEnabled) {
		light->setPower(reqIsEnabled);
		hasChanges = true;
	} else if (reqBrightness != brightness && reqBrightness != 0) {
		light->setBrightness(reqBrightness);
		hasChanges = true;
	}

	if (reqEffect != light->getEffect() && reqEffect != 255) {
		light->setEffect(reqEffect);
		hasChanges = true;
	}

	if (reqColor != color && reqColor != RGBColor::Black) {
		light->setColor(reqColor);
		hasChanges = true;
	} else if (reqColorTemp != 0) {
		light->setColorTemperature(reqColorTemp);
		hasChanges = true;
	}

	if (hasChanges) {
		reportLightState(client);
	}
}

void updateFPS(PubSubClient* client, byte* payload, uint length) {
	auto light = DI_INJECT(IOLight);
	uint8_t fps = atoi((char*)payload);
	light->setFPS(fps);
}

void reportLightConfig(PubSubClient* client) {
	client->publish(kLightConfigTopic, kLightConfig);
}

void registerLightTopics(PubSubServer* server) {
	server
		->report(reportLightConfig, kLightConfigReportInterval)
		->report(reportLightState, kLightReportInterval)
		->on(kLightInTopic, updateLightState)
		->on(kLightFPSInTopic, updateFPS);
}
