#pragma once

#include <functional>
#include <string>

#include "input/enum.h"
#include "interface/window.h"

namespace Input
{
    class Button
    {
        Enum index = None;

        bool Assign(Enum begin, Enum end)
        {
            auto window = Interface::Window::Get();
            uint64_t tick = window.Ticks();

            for (auto i = begin; i < end; i = Enum(i+1))
            {
                if (window.GetInputData(i).press == tick)
                {
                    index = i;
                    return true;
                }
            }

            return false;
        }

      public:
        Button() {}
        Button(Enum index) : index(index) {}

        [[nodiscard]] bool pressed () const {auto window = Interface::Window::Get(); return window.GetInputData(index).press   == window.Ticks();}
        [[nodiscard]] bool released() const {auto window = Interface::Window::Get(); return window.GetInputData(index).release == window.Ticks();}
        [[nodiscard]] bool repeated() const {auto window = Interface::Window::Get(); return window.GetInputData(index).repeat  == window.Ticks();}
        [[nodiscard]] bool down    () const {return Interface::Window::Get().GetInputData(index).is_down;}
        [[nodiscard]] bool up      () const {return !down();}

        // Returns true if the key is not null.
        // We use a function instead of `operator bool` because then it's too easy to forget `.pressed()` (and other similar functions) when referring to a button.
        [[nodiscard]] bool IsAssigned() const
        {
            return index != None;
        }

        [[nodiscard]] Enum Index() const
        {
            return index;
        }

        // Returns a button name which should hopefully be layout-dependent, but not input-language-dependent.
        [[nodiscard]] std::string Name() const
        {
            if (index == None)
            {
                return "None";
            }
            else if (index >= BeginKeys && index < EndKeys) // Note that `BeginKeys == None`, so we check for `None` before this.
            {
                const char *ret;

                // From looking at the code, `SDL_GetKeyFromScancode` returns 0 on failure.
                if (SDL_Keycode keycode = SDL_GetKeyFromScancode(SDL_Scancode(index)))
                    ret = SDL_GetKeyName(keycode);
                else
                    ret = SDL_GetScancodeName(SDL_Scancode(index));

                // It looks like the functions we call return an empty string rather than null on failure, but it's better to be safe.
                if (ret && *ret)
                    return ret;
                else
                    return "Unknown " + std::to_string(index);
            }
            else if (index >= BeginMouseButtons && index < EndMouseButtons)
            {
                switch (index)
                {
                  case mouse_left:
                    return "Left Mouse Button";
                  case mouse_middle:
                    return "Middle Mouse Button";
                  case mouse_right:
                    return "Right Mouse Button";
                  case mouse_x1:
                    return "X1 Mouse Button";
                  case mouse_x2:
                    return "X2 Mouse Button";
                  default:
                    return "Mouse Button " + std::to_string(index - mouse_left + 1);
                }
            }
            else
            {
                switch (index)
                {
                  case mouse_wheel_up:
                    return "Mouse Wheel Up";
                  case mouse_wheel_down:
                    return "Mouse Wheel Down";
                  case mouse_wheel_left:
                    return "Mouse Wheel Left";
                  case mouse_wheel_right:
                    return "Mouse Wheel Right";
                  default:
                    return "Invalid " + std::to_string(index);
                }
            }
        }

        // If a key is currently pressed (not down), assigns its index to this button.
        // Returns false if nothing is pressed.
        bool AssignKey()
        {
            return Assign(BeginKeys, EndKeys);
        }
        // Same but for the mouse buttons.
        bool AssignMouseButton()
        {
            return Assign(BeginMouseButtons, EndMouseButtons);
        }
        // Same but for the mouse wheel.
        bool AssignMouseWheel()
        {
            return Assign(BeginMouseWheel, EndMouseWheel);
        }
    };

    class ButtonList
    {
        std::vector<Button> buttons;
        mutable std::uint64_t cached_time = 0;
        mutable std::optional<bool> cached_pressed, cached_released, cached_repeated, cached_down;

        void UpdateCache() const
        {
            auto cur_time = Interface::Window::Get().Ticks();
            if (cur_time == cached_time)
                return;
            cached_time = cur_time;

            cached_pressed.reset();
            cached_released.reset();
            cached_repeated.reset();
            cached_down.reset();
        }

      public:
        ButtonList() {}
        ButtonList(std::vector<Button> buttons) : buttons(buttons) {}

        [[nodiscard]] const std::vector<Button> &GetButtons() const {return buttons;}
        [[nodiscard]] std::vector<Button> &ModifyButtons() {cached_time = 0; return buttons;}

        [[nodiscard]] bool pressed() const
        {
            // Return true if at least one button is pressed, and none of them are down and not pressed at the same time.
            UpdateCache();
            if (!cached_pressed)
            {
                cached_pressed = false;
                for (const Button &button : buttons)
                {
                    bool button_pressed = button.pressed();
                    if (!button_pressed && button.down())
                    {
                        *cached_pressed = false;
                        break;
                    }
                    if (button_pressed)
                        *cached_pressed = true;
                }
            }
            return *cached_pressed;
        }

        [[nodiscard]] bool released() const
        {
            // Return true if at least one button is released, and none of them are down.
            UpdateCache();
            if (!cached_released)
            {
                cached_released = false;
                for (const Button &button : buttons)
                {
                    bool button_released = button.released();
                    if (!button_released && !button.pressed() && button.down()) // This can't be just `button.down()` to properly handle regrabbing the (same or different) button on the same tick.
                    {
                        *cached_released = false;
                        break;
                    }
                    if (button_released)
                        *cached_released = true;
                }
            }
            return *cached_released;
        }

        [[nodiscard]] bool repeated() const
        {
            // Return true if any of the buttons is repeated.
            UpdateCache();
            if (!cached_repeated)
                cached_repeated = std::any_of(buttons.begin(), buttons.end(), std::mem_fn(&Button::repeated));
            return *cached_repeated;
        }

        [[nodiscard]] bool down() const
        {
            // Return true if any of the buttons is down.
            UpdateCache();
            if (!cached_down)
                cached_down = std::any_of(buttons.begin(), buttons.end(), std::mem_fn(&Button::down));
            return *cached_down;
        }

        [[nodiscard]] bool up() const
        {
            return !down();
        }
    };
}
