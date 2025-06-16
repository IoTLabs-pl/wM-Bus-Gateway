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
                it.print(0, 0, font_small, "More");
                it.print(0, font_small->get_height(), font_small, "info");
                it.print(0, font_small->get_height() * 2, font_small, "at:");

                const uint8_t scale = 3;
                const uint8_t x_offset = 65;
                const uint8_t y_offset = 1;

                for (uint8_t x = 0; x < qr_code->get_width(); x++)
                    for (uint8_t y = 0; y < qr_code->get_height(); y++)
                        if (qr_code->get_pixel(x, y).is_on())
                            it.filled_rectangle(x * scale + x_offset, y * scale + y_offset, scale, scale);
            }
        };
    }
}