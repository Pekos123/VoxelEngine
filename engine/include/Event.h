#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <functional>

namespace e {
    class Event {
    private:
        std::vector<std::function<void()>> listeners;

    public:
        template<typename T>
        void AddListener(T* instance, void (T::*method)())
        {
            listeners.push_back([instance, method]() {
                (instance->*method)();
            });
        }
        bool Invoke();
    };
}

#endif