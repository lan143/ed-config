#pragma once

#include <LittleFS.h>

#include "storage.hpp"

namespace EDConfig
{
    template<class T>
    class StorageLittleFS : public Storage<T>
    {
    public:
        StorageLittleFS(std::string fileName) : _fileName(fileName) { }

        std::pair<T*, uint16_t> load()
        {
            T* data = new T();
            uint16_t checksum = 0;

            File file = LittleFS.open(_fileName.c_str(), FILE_READ);
            if (!file) {
                ESP_LOGW("storage", "file %s not found, returning default data", _fileName.c_str());
                return std::make_pair(data, checksum);
            }

            if (file.size() < sizeof(T) + sizeof(uint16_t)) {
                ESP_LOGW("storage", "file %s too small, returning default data", _fileName.c_str());
                file.close();
                return std::make_pair(data, checksum);
            }

            if (file.read(reinterpret_cast<uint8_t*>(data), sizeof(T)) != sizeof(T)) {
                ESP_LOGE("storage", "failed to read data");
                file.close();
                return std::make_pair(data, 0);
            }

            if (file.read(reinterpret_cast<uint8_t*>(&checksum), sizeof(uint16_t)) != sizeof(uint16_t)) {
                ESP_LOGE("storage", "failed to read checksum");
                file.close();
                return std::make_pair(data, 0);
            }

            file.close();

            return std::make_pair(data, checksum);
        };

        bool store(std::pair<T*, uint16_t> entity)
        {
            T* data = entity.first;
            uint16_t checksum = entity.second;

            File file = LittleFS.open("/data.tmp", "wb");
            if (!file) {
                ESP_LOGE("storage", "failed to open tmp file for write");
                return false;
            }

            if (file.write(reinterpret_cast<uint8_t*>(data), sizeof(T)) != sizeof(T)) {
                ESP_LOGE("storage", "failed to write data to file");
                file.close();
                return false;
            }

            if (file.write(reinterpret_cast<uint8_t*>(&checksum), sizeof(uint16_t)  ) != sizeof(uint16_t)) {
                ESP_LOGE("storage", "failed to write checksum to file");
                file.close();
                return false;
            }

            file.close();

            if (!LittleFS.rename("/data.tmp", _fileName.c_str())) {
                ESP_LOGE("storage", "failed to rename tmp file to %s", _fileName.c_str());
                return false;
            }

            return true;
        };

    private:
        std::string _fileName;
    };
}
