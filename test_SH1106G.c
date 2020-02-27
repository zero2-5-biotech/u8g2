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
static u8g2_t u8g2; // a structure which will contain all the data for one display
static bool disp_init = false;
extern int AltConcPM2_5;


void checkDispInit(void) {

	if(disp_init) return;
	disp_init = true;
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.clk   = PIN_CLK;
	u8g2_esp32_hal.mosi  = PIN_MOSI;
	u8g2_esp32_hal.cs    = PIN_CS;
	u8g2_esp32_hal.dc    = PIN_DC;
	u8g2_esp32_hal.reset = PIN_RESET;
	u8g2_esp32_hal.ext_vcc = PIN_EXTVCC;
	u8g2_esp32_hal_init(u8g2_esp32_hal);


	ESP_LOGI(tag,"init");
	u8g2_Setup_sh1106_128x64_ext_vcc(
		&u8g2,
		U8G2_R0,
		u8g2_esp32_spi_byte_cb,
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure

	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this

}

void task_hello_SH1106(void *pl) {


	checkDispInit();

	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	u8g2_ClearBuffer(&u8g2);
	ESP_LOGI(tag, "Draw Frame");
	u8g2_DrawFrame(&u8g2, 0,0, 128, 64);
	ESP_LOGI(tag, "Draw string");
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
	u8g2_DrawStr(&u8g2, 5,17,"Power Level");
	switch((int)pl){
	case 0:
		u8g2_DrawStr(&u8g2, 50,34,"Off");
		break;
	case 1:
		u8g2_DrawStr(&u8g2, 30,34,"Standby");
		break;
	case 2:
		u8g2_DrawStr(&u8g2, 50,34,"Low");
		break;
	case 3:
		u8g2_DrawStr(&u8g2, 30,34,"Medium");
		break;
	case 4:
		u8g2_DrawStr(&u8g2, 40,34,"High");
		break;
	default:
		u8g2_DrawStr(&u8g2, 30,34,"Error");
		break;
	}
	u8g2_SendBuffer(&u8g2);

	vTaskDelete(NULL);
}

void task_pm_SH1106(void *ignore){
	char str[20];
	checkDispInit();
	u8g2_SetPowerSave(&u8g2, 0); // wake up display

	u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
	u8g2_DrawStr(&u8g2, 5,55,"PM");
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tn);
	u8g2_DrawStr(&u8g2, 30,60,"2.5");
	u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
	u8g2_DrawStr(&u8g2, 45,55,":");
	u8g2_SetDrawColor(&u8g2,0);
	u8g2_DrawBox(&u8g2, 52,43, 30, 15);
	u8g2_SetDrawColor(&u8g2,1);
	sprintf(str,"%d",AltConcPM2_5);
	u8g2_DrawStr(&u8g2, 52,55,str);
	u8g2_DrawStr(&u8g2, 82,55,"ug/m");
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tn);
	u8g2_DrawStr(&u8g2, 119,49,"3");

	u8g2_SendBuffer(&u8g2);

	vTaskDelete(NULL);
}

void task_sleep_SH1106(void *ignore) {

	checkDispInit();
	ESP_LOGI(tag,"sleep");
	u8g2_ClearBuffer(&u8g2);
	u8g2_SendBuffer(&u8g2);
	u8g2_SetPowerSave(&u8g2, 1); // wake up display

	vTaskDelete(NULL);
}
