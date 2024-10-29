#include "platforms.h"
#include <config.h>

IO_REGISTER_IMPLEMENTATION(
	IWiFiPlatform,
	WiFiPlatform,
	{
		.ssid = CONFIG_WIFI_SSID,
		.password = CONFIG_WIFI_PASSWORD,
		.hostname = CONFIG_DEVICE_NAME,
		.startDelayMs = CONFIG_WIFI_START_DELAY
	}
);

IO_REGISTER_IMPLEMENTATION(
	ILightPlatform,
	LightPlatform,
	{
		.ledCount = CONFIG_LIGHT_LED_COUNT,
		.colorCorrection = CONFIG_LIGHT_COLOR_CORRECTION,
		.colorInitial = CONFIG_LIGHT_COLOR_INITIAL,
		.colorWarmWhite = CONFIG_LIGHT_COLOR_WARM_WHITE,
		.colorWarmWhiteMireds = CONFIG_LIGHT_COLOR_WARM_WHITE_MIREDS,
		.colorColdWhite = CONFIG_LIGHT_COLOR_COLD_WHITE,
		.colorColdWhiteMireds = CONFIG_LIGHT_COLOR_COLD_WHITE_MIREDS,
		.transitionMs = CONFIG_LIGHT_TRANSITION_COLOR
	}
)

IO_REGISTER_IMPLEMENTATION(
	IHeightPlatform,
	HeightPlatform,
	{
		.linUART = &CONFIG_BEKANT_LIN_UART,
		.linPinTX = CONFIG_BEKANT_PIN_LIN_TX,
		.linPinRX = CONFIG_BEKANT_PIN_LIN_RX,
		.bekantPinUp = CONFIG_BEKANT_PIN_BUTTON_UP,
		.bekantPinDown = CONFIG_BEKANT_PIN_BUTTON_DOWN,
		.bekantHeightMax = CONFIG_BEKANT_HEIGHT_MAX,
		.bekantHeightMin = CONFIG_BEKANT_HEIGHT_MIN,
		.bekantHeightSlope = CONFIG_BEKANT_HEIGHT_SLOPE,
		.bekantHeightBias = CONFIG_BEKANT_HEIGHT_BIAS
	}
)
