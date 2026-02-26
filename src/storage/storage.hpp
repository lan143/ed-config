#pragma once

#include <utility>

namespace EDConfig
{
    template<class T>
    class Storage
    {
    public:
        Storage() {}

        virtual std::pair<T*, uint16_t> load() = 0;
        virtual bool store(std::pair<T*, uint16_t> entity) = 0;
    };
}
