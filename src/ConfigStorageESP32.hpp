#pragma once

#if defined(ESP32)
    #include <EEPROM.h>
#endif

namespace EDConfig
{
    template<class T>
    class ConfigStorageESP32 : public ConfigStorage<T>
    {
    public:
        ConfigStorageESP32(int eepromSize) : ConfigStorage<T>(eepromSize) { }

        ConfigStorageEntity<T> load()
        {
            T* config = new T;
            uint16_t checksum = 0;

            EEPROM.begin(this->_eepromSize);
            EEPROM.get(0, *config);
            EEPROM.get(sizeof(T), checksum);
            EEPROM.end();

            return ConfigStorageEntity<T>(config, checksum);
        };

        bool store(ConfigStorageEntity<T> entity)
        {
            T* config = entity.getConfig();
            uint16_t checksum = entity.getCheckSum();

            EEPROM.begin(this->_eepromSize);
            EEPROM.put(0, *config);
            EEPROM.put(sizeof(T), checksum);

            bool result = EEPROM.commit();
            EEPROM.end();

            return result;
        };
    };
}
