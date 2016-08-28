#include "TestThermostat.h"

using namespace std;

TestThermostat::TestThermostat() : TestClass("Thermostat", this)
{
	addTest("Constructors", &TestThermostat::Constructors);
	addTest("GetMethods", &TestThermostat::GetMethods);
	addTest("Standard Thermostat", &TestThermostat::StdThermostat);
	addTest("Cooling Thermostat", &TestThermostat::ColdThermostat);
	addTest("Differential Thermostat", &TestThermostat::DiffThermostat);
	addTest("SetMode", &TestThermostat::SetMode);
}

TestThermostat::~TestThermostat()
{
}

bool TestThermostat::Constructors()
{
    Thermostat th1("MyFirstTh");
    Thermostat th2("PoolTh", "fragxpl-onewire.default:poolheating", 28.5, "fragxpl-onewire.default:pooltemp", 0.4, false, "", 0, 0);
    return true;
}

bool TestThermostat::GetMethods()
{
    Thermostat th("PoolTh", "fragxpl-onewire.default:poolheating", 28.5, "fragxpl-onewire.default:pooltemp", 0.4, true, "fragxpl-onewire.default:solartemp", 2, 5);

    assert("PoolTh"==th.GetName());
    assert("fragxpl-onewire.default"==th.GetModuleName());
    assert("poolheating"==th.GetOutputName());
    assert("fragxpl-onewire.default:pooltemp"==th.GetTemperatureDevice());
    assert("fragxpl-onewire.default:solartemp"==th.GetDifferentialDevice());
    return true;
}

bool TestThermostat::StdThermostat()
{
    Thermostat th("HeatTh", "fragxpl-onewire.default:househeating", 20.5, "fragxpl-onewire.default:housetemp", 0.4, false, "", 0, 0);

    assert(false==th.GetState());
    th.SetTemperatureValue(18);         //Mise en marche 18 < 20.5-0.4
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetTemperatureValue(20.85);      //Pas d'arret 20.85 < 20.5+0.4
    assert(false==th.StateChange());
    assert(true==th.GetState());

    th.SetTemperatureValue(20.95);      //Arret 20.95 > 20.5+0.4
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetTemperatureValue(20.15);      //Pas de mise en marche 20.15 > 20.5-0.4
    assert(false==th.StateChange());
    assert(false==th.GetState());

    th.SetTemperatureValue(20.05);      //Mise en marche 20.05 < 20.5-0.4
    assert(true==th.StateChange());
    assert(true==th.GetState());

    return true;
}

bool TestThermostat::ColdThermostat()
{
    Thermostat th("AirTh", "fragxpl-onewire.default:airconditioner", 25, "fragxpl-onewire.default:housetemp", 0.3, true, "", 0, 0);

    assert(false==th.GetState());
    th.SetTemperatureValue(26);         //Mise en marche 26 > 25+0.3
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetTemperatureValue(24.8);       //Pas d'arret 24.8 > 25-0.3
    assert(false==th.StateChange());
    assert(true==th.GetState());

    th.SetTemperatureValue(24.6);       //Arret, 24.6 < 25-0.3
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetTemperatureValue(25.2);      //Pas de mise en marche 25.2 < 25+0.3
    assert(false==th.StateChange());
    assert(false==th.GetState());

    th.SetTemperatureValue(25.4);      //Mise en marche 25.4 > 25+0.3
    assert(true==th.StateChange());
    assert(true==th.GetState());

    return true;
}

bool TestThermostat::DiffThermostat()
{
    Thermostat th("PoolTh", "fragxpl-onewire.default:poolheating", 28, "fragxpl-onewire.default:pooltemp", 0.5, false, "fragxpl-onewire.default:solartemp", 3, 6);

    assert(false==th.GetState());
    th.SetTemperatureValue(25);         //Mise en marche
    th.SetDifferentialValue(65);
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetTemperatureValue(29);         //Arret -> température atteinte
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetTemperatureValue(26);         //Mise en marche -> température trop basse
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetDifferentialValue(27);        //Pas assez de soleil
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetDifferentialValue(60);        //Le nuage est passé
    assert(true==th.StateChange());
    assert(true==th.GetState());

    return true;
}

bool TestThermostat::SetMode()
{
    Thermostat th("PoolTh", "fragxpl-onewire.default:poolheating", 28, "fragxpl-onewire.default:pooltemp", 0.5, false, "fragxpl-onewire.default:solartemp", 3, 6);

    th.SetTemperatureValue(25);         //Mise en marche
    th.SetDifferentialValue(65);
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetMode("DISABLE");
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetMode("HIGH");
    assert(true==th.StateChange());
    assert(true==th.GetState());

    th.SetMode("LOW");
    assert(true==th.StateChange());
    assert(false==th.GetState());

    th.SetMode("ENABLE");
    assert(true==th.StateChange());
    assert(true==th.GetState());

    return true;
}
