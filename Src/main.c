#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

void task1(void *pv)
{
    while (1)
    {
        printf("Task 1 running every 500ms\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void task2(void *pv)
{
    while (1)
    {
        printf("Task 2 running every 1000ms\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xTaskCreate(task1, "Task1", 2048, NULL, 1, NULL);
    xTaskCreate(task2, "Task2", 2048, NULL, 1, NULL);
}
