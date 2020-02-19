#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

// CLK - GPIO18
#define PIN_CLK 18

// MOSI - GPIO 23
#define PIN_MOSI 23

// RESET - GPIO 2
#define PIN_RESET 2

// DC - GPIO 15
#define PIN_DC 15

// CS - GPIO 5
#define PIN_CS 5

// EXT_VCC - GPIO 4
#define PIN_EXTVCC 4

static char tag[] = "test_SH1106";

void task_test_SH1106(void *ignore) {
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.clk   = PIN_CLK;
	u8g2_esp32_hal.mosi  = PIN_MOSI;
	u8g2_esp32_hal.cs    = PIN_CS;
	u8g2_esp32_hal.dc    = PIN_DC;
	u8g2_esp32_hal.reset = PIN_RESET;
	u8g2_esp32_hal.ext_vcc = PIN_EXTVCC;
	u8g2_esp32_hal_init(u8g2_esp32_hal);


	u8g2_t u8g2; // a structure which will contain all the data for one display
	u8g2_Setup_sh1106_128x64_ext_vcc(
		&u8g2,
		U8G2_R0,
		u8g2_esp32_spi_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure

	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawBox(&u8g2, 10,20, 20, 30);
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
	u8g2_DrawStr(&u8g2, 0,15,"Hello World!");
	u8g2_SendBuffer(&u8g2);

	ESP_LOGD(tag, "All done!");

	vTaskDelete(NULL);
}
