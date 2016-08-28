#include <exception>
#include "UnitTest/UnitTest.h"
#include "TestCache.h"
#include "TestThermostat.h"
#include "TestxPLThermostat.h"

using namespace std;

int main()
{
    UnitTest unitTest;
    int ret = 0;


    try
    {
        unitTest.addTestClass(new TestCache());
        unitTest.addTestClass(new TestThermostat());
        unitTest.addTestClass(new TestxPLThermostat());
    }
    catch(const exception &e)
    {
        unitTest.displayError(e.what());
        ret = -1;
    }

    if(ret!=-1)
        if(!unitTest.run()) ret = 1;

	return ret;
}
