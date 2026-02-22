#pragma once

#include <functional>
#include <esp_log.h>
#include "ConfigStorage.hpp"

#if defined(ESP32)
    #include "ConfigStorageESP32.hpp"
#endif

namespace EDConfig
{
    template<class T>
    using config_default_t = std::function<void(T*)>;

    template<class T>
    class ConfigMgr
    {
    public:
        ConfigMgr(int eepromSize)
        {
            #if defined(ESP32)
                _storage = new ConfigStorageESP32<T>(eepromSize);
            #endif
        }

        ~ConfigMgr()
        {
            delete _storage;
        }

        bool load()
        {
            ESP_LOGD("configMgr", "load from storage");
            ConfigStorageEntity<T> entity = _storage->load();
            ESP_LOGD("configMgr", "get config");
            _config = entity.getConfig();
            ESP_LOGD("configMgr", "calculate checksum in eeprom");
            uint16_t checksum = entity.getCheckSum();
            ESP_LOGD("configMgr", "calculate config checksum");
            uint16_t configChecksum = calculateChecksum(_config);

            ESP_LOGD("configMgr", "validate checksum");
            if (checksum == configChecksum) {
                return true;
            }

            ESP_LOGD("configMgr", "load defaults");
            if (_defaultFn != NULL) {
                _defaultFn(_config);
            }

            return false;
        }

        bool store()
        {
            uint16_t checksum = calculateChecksum(_config);

            return _storage->store(ConfigStorageEntity<T>(_config, checksum));
        }
        void setDefault(config_default_t<T> fn) { _defaultFn = fn; }

        T* getConfig() { return _config; }

    private:
        uint16_t calculateChecksum(T* config)
        {
            auto *buf = reinterpret_cast<const uint8_t*>(config);
            uint16_t crc = 0xffff, poly = 0xa001;
            uint16_t i = 0;
            uint16_t len = sizeof(T) - 2;

            for (i = 0; i < len; i++) {
                crc ^= buf[i];
                for (uint8_t j = 0; j < 8; j++) {
                    crc >>= 1;

                    if (crc & 0x01) {
                        crc ^= poly;
                    }
                }
            }

            return crc;
        }

    private:
        T* _config = nullptr;
        ConfigStorage<T>* _storage;
        config_default_t<T> _defaultFn;
    };
}
