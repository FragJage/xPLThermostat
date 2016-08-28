#include <cassert>
#include <thread>
#include "xPLLib/Schemas/SchemaObject.h"
#include "Plateforms/Plateforms.h"
#include "UnitTest/UnitTest.h"
#ifdef WIN32
    #include "Thread/mingw.thread.h"
#else
    #include <thread>
#endif
#include "../src/xPLThermostat.h"
#include "ControlSockMock.h"


class TestxPLThermostat : public TestClass<TestxPLThermostat>
{
public:
    TestxPLThermostat();
    ~TestxPLThermostat();
    static void ThreadStart(xPLThermostat* pxPLDev);
    bool Start();
    bool StdConfig();
    bool SetAdvConfig();
    bool ModeAuto();
    bool ModeForceON();
    bool ModeForceOFF();
    bool GetAdvConfig();
    bool ModifyAdvConfig();
    bool Stop();
    bool ReStart();
    bool DelAdvConfig();
    bool ReStop();
    xPLThermostat xPLDev;

private:
    void SetDeviceValue(std::string device, std::string current, std::string type, std::string source);
};
