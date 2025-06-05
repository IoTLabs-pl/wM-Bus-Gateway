#include "component.h"

#include "meters.h"

#include "esphome/core/log.h"

namespace esphome
{
    namespace wmbus_common
    {
        static const char *TAG = "wmbus_common";

        std::vector<std::string> sorted_driver_names()
        {
            std::vector<std::string> driver_names;
            driver_names.reserve(allDrivers().size());
            for (auto &driver : allDrivers())
                driver_names.push_back(driver->name().str());
            std::sort(driver_names.begin(), driver_names.end());
            return driver_names;
        }

        const std::vector<std::string> driver_names = std::move(sorted_driver_names());

        void WMBusCommon::dump_config()
        {
            ESP_LOGCONFIG(TAG, "wM-Bus Component:");
            ESP_LOGCONFIG(TAG, "  wmbusmeters version: %s", WMBUSMETERS_TAG);
            ESP_LOGCONFIG(TAG, "  Loaded drivers:");
            for (auto &driver : driver_names)
                ESP_LOGCONFIG(TAG, ("    - " + driver).c_str());
        }

    }
}