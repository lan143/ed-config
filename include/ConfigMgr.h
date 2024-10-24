#pragma once

#include <functional>
#include "ConfigStorage.h"

namespace Config
{
    template<class T>
    using config_default_t = std::function<void(T&)>;

    template<class T>
    class ConfigMgr
    {
    public:
        ConfigMgr();

        bool load();
        bool store();
        void setDefault(config_default_t fn) { _defaultFn = fn; }

        T& getConfig() { return _config; }

    private:
        uint16_t calculateChecksum(T &config);

    private:
        T _config;
        ConfigStorage* _storage;
        config_default_t _defaultFn;
    };
}
