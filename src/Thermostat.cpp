#include <stdexcept>
#include "StringTools/StringTools.h"
#include "Thermostat.h"

using namespace std;

Thermostat::Thermostat(const string& name)
{
    m_Name = name;
    m_Mode = Thermostat::ENABLE;
    m_State = false;
}

Thermostat::Thermostat(const string& name, const string& output, double setpoint, const string& temperatureDevice, double hysteresis, bool cooling, const string& differentialDevice, double engagementGap, double disengagementGap)
{
    m_Name = name;
    SetThermostat(output, setpoint, temperatureDevice, hysteresis, cooling, differentialDevice, engagementGap, disengagementGap);
}

Thermostat::~Thermostat()
{
}

void Thermostat::SetThermostat(const string& output, double setpoint, const string& temperatureDevice, double hysteresis, bool cooling, const string& differentialDevice, double engagementGap, double disengagementGap)
{
    size_t pos;


    m_OutputDevice = output;
    pos = m_OutputDevice.find(":");
    if(pos==string::npos) throw logic_error("The output device name must be in the form vendor-module.instance:device");
    m_ModuleName = m_OutputDevice.substr(0, pos);
    m_DeviceName = m_OutputDevice.substr(pos+1);
    m_SetPoint = setpoint;
    m_TemperatureDevice = temperatureDevice;
    m_Hysteresis = hysteresis;
    m_Cooling = cooling;
    m_DifferentialDevice = differentialDevice;
    m_DifferentialEngagementGap = engagementGap;
    m_DifferentialDisengagementGap = disengagementGap;
    m_State = false;
    m_Mode = Thermostat::ENABLE;
}

string Thermostat::GetName()
{
    return m_Name;
}

string Thermostat::GetModuleName()
{
    return m_ModuleName;
}

string Thermostat::GetOutputName()
{
    return m_DeviceName;
}

string Thermostat::GetTemperatureDevice()
{
    return m_TemperatureDevice;
}

string Thermostat::GetDifferentialDevice()
{
    return m_DifferentialDevice;
}

bool Thermostat::GetState()
{
    return m_State;
}

void Thermostat::SetTemperatureValue(double value)
{
    m_TemperatureValue = value;
}

void Thermostat::SetDifferentialValue(double value)
{
    m_DifferentialValue = value;
}

bool Thermostat::SetState(bool state)
{
    bool ret;

    ret = (state!=m_State);
    m_State = state;

    return ret;
}

bool Thermostat::StateChange()
{
    if(m_Mode==ThermostatModeKind::DISABLE) return SetState(false);
    if(m_Mode==ThermostatModeKind::FORCEOFF) return SetState(false);
    if(m_Mode==ThermostatModeKind::FORCEON) return SetState(true);
    if(m_Cooling)
        return EvaluateCooling();
    else
        return EvaluateHeating();
}

bool Thermostat::EvaluateHeating()
{
    bool bOk = true;


	//Traiter la position OFF
	if(m_TemperatureValue > m_SetPoint+m_Hysteresis) return SetState(false);

	//Traiter la position OFF (Différentiel)
	if(m_DifferentialDevice!="")
        if(m_DifferentialValue-m_TemperatureValue < m_DifferentialDisengagementGap) return SetState(false);

	//Traiter la position ON
	if(m_TemperatureValue < m_SetPoint-m_Hysteresis)
	{
		if(m_DifferentialDevice!="")		//Controle du differentiel
		{
			if(m_DifferentialValue-m_TemperatureValue > m_DifferentialEngagementGap)
				bOk = true;
			else
				bOk = false;
		}
		if(bOk) return SetState(true);
	}

	return false;
}

bool Thermostat::EvaluateCooling()
{
    bool bOk = true;


	//Traiter la position OFF
	if(m_TemperatureValue < m_SetPoint-m_Hysteresis) return SetState(false);

	//Traiter la position OFF (Différentiel)
	if(m_DifferentialDevice!="")
        if(m_TemperatureValue-m_DifferentialValue < m_DifferentialDisengagementGap) return SetState(false);

	//Traiter la position ON
	if(m_TemperatureValue > m_SetPoint+m_Hysteresis)
	{
        if(m_DifferentialDevice!="")		//Controle du differentiel
        {
			if(m_TemperatureValue-m_DifferentialValue > m_DifferentialEngagementGap)
				bOk = true;
			else
				bOk = false;
        }
		if(bOk) return SetState(true);
	}

	return false;
}

void Thermostat::SetMode(const std::string& mode)
{
    m_Mode = ToThermostatModeKind(mode);
}

Thermostat::ThermostatModeKind Thermostat::ToThermostatModeKind(const std::string& mode)
{
    if(StringTools::IsEqualCaseInsensitive(mode, "ENABLE"))
        return ThermostatModeKind::ENABLE;

    if(StringTools::IsEqualCaseInsensitive(mode, "DISABLE"))
        return ThermostatModeKind::DISABLE;

    if(StringTools::IsEqualCaseInsensitive(mode, "HIGH"))
        return ThermostatModeKind::FORCEON;

    if(StringTools::IsEqualCaseInsensitive(mode, "LOW"))
        return ThermostatModeKind::FORCEOFF;

    return ThermostatModeKind::ENABLE;
}
