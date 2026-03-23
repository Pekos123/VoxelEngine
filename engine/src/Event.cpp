#include <Event.h>

bool e::Event::Invoke()
{
    for (auto& listener : listeners) {
        listener();
    }
    return true;
}