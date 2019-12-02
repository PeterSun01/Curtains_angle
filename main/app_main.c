#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "Led.h"
#include "JY61.h"



void app_main(void)
{

  //ESP_ERROR_CHECK( nvs_flash_init() );
  //ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

  Led_Init();
  JY61_Init();

}
