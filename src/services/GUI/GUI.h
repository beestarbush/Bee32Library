#ifndef _SERVICES_GUI_H
#define _SERVICES_GUI_H

#include <stdint.h>
#include <WString.h>
#include <array>

namespace HAL::SSD1306
{
    class Driver;
};

namespace SSL::GUI
{
    constexpr uint8_t MAX_COMPONENTS = 10;
    constexpr uint8_t NO_SCREEN = 0;
    constexpr uint8_t MAX_SCREENS = 4;

    class Component
    {
        public:
            using ComponentId = uint8_t;

            Component(ComponentId id, uint8_t x, uint8_t y);
            ~Component() = default;

            ComponentId getId() { return _id; }

            virtual void draw(HAL::SSD1306::Driver& display) = 0;

            bool getChanged() { return _changed; }

        protected:
            void setVisible(bool visible) { _visible = visible; }
            void setEnabled(bool enabled) { _enabled = enabled; }
            void setChanged(bool changed) { _changed = changed; }

            bool getVisible() { return _visible; }
            bool getEnabled() { return _enabled; }

            void setX(uint8_t x) { _x = x; }
            void setY(uint8_t y) { _y = y; }
            uint8_t getX() { return _x; };
            uint8_t getY() { return _y; };
            uint8_t getInitialX() { return _initialX; }
            uint8_t getInitialY() { return _initialY; }

        private:
            ComponentId _id;
            uint8_t _x;
            uint8_t _y;
            uint8_t _initialX;
            uint8_t _initialY;
            bool _visible;
            bool _enabled;
            bool _changed;
    };
    class Label : public Component
    {
        using Base = Component;
        public:
            Label(ComponentId id, uint8_t x, uint8_t y, String text);
            ~Label() = default;

            void setText(String text);

            void draw(HAL::SSD1306::Driver& display) override;

        private:
            String _text;
    };
    class Rectangle : public Component
    {
        using Base = Component;
        public:
            Rectangle(ComponentId id, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
            ~Rectangle() = default;

            void draw(HAL::SSD1306::Driver& display) override;

        protected:
            uint8_t getWidth() { return _width; }
            uint8_t getHeight() { return _height; }
            void setFilled(bool filled) { _filled = filled; }

        private:
            uint8_t _width;
            uint8_t _height;
            bool _filled;
    };
    class Button : public Rectangle
    {
        using Base = Rectangle;
        public:
            Button(ComponentId id, uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool active, String text);
            ~Button() = default;

            void setActive(bool active);

            void draw(HAL::SSD1306::Driver& display) override;

        private:
            String _text;
            bool _active;
    };
    class Screen
    {
        public:
            using ScreenId = uint8_t;
            friend class Service;

            Screen(ScreenId id, uint8_t width, uint8_t height);
            ~Screen() = default;

            ScreenId getId();

            void addComponent(Component* component);
            Component* getComponent(Component::ComponentId id);
            void draw(HAL::SSD1306::Driver& display);
            void handleInput();

        private:
            void setActive(bool active);
            bool getActive();

        private:
            ScreenId _id;
            uint8_t _width;
            uint8_t _height;
            Component* _components[MAX_COMPONENTS];
            uint8_t _componentCount;
            bool _active;
    };
    class Service
    {
        public:
            Service(HAL::SSD1306::Driver& display);
            ~Service() = default;

            void begin();
            void end();
            void run();

            void addScreen(Screen* screen);
            Screen* getScreen(Screen::ScreenId id);
            void show(Screen::ScreenId id);
            void hide(Screen::ScreenId id);

        private:
            HAL::SSD1306::Driver& _display;
            Screen* _screens[MAX_SCREENS];
            Screen _noScreen;
            uint8_t _currentScreen;
            uint8_t _screenCount;
    };
}

#endif