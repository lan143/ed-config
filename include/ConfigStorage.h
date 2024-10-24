#pragma once

namespace Config
{
    template<class T>
    class ConfigStorageEntity
    {
    public:
        ConfigStorageEntity(T config, uint16_t checksum)
        {
            _config = config;
            _checksum = checksum;
        }

        T getConfig() { return _config; }
        uint16_t getCheckSum() { return _checksum; }

    private:
        T _config;
        uint16_t _checksum = 0;
    };

    template<class T>
    class ConfigStorage
    {
    public:
        virtual ConfigStorageEntity<T> load() = 0;
        virtual bool store(ConfigStorageEntity<T> entity) = 0;
    };
}
