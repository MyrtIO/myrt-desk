#include "platforms.h"
#include <config.h>

DI_REGISTER_IMPLEMENTATION(
	IOWiFi,
	WiFiPlatform,
	{
		.ssid = CONFIG_WIFI_SSID,
		.password = CONFIG_WIFI_PASSWORD,
		.hostname = CONFIG_DEVICE_NAME,
		.connectDelay = CONFIG_WIFI_START_DELAY,
		.reconnectTimeout = CONFIG_WIFI_RECONNECT_TIMEOUT
	}
);

DI_REGISTER_IMPLEMENTATION(
	IOLight,
	LightPlatform,
	{
		.ledCount = CONFIG_LIGHT_LED_COUNT,
		.colorCorrection = CONFIG_LIGHT_COLOR_CORRECTION,
		.colorInitial = CONFIG_LIGHT_COLOR_INITIAL,
		.colorWarmWhite = CONFIG_LIGHT_COLOR_WARM_WHITE,
		.colorWarmWhiteMireds = CONFIG_LIGHT_COLOR_WARM_WHITE_MIREDS,
		.colorColdWhite = CONFIG_LIGHT_COLOR_COLD_WHITE,
		.colorColdWhiteMireds = CONFIG_LIGHT_COLOR_COLD_WHITE_MIREDS,
		.transitionMs = CONFIG_LIGHT_COLOR_TRANSITION_MS
	}
)

DI_REGISTER_IMPLEMENTATION(
	IOHeight,
	HeightPlatform,
	{
		.linUART = &CONFIG_BEKANT_LIN_UART,
		.linPinTX = CONFIG_PIN_LIN_TX,
		.linPinRX = CONFIG_PIN_LIN_RX,
		.bekantPinUp = CONFIG_PIN_BUTTON_UP,
		.bekantPinDown = CONFIG_PIN_BUTTON_DOWN,
		.bekantHeightMax = CONFIG_BEKANT_HEIGHT_MAX,
		.bekantHeightMin = CONFIG_BEKANT_HEIGHT_MIN,
		.bekantHeightSlope = CONFIG_BEKANT_HEIGHT_SLOPE,
		.bekantHeightBias = CONFIG_BEKANT_HEIGHT_BIAS
	}
)
