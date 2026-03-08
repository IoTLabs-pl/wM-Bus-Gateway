#pragma once

#include "screen_info.h"

#include "esp_wifi.h"

#include "esphome/components/network/ip_address.h"

namespace esphome
{
    namespace wmbus_gateway
    {
        class ConnectionInfoScreen : public InfoScreen
        {
        protected:
            std::vector<std::pair<std::string, std::string>> collect_data() override
            {
                std::string ssid = NO_DATA, ip = NO_DATA, rssi_ch = NO_DATA;
                int8_t rssi = 0;
                uint8_t channel = 0;

                wifi_mode_t mode;
                if (esp_wifi_get_mode(&mode) == ESP_OK)
                {
                    switch (mode)
                    {
                    case WIFI_MODE_STA:
                        wifi_ap_record_t info;
                        if (esp_wifi_sta_get_ap_info(&info) == ESP_OK)
                        {
                            if (strlen(reinterpret_cast<const char *>(info.ssid)))
                                ssid = reinterpret_cast<const char *>(info.ssid);

                            rssi = info.rssi;
                            channel = info.primary;
                        }
                        break;
                    case WIFI_MODE_AP:
                    case WIFI_MODE_APSTA:
                        wifi_config_t config;
                        if (esp_wifi_get_config(WIFI_IF_AP, &config) == ESP_OK)
                        {
                            ssid = reinterpret_cast<const char *>(config.ap.ssid);

                            wifi_sta_list_t sta_list;
                            if (esp_wifi_ap_get_sta_list(&sta_list) == ESP_OK && sta_list.num > 0)
                                rssi = sta_list.sta[0].rssi;
                            channel = config.ap.channel;
                        }

                        break;
                    }

                    if (rssi)
                        rssi_ch = std::to_string(rssi) + "dBm";
                    if (channel)
                        rssi_ch += " CH" + std::to_string(channel);

                    auto *default_netif = esp_netif_get_default_netif();
                    esp_netif_ip_info_t ip_info;

                    if (default_netif && esp_netif_get_ip_info(default_netif, &ip_info) == ESP_OK)
                    {
                        char buf[network::IP_ADDRESS_BUFFER_SIZE];
                        network::IPAddress(&ip_info.ip).str_to(buf);
                        ip = buf;
                    }
                }

                return {
                    {"AP", ssid},
                    {"IP", ip},
                    {"RSSI", rssi_ch},
                    {"MAC", get_mac_address()},
                };
            }
        };
    }
}