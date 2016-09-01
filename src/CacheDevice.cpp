#include <algorithm>
#include "CacheDevice.h"
#include "Thermostat.h"

using namespace std;

std::vector<Thermostat*> CacheDevice::NoThermostat;

CacheDevice::CacheDevice()
{
    m_Lu = false;
    m_Current = 0;
}

CacheDevice::CacheDevice(Thermostat* thermostat)
{
    m_Lu = false;
    m_Current = 0;
    m_Thermostats.push_back(thermostat);
}

CacheDevice::~CacheDevice()
{
}

void CacheDevice::swap(CacheDevice& other)
{
    std::swap(m_Lu, other.m_Lu);
    std::swap(m_Current, other.m_Current);
    std::swap(m_Thermostats, other.m_Thermostats);
}

CacheDevice::CacheDevice(CacheDevice const& other) :
    m_Lu{other.m_Lu},
    m_Current{other.m_Current},
    m_Thermostats{other.m_Thermostats}
{
}

CacheDevice& CacheDevice::operator=(CacheDevice const& other)
{
    CacheDevice{other}.swap(*this);
    return *this;
}

CacheDevice::CacheDevice(CacheDevice &&other)
{
    m_Thermostats = move(other.m_Thermostats);
    m_Lu = other.m_Lu;
    m_Current = other.m_Current;
}

CacheDevice& CacheDevice::operator=(CacheDevice&& other) noexcept
{
   CacheDevice(move(other)).swap(*this);
   return *this;
}

void CacheDevice::AddThermostat(Thermostat* thermostat)
{
    m_Thermostats.push_back(thermostat);
}

void CacheDevice::RemoveThermostat(Thermostat* thermostat)
{
    std::vector<Thermostat*>::iterator it;
    std::vector<Thermostat*>::const_iterator itEnd;

    itEnd = m_Thermostats.end();
    for(it=m_Thermostats.begin(); it!=itEnd; ++it)
    {
        if(*it!=thermostat) continue;
        m_Thermostats.erase(it);
        break;
    }
}

void CacheDevice::Clear()
{
    m_Thermostats.clear();
}

int CacheDevice::ThermostatCount()
{
    return m_Thermostats.size();
}

vector<Thermostat*> CacheDevice::UpdateDevice(double value)
{
    if(m_Current==value) return CacheDevice::NoThermostat;

    m_Current = value;
    m_Lu = true;

    return m_Thermostats;
}

bool CacheDevice::GetValue(double* value)
{
    if(!m_Lu) return false;
    *value = m_Current;
    return true;
}
