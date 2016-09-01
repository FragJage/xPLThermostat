#ifndef FRAGCACHEDEVICE_H
#define FRAGCACHEDEVICE_H

#include<string>
#include<vector>
#include "Thermostat.h"

class CacheDevice
{
    public:
        CacheDevice();
        CacheDevice(Thermostat* thermostat);
        ~CacheDevice();

        void swap(CacheDevice& other);
        CacheDevice(CacheDevice const& other);
        CacheDevice& operator=(CacheDevice const& other);
        CacheDevice(CacheDevice &&other);
        CacheDevice& operator=(CacheDevice&& other) noexcept;

        std::vector<Thermostat*> UpdateDevice(double value);
        void AddThermostat(Thermostat* thermostat);
        void RemoveThermostat(Thermostat* thermostat);
        void Clear();
        int ThermostatCount();
        bool GetValue(double* value);

        static std::vector<Thermostat*> NoThermostat;

    private:
        bool m_Lu;
        double m_Current;
        std::vector<Thermostat*> m_Thermostats;
};

#endif // FRAGCACHEDEVICE_H
