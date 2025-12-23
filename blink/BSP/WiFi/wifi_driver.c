// Includes for wifi connection
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include <string.h>


// static const char *TAG = "I2S_RADIO";
// static const char *RADIO_URL = "https://wrti-live.streamguys1.com/classical-mp3"; // replace with your radio


// // WiFi connection Credentials
// // TODO: change to env variables?
// #define EXAMPLE_ESP_WIFI_SSID      ""
// #define EXAMPLE_ESP_WIFI_PASS      ""

// static void event_handler(void* arg, esp_event_base_t event_base,
//                                 int16_t event_id, void* event_data)
// {
//     if (event_base == WIFI_STA_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
//         ESP_LOGI("WiFi", "Connected to AP!");
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//         ESP_LOGI("WiFi", "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
//     }
//     // Add handling for disconnection and reconnection here if needed
// }

// void wifi_init_sta(void)
// {
//     // 1. Initialize the TCP/IP stack and event loops
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     // 2. Register our event handler
//     esp_event_handler_instance_t instance_any_id;
//     esp_event_handler_instance_t instance_got_ip;
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
//                                                         ESP_EVENT_ANY_ID,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_any_id));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
//                                                         IP_EVENT_STA_GOT_IP,
//                                                         &event_handler,
//                                                         NULL,
//                                                         &instance_got_ip));

//     // 3. Initialize Wi-Fi driver
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // 4. Configure the Wi-Fi credentials
//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = EXAMPLE_ESP_WIFI_SSID,
//             .password = EXAMPLE_ESP_WIFI_PASS,
//             /* Setting a password implies station will connect to all security modes including WEP/WPA of given SSID. */
//             /* If you want to connect to an AP whose security type is not WPA2, set bottom one to TRUE. */
//             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//         },
//     };
    
//     // 5. Set mode and start the connection process
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI("WiFi", "wifi_init_sta finished.");
//     ESP_LOGI("WiFi", "connecting to %s", EXAMPLE_ESP_WIFI_SSID);
// }