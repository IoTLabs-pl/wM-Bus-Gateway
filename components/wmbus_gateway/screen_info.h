#pragma once

#include "screen.h"
#include "resources.h"

namespace esphome
{
    namespace wmbus_gateway
    {
        class InfoScreen : public Screen
        {
        public:
            InfoScreen() : Screen{1000} {};

        protected:
            void render_fcn(display::Display &it) override
            {
                int y = 0;
                int occupied, dummy;
                auto infos = this->collect_data();

                for (const auto &info : infos)
                {
                    it.print(0, y, resources::font_small, info.first.c_str());
                    resources::font_small->measure((info.first + ' ').c_str(), &occupied, &dummy, &dummy, &dummy);
                    this->render_string(it,
                                        it.get_width(),
                                        y,
                                        display::TextAlign::TOP_RIGHT,
                                        resources::font_small,
                                        info.second.c_str(),
                                        it.get_width() - occupied);
                    y += it.get_height() / infos.size();
                }
            }

            virtual std::vector<std::pair<std::string, std::string>> collect_data() = 0;
        };
    }
}