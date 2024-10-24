#pragma once

#if defined(ESP32)
    #include <EEPROM.h>
#endif

namespace Config
{
    template<class T>
    class ConfigStorageESP32 : ConfigStorage
    {
    public:
        ConfigStorageEntity<T> load()
        {
            T config = {};
            uint16_t checksum = 0;

            EEPROM.begin(sizeof(config) + sizeof(checksum));
            EEPROM.get(0, config);
            EEPROM.get(sizeof(config), checksum);
            EEPROM.end();

            return ConfigStorageEntity<T>(config, checksum);
        };

        bool store(ConfigStorageEntity<T> entity)
        {
            T config = entity.getConfig();
            uint16_t checksum = entity.getCheckSum();

            EEPROM.begin(sizeof(config) + sizeof(checksum));
            EEPROM.put(0, config);
            EEPROM.put(sizeof(config), checksum);

            bool result = EEPROM.commit();
            EEPROM.end();

            return result;
        };
    };
}
