#pragma once

#if defined(ESP32)
    #include <EEPROM.h>
#endif

#include "storage.hpp"

namespace EDConfig
{
    template<class T>
    class ConfigStorageESP32 : public ConfigStorage<T>
    {
    public:
        ConfigStorageESP32(int eepromSize) : _eepromSize(eepromSize) { }

        std::pair<T*, uint16_t> load()
        {
            T* config = new T();
            uint16_t checksum = 0;

            EEPROM.begin(this->_eepromSize);
            EEPROM.get(0, *config);
            EEPROM.get(sizeof(T), checksum);
            EEPROM.end();

            return std::make_pair(config, checksum);
        };

        bool store(std::pair<T*, uint16_t> entity)
        {
            T* config = entity.first;
            uint16_t checksum = entity.second;

            EEPROM.begin(this->_eepromSize);
            EEPROM.put(0, *config);
            EEPROM.put(sizeof(T), checksum);

            bool result = EEPROM.commit();
            EEPROM.end();

            return result;
        };

    private:
        int _eepromSize;
    };
}
