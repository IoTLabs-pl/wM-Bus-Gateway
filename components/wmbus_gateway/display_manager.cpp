#include "display_manager.h"

namespace esphome
{
    namespace wmbus_gateway
    {

        const static char *TAG = "DisplayManager";

        DisplayManager::DisplayManager(ssd1306_base::SSD1306 *display, binary_sensor::BinarySensor *button)
            : display(display), button(button)
        {
        }

        uint32_t DisplayManager::get_update_interval() const
        {
            return this->current_screen->get_update_interval();
        }

        float DisplayManager::get_setup_priority() const
        {
            return this->display->get_actual_setup_priority() - 1;
        }

        void DisplayManager::draw_next()
        {
            if (this->display->is_on())
                this->change_page();
            else
                this->display->turn_on();

            this->current_screen->reinit();
            this->update();
            this->start_poller();
            this->set_timeout("display_off",
                              this->display_timeout,
                              [this]()
                              { this->display->turn_off();
                                this->stop_poller(); });
        }

        void DisplayManager::call_setup()
        {
            button->add_on_state_callback([this](bool state)
                                          { if(!state) return;
                                            this->draw_next(); });

            this->add_screen(new ConnectionInfoScreen());
            this->add_screen(new RuntimeInfoScreen());
            this->add_screen(new QRCodeScreen());

            auto ss = SplashScreen();
            ss.render(*this->display);
            this->display->display();

            this->set_timeout("display_off",
                              10'000,
                              [this]()
                              { this->draw_next(); });
        }

        void DisplayManager::update()
        {
            auto current_interval = this->get_update_interval();
            this->current_screen->render(*this->display);
            this->display->display();
            auto new_interval = this->get_update_interval();

            if (current_interval != new_interval)
                this->start_poller();
        }

#ifdef USE_WMBUS_METER_SENSOR
        void DisplayManager::add_sensor(wmbus_meter::Sensor *sensor)
        {
            auto screen = new SensorScreen(sensor);
            this->add_screen(screen);

            sensor->add_on_state_callback(
                [this, screen](float state)
                {
                    if (this->current_screen == screen)
                        this->update();
                });
        }
#endif

        void DisplayManager::add_screen(Screen *screen)
        {
            if (!this->last_screen)
                screen->next = screen;
            else
            {
                auto old_next = this->last_screen->next;
                this->last_screen->next = screen;
                screen->next = old_next;
            }

            this->last_screen = screen;
        }

        void DisplayManager::change_page()
        {
            auto current = this->current_screen ? this->current_screen : this->last_screen;

            this->current_screen = current->next;
        }

        void DisplayManager::set_display_timeout(uint32_t timeout)
        {
            this->display_timeout = timeout;
        }
    }
}
