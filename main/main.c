#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "cron.h"
#include "esp_sntp.h"
#include "bridge.h"
#include "esp_log.h"
#include "time.h"


#define GPIO_INPUT 16
#define GPIO_OUTPUT 18

void print_time(time_t t) {
    char strftime_buf[64];
    struct tm timeinfo;
    localtime_r(&t,&timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The date/time in Paris is: %s", strftime_buf);
}

void check_time() {
    int status = sntp_get_sync_status();
    ESP_LOGI(TAG,"SNTP sync status: %i",status);
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    time(&now);
    localtime_r(&now,&timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Paris is: %s", strftime_buf);
}

void manage_switcher(cron_job *job) {
    check_time();
    int duration = 900000; // un quart d'heure
    ESP_LOGI(TAG,"Opening gpio...");
    gpio_set_level(GPIO_OUTPUT,true);
    vTaskDelay(duration/ portTICK_PERIOD_MS);
    gpio_set_level(GPIO_OUTPUT,false);
    ESP_LOGI(TAG,"Closing gpio...");
    ESP_LOGI(TAG,"Next execution is: ");
    print_time(job->next_execution);
    return;
}


void app_main(void)
{
    init();
    // TODO si pas de connexion,
    // donner des petites doses toutes les dix minutes?
    
    //gpio_pad_select_gpio(GPIO_INPUT);
    //gpio_set_direction(GPIO_INPUT,GPIO_MODE_INPUT);
    // INIT
    // // gpio
    ESP_LOGI(TAG,"Setting gpio...");
    gpio_pad_select_gpio(GPIO_OUTPUT);
    gpio_set_direction(GPIO_OUTPUT,GPIO_MODE_OUTPUT);
    // // time
    ESP_LOGI(TAG,"Setting clock...");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
        ESP_LOGI(TAG, "Waiting for system time to be set...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Setting time zone...");
    setenv("TZ","UTC-2",1);
    tzset();
    check_time();

    // // cron
    ESP_LOGI(TAG,"Setting cron job...");
    cron_job * jobs[2];
    jobs[0] = cron_job_create("0 0 21 * * *",manage_switcher, (void*)0);
    jobs[1] = cron_job_create("0 0 8 * * *",manage_switcher,(void*)0);

    ESP_LOGI(TAG,"Starting cron job...");
    cron_start();


    
}

