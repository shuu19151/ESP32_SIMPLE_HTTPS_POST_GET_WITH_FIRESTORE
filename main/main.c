/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_http_client.h>
#include <esp_http_server.h>
#include <esp_https_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"

/**
 * A simple example that demonstrates how to create GET handlers.
*/

__attribute__((unused)) static const char *TAG = "main";

/* SSL certification */
extern const uint8_t certificate_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t certificate_pem_end[]   asm("_binary_certificate_pem_end");

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

}

static esp_err_t client_event_handler(esp_http_client_event_handle_t event) {
    switch(event->event_id)
    {
        case HTTP_EVENT_ON_DATA:
        ESP_LOGI(__func__, "HTTP_EVENT_ON_DATA %.*s\n", event->data_len, (char *)event->data);
        break;

        default:
        break;
    }
    return ESP_OK;
}

static void Firestore_GET_Request(void) {
    esp_http_client_config_t getMethodConfig = {
        .url = "https://firestore.googleapis.com/v1/projects/testfirestore-ed7f2/databases/(default)/documents/Users/",
        .method = HTTP_METHOD_GET,
        .cert_pem = (const char *)certificate_pem_start,
        .event_handler = client_event_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&getMethodConfig);

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

static void Firestore_POST_Request(void) {
    esp_http_client_config_t PostMethodConfig = {
        .url = "https://firestore.googleapis.com/v1/projects/testfirestore-ed7f2/databases/(default)/documents/Users/",
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)certificate_pem_start,
        .event_handler = client_event_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&PostMethodConfig);

    char *postData = "{\"fields\":{\"Name\":{\"stringValue\":\"LamQuangDong\"},\"Age\":{\"integerValue\":\"60\"}}}";
    esp_http_client_set_post_field(client, postData, strlen(postData));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    if(esp_http_client_perform(client) == ESP_OK) {
        ESP_LOGI(__func__, "Post requested successfully!");
    }
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /**
     * Register event handlers to request a GET method to firebase when Wi-Fi or Ethernet is connected,
     */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    ESP_ERROR_CHECK(example_connect());
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Firestore_POST_Request();
}
