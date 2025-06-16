#pragma once

#include "screen.h"
#include "resources.h"

namespace esphome
{

    namespace wmbus_gateway
    {

        class SplashScreen : public Screen
        {
        protected:
            void render_fcn(display::Display &it) override
            {
                it.print(it.get_width() / 2, 0, font_small, display::TextAlign::TOP_CENTER, std::strchr(ESPHOME_PROJECT_NAME, '.') + 1);
                it.image(it.get_width() / 2, it.get_height() / 2, logo, display::ImageAlign::CENTER);
                #ifndef ESPHOME_PROJECT_VARIANT
                #define ESPHOME_PROJECT_VARIANT "esph"
                #endif
                it.print(it.get_width() / 2, it.get_height(), font_small, display::TextAlign::BOTTOM_CENTER, ESPHOME_PROJECT_VARIANT "-v" ESPHOME_PROJECT_VERSION);
            }
        };
    }
}