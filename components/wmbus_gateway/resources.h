#pragma once

#include "esphome/components/image/image.h"
#include "esphome/components/font/font.h"

namespace esphome
{
    namespace wmbus_gateway
    {
        namespace resources
        {
            extern esphome::image::Image *logo;
            extern esphome::image::Image *qrcode;
            extern esphome::font::Font *font_small;
            extern esphome::font::Font *font_large;
        }
    }
}
