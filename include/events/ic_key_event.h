#pragma once

#include <ic_keycodes.h>

#include "ic_event.h"

namespace IC {
    class KeyEvent : public Event {
    public:
        KeyCode GetKeyCode() const { return _keyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(const KeyCode keycode) : _keyCode(keycode) {}

        KeyCode _keyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(const KeyCode keycode, bool isRepeat = false) : KeyEvent(keycode), _isRepeat(isRepeat) {}

        bool IsRepeat() const { return _isRepeat; }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool _isRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

        EVENT_CLASS_TYPE(KeyReleased)
    };
} // namespace IC
