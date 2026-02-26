#pragma once

#include <functional>
#include <esp_log.h>
#include "storage/storage.hpp"

namespace EDConfig
{
    template<class T>
    using data_default_t = std::function<void(T*)>;

    template<class T>
    class DataMgr
    {
    public:
        DataMgr(Storage<T>* storage) : _storage(storage) { }

        ~DataMgr()
        {
            delete _storage;
        }

        bool load()
        {
            ESP_LOGD("dataMgr", "load data from storage");
            auto pair = _storage->load();
            ESP_LOGD("dataMgr", "get data");
            auto data = pair.first;
            ESP_LOGD("dataMgr", "get checksum from file");
            uint16_t checksum = pair.second;

            ESP_LOGD("dataMgr", "calculate data checksum");
            uint16_t dataChecksum = calculateChecksum(data);

            ESP_LOGD("dataMgr", "validate checksum. from file: %u, calculated: %u", checksum, dataChecksum);

            if (checksum == dataChecksum) {
                _data = data;
                return true;
            }

            _data = new T();

            ESP_LOGD("dataMgr", "load defaults");
            if (_defaultFn != NULL) {
                _defaultFn(_data);
            }

            store();

            return false;
        }

        bool store()
        {
            uint16_t checksum = calculateChecksum(_data);

            return _storage->store(std::make_pair(_data, checksum));
        }

        void setDefault(data_default_t<T> fn) { _defaultFn = fn; }

        T* getData() { return _data; }
        void setData(T* data)
        {
            memcpy(_data, data, sizeof(T));
        }

    private:
        uint16_t calculateChecksum(T* data)
        {
            auto *buf = reinterpret_cast<const uint8_t*>(data);
            uint16_t crc = 0xffff, poly = 0xa001;
            uint16_t i = 0;
            uint16_t len = sizeof(T);

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
        T* _data = nullptr;
        Storage<T>* _storage;
        data_default_t<T> _defaultFn;
    };
}
