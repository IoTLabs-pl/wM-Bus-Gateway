#ifdef USE_WMBUS_METER_SENSOR

#pragma once

#include "screen.h"
#include "resources.h"

namespace esphome
{

    namespace wmbus_gateway
    {

        class SensorScreen : public Screen
        {
        public:
            SensorScreen(wmbus_meter::Sensor *sensor) : Screen{60000}, sensor{sensor}
            {
                this->sensor->add_on_state_callback([this](float state)
                                                    { this->last_update = millis(); });
            }

        protected:
            void render_fcn(display::Display &it) override
            {
                this->render_string(it, 0, 0, display::TextAlign::TOP_LEFT, resources::font_small, this->sensor->get_name().c_str());
                this->render_value(it);
                this->render_time_ago(it);
            }

            wmbus_meter::Sensor *sensor;

            uint32_t last_update = 0;

            void render_value(display::Display &it)
            {
                std::string data = NO_DATA;
                if (!std::isnan(sensor->get_state()))
                {
                    char buf[VALUE_ACCURACY_MAX_LEN];
                    value_accuracy_to_buf(buf, sensor->get_state(), sensor->get_accuracy_decimals());
                    data = buf;
                    if (data.find('.') != std::string::npos)
                        while (!data.empty())
                        {
                            switch (data.back())
                            {
                            case '0':
                                data.pop_back();
                                continue;
                            case '.':
                                data.pop_back();
                                break;
                            }
                            break;
                        }
                    data += sensor->get_unit_of_measurement_ref();
                }
                it.print(it.get_width(),
                         it.get_height() / 2,
                         resources::font_large,
                         display::TextAlign::CENTER_RIGHT,
                         data.c_str());
            }
            void render_time_ago(display::Display &it)
            {
                std::string data = NO_DATA;
                if (!std::isnan(sensor->get_state()))
                    data = std::to_string((millis() - this->last_update) / 1000 / 60) + " minutes ago";

                it.print(it.get_width(),
                         it.get_height(),
                         resources::font_small,
                         display::TextAlign::BOTTOM_RIGHT,
                         data.c_str());
            }
        };
    }
}
#endif
