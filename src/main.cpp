#include "xPLThermostat.h"

using namespace std;

int main(int argc, char* argv[])
{
    int res = 0;

    try
    {
        xPLThermostat xPLDev;

        Service* pService = Service::Create("xPLThermostat", "Thermostat for xPL protocol", &xPLDev);
        res = pService->Start(argc, argv);
        Service::Destroy();
    }
    catch(const exception &e)
    {
        std::cout << e.what();
    }
    return res;
}
