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
                this->render_string(it, 0, 0, display::TextAlign::TOP_LEFT, font_small, this->sensor->get_name().c_str());
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
                    data = std::to_string(sensor->get_state());
                    size_t pos = data.find('.');
                    if (pos != std::string::npos)
                    {
                        if (sensor->get_accuracy_decimals() > 0)
                            pos += 1 + sensor->get_accuracy_decimals();
                        data = str_truncate(data, std::min(pos, 11 - sensor->get_unit_of_measurement_ref().size()));
                    }
                    data += sensor->get_unit_of_measurement_ref();
                }
                it.print(it.get_width(),
                         it.get_height() / 2,
                         font_large,
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
                         font_small,
                         display::TextAlign::BOTTOM_RIGHT,
                         data.c_str());
            }
        };
    }
}
#endif
