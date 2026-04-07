#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/ssd1306_base/ssd1306_base.h"

#include "screen.h"
#include "screen_splash.h"
#include "screen_info_connection.h"
#include "screen_info_runtime.h"
#include "screen_qrcode.h"

#ifdef USE_WMBUS_METER_SENSOR
#include "esphome/components/wmbus_meter/sensor.h"
#include "screen_sensor.h"
#endif

namespace esphome
{
    namespace wmbus_gateway
    {

        class DisplayManager : public PollingComponent
        {
        public:
            DisplayManager(ssd1306_base::SSD1306 *display, binary_sensor::BinarySensor *button);

            void call_setup() override;
            void update() override;
            float get_setup_priority() const override;
            uint32_t get_update_interval() const override;
            void set_display_timeout(uint32_t timeout);

#ifdef USE_WMBUS_METER_SENSOR
        public:
            void add_sensor(wmbus_meter::Sensor *sensor);
#endif

        private:
            Screen *current_screen = nullptr;
            Screen *last_screen = nullptr;
            void add_screen(Screen *screen);

            ssd1306_base::SSD1306 *display;
            binary_sensor::BinarySensor *button;
            
            uint32_t display_timeout;

            void change_page();
            void draw_next();
        };
    }
}
