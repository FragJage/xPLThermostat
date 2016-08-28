#ifndef FRAGCACHEMANAGER_H
#define FRAGCACHEMANAGER_H

#include<string>
#include<map>
#include "Thermostat.h"
#include "CacheDevice.h"

class CacheManager
{
    public:
        CacheManager();
        ~CacheManager();

        bool AddDevice(std::string device, Thermostat* thermostat);
        void RemoveThermostat(Thermostat* thermostat);
        void Clear();
        std::vector<Thermostat*> UpdateDevice(std::string device, double value);
        bool GetValue(std::string device, double* value);

    private:
        std::map<std::string, CacheDevice> m_Cache;
};

#endif // FRAGCACHEMANAGER_H
