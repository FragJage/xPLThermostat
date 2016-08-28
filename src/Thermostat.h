#ifndef FRAGTHERMOSTAT_H
#define FRAGTHERMOSTAT_H

#include<string>

class Thermostat
{
    public:
        enum ThermostatModeKind {ENABLE, DISABLE, FORCEON, FORCEOFF};
        Thermostat(const std::string& name);
        Thermostat(const std::string& name, const std::string& output, double setpoint, const std::string& temperatureDevice, double hysteresis, bool cooling, const std::string& differentialDevice, double engagementGap, double disengagementGap);
        ~Thermostat();

        void SetThermostat(const std::string& output, double setpoint, const std::string& temperatureDevice, double hysteresis, bool cooling, const std::string& differentialDevice, double engagementGap, double disengagementGap);
        std::string GetName();
        std::string GetModuleName();
        std::string GetOutputName();
        std::string GetTemperatureDevice();
        std::string GetDifferentialDevice();
        void SetTemperatureValue(double value);
        void SetDifferentialValue(double value);
        void SetMode(const std::string& mode);
        bool GetState();
        bool StateChange();

    private:
        bool SetState(bool state);
        bool EvaluateHeating();
        bool EvaluateCooling();
        ThermostatModeKind ToThermostatModeKind(const std::string& mode);

        std::string m_Name;
        ThermostatModeKind m_Mode;
        std::string m_OutputDevice;
        std::string m_DeviceName;
        std::string m_ModuleName;
        double m_SetPoint;
        double m_Hysteresis;
        bool m_Cooling;
        bool m_State;
        std::string m_TemperatureDevice;
        std::string m_DifferentialDevice;
        double m_DifferentialEngagementGap;
        double m_DifferentialDisengagementGap;
        double m_TemperatureValue;
        double m_DifferentialValue;
};

#endif // FRAGTHERMOSTAT_H
