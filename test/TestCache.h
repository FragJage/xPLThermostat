#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/Thermostat.h"
#include "../src/CacheManager.h"

using namespace std;

class TestCache : public TestClass<TestCache>
{
public:
    TestCache();
    ~TestCache();

    bool AddDevice();
    bool RemoveThermostat();
    bool Clear();
    bool UpdateDevice();
    bool GetValue();

    CacheManager TheCache;
    Thermostat HeatTh;
    Thermostat AirTh;
    Thermostat PoolTh;
};
