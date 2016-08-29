#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/Thermostat.h"

using namespace std;

class TestThermostat : public TestClass<TestThermostat>
{
public:
    TestThermostat();
    ~TestThermostat();

    bool Constructors();
    bool GetMethods();
    bool StdThermostat();
    bool ColdThermostat();
    bool DiffThermostat();
    bool ColdDiffThermostat();
    bool SetMode();
};
