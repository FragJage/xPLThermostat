#include "xPLThermostat.h"

using namespace std;

int main(int argc, char* argv[])
{
    xPLThermostat xPLDev;

	Service* pService = Service::Create("xPLThermostat", "Thermostat for xPL protocol", &xPLDev);
	int res = pService->Start(argc, argv);
	Service::Destroy();
	return res;
}