#include "utils.h"

template <typename T>
std::string enumToString(const T &value)
{
    std::string name;

    for (int i = 0; i < T::value; ++i)
    {
        if (static_cast<T>(i) == value)
        {
            name = static_cast<std::string>(static_cast<T>(i));
            break;
        }
    }

    return name;
}