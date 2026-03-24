#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <functional>

namespace e {
    class Event {
    private:
        std::vector<std::function<void()>> m_Listeners;

    public:
        template<typename T>
        void AddListener(T* instance, void (T::*method)())
        {
            m_Listeners.push_back([instance, method]() {
                (instance->*method)();
            });
        }
        bool Invoke();
    };
}

#endif