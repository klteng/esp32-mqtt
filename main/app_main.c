#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "soc/rtc_cntl_reg.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "mqtt.h"
#include "os.h"

static const char* WiFiTAG = "WIFI";
static const char* MqttTAG = "MQTT";

void connected_cb(void *self, void *params)
{
    mqtt_client *client = (mqtt_client *)self;
    mqtt_subscribe(client, "/test", 0);       
    ESP_LOGI(MqttTAG, "Connected to MQTT broker sucessful!");
}
void disconnected_cb(void *self, void *params)
{

}
void reconnect_cb(void *self, void *params)
{

}
void subscribe_cb(void *self, void *params)
{
    ets_printf("subscribe_cb!!!!!!\n");
    ESP_LOGI(MqttTAG, "[APP] Subscribe ok, test publish msg");
    mqtt_client *client = (mqtt_client *)self;
    mqtt_publish(client, "/test", "abcde", 5, 0, 0);
}

void publish_cb(void *self, void *params)
{
    ets_printf("publish_cb!!!!!!\n");
}

void data_cb(void *self, void *params)
{
    mqtt_client *client = (mqtt_client *)self;
    mqtt_event_data_t *event_data = (mqtt_event_data_t *)params;

    if (event_data->data_offset == 0) {

        char *topic = malloc(event_data->topic_length + 1);
        memcpy(topic, event_data->topic, event_data->topic_length);
        topic[event_data->topic_length] = 0;
        printf("Publish topic: %s\n", topic);
        free(topic);
    }

    printf("Publish data[%d/%d bytes]\n",
           event_data->data_length + event_data->data_offset,
           event_data->data_total_length);
}

// MQTT settings for mosquitto test server 
mqtt_settings settings = {
    .host = "test.mosquitto.org",
    .port = 1883, // unencrypted
    .client_id = "mqtt_client",
    .clean_session = 0,
    .keepalive = 120,
    .lwt_topic = "/lwt",
    .lwt_msg = "offline",
    .lwt_qos = 0,
    .lwt_retain = 0,
    .connected_cb = connected_cb,
    .disconnected_cb = disconnected_cb,
    .reconnect_cb = reconnect_cb,
    .subscribe_cb = subscribe_cb,
    .publish_cb = publish_cb,
    .data_cb = data_cb
};


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(WiFiTAG, "Connecting to AP...");
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(WiFiTAG, "Connected.");
        mqtt_start(&settings);
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(WiFiTAG, "Wifi disconnected, try to connect ...");
        mqtt_stop();
        esp_wifi_connect();
        break;

    default:
        break;
    }

    return ESP_OK;
}

void esp32_wifi_init(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "your SSID",
            .password = "your PASSWORD",
            .bssid_set = false
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    ESP_LOGI(WiFiTAG, "Free heap size: %d bytes\n", esp_get_free_heap_size());
    esp32_wifi_init();
}