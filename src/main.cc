#include <MyrtIO.h>
#include <WiFi.h>
#include "config.h"
#include "controllers/controllers.h"
#include "platform/platform.h"
#include <task.h>
#include <FreeRTOS.h>

IODevice desk;
LightPlatform *light;

IOLogger mainLog("Main", &Serial);

TaskHandle_t lightTaskHandle;

void lightTask() {
	light->setup();
	while (true) {
		light->loop();
	}
}

void setup() {
	Serial.begin();
#ifdef IO_DEBUG
	while (!Serial) {
		// Wait for serial port to connect
	}
#endif
  	int temp = xTaskCreatePinnedToCore(
      lightTask,   /* Function to implement the task */
      "coreTask", /* Name of the task */
      1000,       /* Stack size in words */
      NULL,       /* Task input parameter */
      0,          /* Priority of the task */
      &lightTaskHandle,       /* Task handle. */
      1);  /* Core where the task should run */

	mainLog.print("starting desk...");
	auto height = IO_INJECT_INSTANCE(HeightPlatform);
	auto wifi = IO_INJECT_INSTANCE(WiFiPlatform);
	desk.setup()
	    ->platform(height, wifi)
	    ->controllers(&ConnectionController, &MQTTController, &OTAController);
	mainLog.print("desk is initialized");
}

void setup1() {
	light = IO_INJECT_INSTANCE(LightPlatform);
	light->setup();
}

void loop() {
	desk.loop();
}

void loop1() {
	light->loop();
}
