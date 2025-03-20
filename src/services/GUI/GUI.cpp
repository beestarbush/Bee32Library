#include "GUI.h"
#include "Arduino.h"
#include <hal/SSD1306/SSD1306.h>

namespace SSL::GUI
{
    Component::Component(ComponentId id, uint8_t x, uint8_t y) :
        _id(id),
        _x(x),
        _y(y),
        _initialX(x),
        _initialY(y),
        _visible(true),
        _enabled(true),
        _changed(false)
    {
    }

    Label::Label(ComponentId id, uint8_t x, uint8_t y, String text) :
        Base(id, x, y),
        _text(text)
    {
        setChanged(true);
    }

    void Label::setText(String text)
    {
        if (_text == text)
        {
            return;
        }

        _text = text;
        setChanged(true);
    }

    void Label::draw(HAL::SSD1306::Driver& display)
    {
        if (getVisible() == false ||
            getEnabled() == false)
        {
            return;
        }

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(getX(), getY());
        display.println(_text);
    }

    Rectangle::Rectangle(ComponentId id, uint8_t x, uint8_t y, uint8_t width, uint8_t height) :
        Base(id, x, y),
        _width(width),
        _height(height)
    {
    }

    void Rectangle::draw(HAL::SSD1306::Driver& display)
    {
        if (getVisible() == false ||
            getEnabled() == false)
        {
            return;
        }

        if (_filled == true)
        {
            display.fillRoundRect(getX(), getY(), _width, _height, 2, WHITE);
        }
        else
        {
            display.drawRoundRect(getX(), getY(), _width, _height, 2, WHITE);
        }
    }

    Button::Button(ComponentId id, uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool active, String text) :
        Base(id, x, y, width, height),
        _text(text),
        _active(active)
    {
    }

    void Button::setActive(bool active)
    {
        if (_active == active)
        {
            return;
        }

        _active = active;
        setVisible(true);
        setEnabled(true);
        setChanged(true);
    }

    void Button::draw(HAL::SSD1306::Driver& display)
    {
        if (getVisible() == false ||
            getEnabled() == false)
        {
            return;
        }

        if (!_active)
        {
            Base::setY(Base::getInitialY() + 17);
        }
        else
        {
            Base::setY(Base::getInitialY());
        }

        Base::setFilled(_active);
        Base::draw(display);

        if (_active)
        {
            // Determine the size of the text.
            int16_t x = 0;
            int16_t y = 0;
            uint16_t w = 0;
            uint16_t h = 0;
            display.setTextSize(2);
            display.getTextBounds(_text, getX(), getY(), &x, &y, &w, &h);
        
            // Center the text
            uint16_t textX = getX() + ((static_cast<float>(getWidth() - w) / 2.0) + 0.5);
            uint16_t textY = getY() + ((static_cast<float>(getHeight() - h) / 2.0) + 0.5);
            display.setCursor(textX, textY);
    
            // Set the text color
            display.setTextColor(BLACK);

            display.println(_text);
        }
    }

    Screen::Screen(ScreenId id, uint8_t width, uint8_t height) :
        _id(id),
        _width(width),
        _height(height),
        _components{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
        _componentCount(0),
        _active(true)
    {
    }

    Screen::ScreenId Screen::getId()
    {
        return _id;
    }

    void Screen::setActive(bool active)
    {
        _active = active;
    }

    bool Screen::getActive()
    {
        return _active;
    }

    void Screen::addComponent(Component* component)
    {
        assert(_componentCount < MAX_COMPONENTS);
        assert(component != nullptr);

        auto id = component->getId();
        assert(id < MAX_COMPONENTS);

        _components[id] = component;
        _componentCount++;
    }

    Component* Screen::getComponent(Component::ComponentId id)
    {
        assert(id < MAX_COMPONENTS);
        assert(_components[id] != nullptr);

        return _components[id];
    }

    void Screen::draw(HAL::SSD1306::Driver& display)
    {
        if (_active == false)
        {
            return;
        }

        bool componentsChanged = false;
        for (auto component : _components)
        {
            if (component == nullptr)
            {
                // There can be empty components, since the array is allocated with MAX_COMPONENTS.
                continue;
            }
            componentsChanged |= component->getChanged();
        }

        if (componentsChanged == false)
        {
            return;
        }

        display.clearDisplay();
        for (uint8_t i = 0; i < MAX_COMPONENTS; i++)
        {
            if (_components[i] != nullptr)
            {
                _components[i]->draw(display);
            }
        }
        display.display();
    }

    void Screen::handleInput()
    {
    }

    Service::Service(HAL::SSD1306::Driver& display) :
        _display(display),
        _screens{nullptr},
        _noScreen(NO_SCREEN, 0, 0),
        _currentScreen(0),
        _screenCount(0)
    {   
        addScreen(&_noScreen);
    }

    void Service::begin()
    {
        show(NO_SCREEN);
    }

    void Service::end()
    {
    }

    void Service::run()
    {
        for (auto screen : _screens)
        {
            if (screen == nullptr)
            {
                // There can be empty screens, since the array is allocated with MAX_SCREENS.
                continue;
            }
            if (screen->getActive())
            {
                screen->draw(_display);
            }
        }
    }

    void Service::addScreen(Screen* screen)
    {
        assert(_screenCount < MAX_SCREENS);
        assert(screen != nullptr);

        _screens[screen->getId()] = screen;
        _screenCount++;
    }

    Screen* Service::getScreen(Screen::ScreenId id)
    {
        assert(id < MAX_SCREENS);
        assert(_screens[id] != nullptr);

        return _screens[id];
    }

    void Service::show(Screen::ScreenId id)
    {
        assert(id < MAX_SCREENS);
        assert(_screens[id] != nullptr);

        _screens[_currentScreen]->setActive(false);
        _screens[id]->setActive(true);
        _currentScreen = id;
    }

    void Service::hide(Screen::ScreenId id)
    {
        assert(id < MAX_SCREENS);
        assert(_screens[id] != nullptr);

        _screens[id]->setActive(false);
        _screens[NO_SCREEN]->setActive(true);
        _currentScreen = NO_SCREEN;
    }
}