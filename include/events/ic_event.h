#pragma once

#include <functional>

namespace IC {
    enum class EventType {
        None = 0,
        // App events.
        AppRender,
        AppUpdate,
        // Window events.
        WindowClose,
        WindowResize,
    };

    enum EventCategory {
        EventCategoryNone = 0,
        EventCategoryApp = 1 << 0,
    };

#define EVENT_CLASS_TYPE(type)                                                                                         \
    static EventType GetStaticType() {                                                                                 \
        return EventType::type;                                                                                        \
    }                                                                                                                  \
    virtual EventType GetEventType() const override {                                                                  \
        return GetStaticType();                                                                                        \
    }

#define EVENT_CLASS_CATEGORY(category)                                                                                 \
    virtual int GetCategoryFlags() const override {                                                                    \
        return category;                                                                                               \
    }

    class Event {
    public:
        virtual ~Event() = default;

        bool handled = false;

        virtual EventType GetEventType() const = 0;
        virtual int GetCategoryFlags() const = 0;

        bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
    };

    class EventDispatcher {
    public:
        EventDispatcher(Event &event) : _event(event) {}

        template <typename T, typename C> bool Dispatch(const C &callback) {
            if (_event.GetEventType() == T::GetStaticType()) {
                _event.handled |= callback(static_cast<T &>(_event));
                return true;
            }

            return false;
        }

    private:
        Event &_event;
    };
} // namespace IC
