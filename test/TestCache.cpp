#include "TestCache.h"

using namespace std;

TestCache::TestCache() : TestClass("TestCache", this),
                            HeatTh("HeatTh", "fragxpl-onewire.default:househeating", 20.5, "fragxpl-onewire.default:housetemp", 0.4, false, "", 0, 0),
                            AirTh("AirTh", "fragxpl-onewire.default:airconditioner", 25, "fragxpl-onewire.default:housetemp", 0.3, true, "", 0, 0),
                            PoolTh("PoolTh", "fragxpl-onewire.default:poolheating", 28, "fragxpl-onewire.default:pooltemp", 0.5, false, "fragxpl-onewire.default:solartemp", 3, 6)
{
	addTest("Constructors", &TestCache::Constructors);
	addTest("AddDevice", &TestCache::AddDevice);
	addTest("UpdateDevice", &TestCache::UpdateDevice);
	addTest("GetValue", &TestCache::GetValue);
	addTest("RemoveThermostat", &TestCache::RemoveThermostat);
	addTest("Clear", &TestCache::Clear);
}

TestCache::~TestCache()
{
}

bool TestCache::Constructors()
{
    CacheDevice cd1;
    CacheDevice cd2;
    CacheDevice cd3(cd1);
    CacheDevice cd4 = cd2;
    CacheDevice cd5(std::move(cd1));
    CacheDevice cd6 = std::move(cd2);
    cd6.swap(cd5);
    return true;
}

bool TestCache::AddDevice()
{
    TheCache.AddDevice("fragxpl-onewire.default:housetemp", &HeatTh);
    TheCache.AddDevice("fragxpl-onewire.default:housetemp", &AirTh);
    TheCache.AddDevice("fragxpl-onewire.default:pooltemp", &PoolTh);
    TheCache.AddDevice("fragxpl-onewire.default:solartemp", &PoolTh);
    return true;
}

bool TestCache::UpdateDevice()
{
    vector<Thermostat*> ths;

    ths = TheCache.UpdateDevice("fragxpl-onewire.default:housetemp", 25.3);
    assert(2==ths.size());

    return true;
}

bool TestCache::GetValue()
{
    double value;

    if(TheCache.GetValue("fragxpl-onewire.default:pooltemp", &value)) return false;
    if(!TheCache.GetValue("fragxpl-onewire.default:housetemp", &value)) return false;
    assert(25.3==value);
    return true;
}

bool TestCache::RemoveThermostat()
{
    vector<Thermostat*> ths;

    TheCache.RemoveThermostat(&AirTh);
    ths = TheCache.UpdateDevice("fragxpl-onewire.default:housetemp", 25.5);
    assert(1==ths.size());

    return true;
}

bool TestCache::Clear()
{
    vector<Thermostat*> ths;

    TheCache.Clear();
    ths = TheCache.UpdateDevice("fragxpl-onewire.default:pooltemp", 26.5);
    assert(0==ths.size());

    return true;
}
