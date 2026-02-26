#pragma once

#if defined(ESP32)
    #include <EEPROM.h>
#endif

#include "storage.hpp"

namespace EDConfig
{
    template<class T>
    class StorageEEPROM : public Storage<T>
    {
    public:
        StorageEEPROM(int eepromSize) : _eepromSize(eepromSize) { }

        std::pair<T*, uint16_t> load()
        {
            T* data = new T();
            uint16_t checksum = 0;

            EEPROM.begin(this->_eepromSize);
            EEPROM.get(0, *data);
            EEPROM.get(sizeof(T), checksum);
            EEPROM.end();

            return std::make_pair(data, checksum);
        };

        bool store(std::pair<T*, uint16_t> entity)
        {
            T* data = entity.first;
            uint16_t checksum = entity.second;

            EEPROM.begin(this->_eepromSize);
            EEPROM.put(0, *data);
            EEPROM.put(sizeof(T), checksum);

            bool result = EEPROM.commit();
            EEPROM.end();

            return result;
        };

    private:
        int _eepromSize;
    };
}
