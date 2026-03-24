#include <Event.h>

bool e::Event::Invoke()
{
    for (auto& listener : m_Listeners) {
        listener();
    }
    return true;
}