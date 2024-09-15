#include "config_data.h"
#include <ArduinoJson.h>
#include <platform.h>

#include "topics.h"

const char* kLightOutTopic = "myrt/desk/light";
const char* kLightInTopic = "myrt/desk/light/set";
const char* kLightConfigTopic = "homeassistant/light/myrt_desk_light/config";
const size_t kLightReportInterval = 60000;

const char* kJsonStringState = "state";
const char* kJsonStringOn = "ON";
const char* kJsonStringOff = "OFF";
const char* kJsonStringColor = "color";
const char* kJsonStringBrightness = "brightness";
const char* kJsonStringR = "r";
const char* kJsonStringG = "g";
const char* kJsonStringB = "b";
const char* kJsonStringRGB = "rgb";
const char* kJsonStringEffect = "effect";
const char* kJsonStringStatic = "static";
const char* kJsonStringRainbow = "rainbow";
const char* kJsonStringLoading = "loading";
const char* kJsonStringColorMode = "color_mode";
const char* kJsonStringColorTemp = "color_temp";

JsonDocument lightStateDoc;
char lightStateBuffer[256];

const char* effectNames[3] = { kJsonStringStatic, kJsonStringRainbow, kJsonStringLoading };

void reportLightState(PubSubClient* client) {
    auto platform = IO_INJECT(ILightPlatform);

    if (platform->getMode() == LightMode::RGBMode) {
        lightStateDoc[kJsonStringColorMode] = kJsonStringRGB;
        CRGB color = platform->getColor();
        auto colorProp = lightStateDoc[kJsonStringColor].to<JsonObject>();
        colorProp[kJsonStringR] = color.r;
        colorProp[kJsonStringG] = color.g;
        colorProp[kJsonStringB] = color.b;
    } else {
        lightStateDoc[kJsonStringColorMode] = kJsonStringColorTemp;
        lightStateDoc[kJsonStringColorTemp] = platform->getTemperature();
    }

    lightStateDoc[kJsonStringEffect] = effectNames[platform->getEffect()];
    lightStateDoc[kJsonStringBrightness] = platform->getBrightness();
    lightStateDoc[kJsonStringState] = platform->getPower() ? kJsonStringOn : kJsonStringOff;

    // Serialize to JSON
    serializeJson(lightStateDoc, lightStateBuffer);
    client->publish(kLightOutTopic, &lightStateBuffer[0]);
}

void updateLightState(PubSubClient* client, byte* payload, unsigned int length) {
    auto platform = IO_INJECT(ILightPlatform);
    bool isEnabled = platform->getPower();
    CRGB color = platform->getColor();
    uint8_t brightness = platform->getBrightness();

    deserializeJson(lightStateDoc, payload);
    bool reqIsEnabled = strcmp(lightStateDoc[kJsonStringState], kJsonStringOn) == 0;
    uint8_t reqBrightness = lightStateDoc[kJsonStringBrightness];
    auto colorProp = lightStateDoc[kJsonStringColor];
    CRGB reqColor = CRGB::Black;
    if (colorProp != nullptr) {
        reqColor = CRGB(
            colorProp[kJsonStringR],
            colorProp[kJsonStringG],
            colorProp[kJsonStringB]
        );
    }
    uint16_t reqColorTemp = lightStateDoc[kJsonStringColorTemp];

    const char* effectValue = lightStateDoc[kJsonStringEffect];
    uint8_t reqEffect = 255;
    if (effectValue != nullptr) {
        for (uint8_t i = 0; i < 3; i++) {
            if (strcmp(effectValue, effectNames[i]) == 0) {
                reqEffect = i;
                break;
            }
        }
    }

    bool hasChanges = false;
    if (reqIsEnabled != isEnabled) {
        platform->setPower(reqIsEnabled);
        hasChanges = true;
    } else if (reqBrightness != brightness && reqBrightness != 0) {
        platform->setBrightness(reqBrightness);
        hasChanges = true;
    }
    if (reqEffect != platform->getEffect() && reqEffect != 255) {
        platform->setEffect(reqEffect);
        hasChanges = true;
    }
    if (reqColor != color && reqColor != CRGB::Black) {
        platform->setColor(reqColor);
        hasChanges = true;
    } else if (reqColorTemp != 0) {
        platform->setColorTemperature(reqColorTemp);
        hasChanges = true;
    }
    if (hasChanges) {
        reportLightState(client);
    }
}

void reportLightConfig(PubSubClient* client) {
    client->publish(kLightConfigTopic, kLightConfig);
}

void registerLightTopics(MyrtQTT* server) {
    server->reportConfig(reportLightConfig)
    ->report(reportLightState, kLightReportInterval)
    ->on(kLightInTopic, updateLightState);
}
