#include <algorithm>
#include "CacheManager.h"

using namespace std;

CacheManager::CacheManager()
{
}

CacheManager::~CacheManager()
{
}

void CacheManager::RemoveThermostat(Thermostat* thermostat)
{
    map<string, CacheDevice>::iterator it;
    map<string, CacheDevice>::const_iterator itEnd;


    itEnd = m_Cache.end();
    for(it=m_Cache.begin(); it!=itEnd; ++it)
    {
        it->second.RemoveThermostat(thermostat);
        if(it->second.ThermostatCount()==0) it = m_Cache.erase(it);
    }
}

void CacheManager::Clear()
{
    map<string, CacheDevice>::iterator it;
    map<string, CacheDevice>::const_iterator itEnd;


    itEnd = m_Cache.end();
    for(it=m_Cache.begin(); it!=itEnd; ++it)
        it->second.Clear();
    m_Cache.clear();
}


vector<Thermostat*> CacheManager::UpdateDevice(string device, double value)
{
    map<string, CacheDevice>::iterator it;

    it = m_Cache.find(device);
    if(it==m_Cache.end()) return CacheDevice::NoThermostat;

    return it->second.UpdateDevice(value);
}

bool CacheManager::AddDevice(string device, Thermostat* thermostat)
{
    map<string, CacheDevice>::iterator it;

    it = m_Cache.find(device);

    if(it!=m_Cache.end())
    {
        it->second.AddThermostat(thermostat);
        return false;
    }

    m_Cache[device] = CacheDevice(thermostat);
    return true;
}

bool CacheManager::GetValue(std::string device, double* value)
{
    map<string, CacheDevice>::iterator it;

    it = m_Cache.find(device);
    if(it==m_Cache.end()) return false;

    return it->second.GetValue(value);
}
