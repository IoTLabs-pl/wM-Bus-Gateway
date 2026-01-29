#pragma once

#include "screen.h"
#include "resources.h"

namespace esphome
{
    namespace wmbus_gateway
    {
        class QRCodeScreen : public Screen
        {
        protected:
            void render_fcn(display::Display &it) override
            {
                it.print(0, 0, resources::font_small, "More");
                it.print(0, resources::font_small->get_height(), resources::font_small, "info");
                it.print(0, resources::font_small->get_height() * 2, resources::font_small, "at:");

                const uint8_t scale = 3;
                const uint8_t x_offset = 65;
                const uint8_t y_offset = 1;

                for (uint8_t x = 0; x < resources::qrcode->get_width(); x++)
                    for (uint8_t y = 0; y < resources::qrcode->get_height(); y++)
                        if (resources::qrcode->get_pixel(x, y).is_on())
                            it.filled_rectangle(x * scale + x_offset, y * scale + y_offset, scale, scale);
            }
        };
    }
}