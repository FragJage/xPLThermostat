#include "xPLThermostat.h"
#include "xPLLib/Schemas/SchemaSensor.h"
#include "xPLLib/Schemas/SchemaControl.h"
#include "Plateforms/Plateforms.h"

using namespace std;

xPLThermostat::xPLThermostat() : m_AdvanceConfig(&m_xPLDevice)
{
    m_Log = m_xPLDevice.GetLogHandle();

    m_AdvanceConfig.AddFormat("configname", xPL::AdvanceConfig::ParamType::STRING, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("output", xPL::AdvanceConfig::ParamType::DEVICE, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("setpoint", xPL::AdvanceConfig::ParamType::FLOAT, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("temperaturedevice", xPL::AdvanceConfig::ParamType::DEVICE, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("hysteresis", xPL::AdvanceConfig::ParamType::FLOAT, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("cooling", xPL::AdvanceConfig::ParamType::BOOLEAN, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("differentialdevice", xPL::AdvanceConfig::ParamType::DEVICE, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("differentialengagementgap", xPL::AdvanceConfig::ParamType::FLOAT, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("differentialdisengagementgap", xPL::AdvanceConfig::ParamType::FLOAT, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.SetCallBack(this);
    m_xPLDevice.AddExtension(&m_AdvanceConfig);
    m_xPLDevice.AddExtension(this);

    m_xPLDevice.Initialisation("fragxpl", "thermost", "default");
    m_xPLDevice.SetAppName("xPL Thermostat", "1.1.0.0");
    m_xPLDevice.SetAnswerAllMsg(true);
    m_bServicePause = false;
    m_bServiceStop = false;
}

xPLThermostat::~xPLThermostat()
{
    ConfigsDelete();
}

void xPLThermostat::RequestValue(const string& longDevice)
{
    xPL::SchemaSensorRequest sensorRequest;
    string module;
    string shortDevice;
    size_t pos;


    pos = longDevice.find(":");
    module = longDevice.substr(0, pos);
    shortDevice = longDevice.substr(pos+1);
    sensorRequest.SetDeviceName(shortDevice);
    m_xPLDevice.SendMessage(&sensorRequest, module);
}

void xPLThermostat::SendValue(Thermostat* thermostat)
{
    xPL::SchemaControlBasic sendCmnd;
    string moduleName;
    string deviceName;
    string value;


  	LOG_ENTER;

    moduleName = thermostat->GetModuleName();
    deviceName = thermostat->GetOutputName();
    if(thermostat->GetState())
        value = "high";
    else
        value = "low";

    sendCmnd.SetDeviceType("output");
    sendCmnd.SetDeviceName(deviceName);
    sendCmnd.SetCurrent(value);

    LOG_VERBOSE(m_Log) << "Send " << value << " to " << moduleName << " device " << deviceName;
    m_xPLDevice.SendMessage(&sendCmnd, moduleName);

   	LOG_EXIT_OK;
}

void xPLThermostat::ConfigsDelete()
{
    m_Thermostats.clear();
    m_Cache.Clear();
}

void xPLThermostat::ConfigDelete(const std::string& configName)
{
    list<Thermostat>::iterator it;

    for(it=m_Thermostats.begin(); it!=m_Thermostats.end(); ++it)
    {
        if(it->GetName()==configName)
        {
            m_Cache.RemoveThermostat(&(*it));
            m_Thermostats.erase(it);
            delete(&(*it));
            return;
        }
    }
}

void xPLThermostat::ConfigChange(const std::string& configName)
{
    list<Thermostat>::iterator it;
    map<string, string>* config;
    string name;
    string output;
    double setpoint;
    string tempDevice;
    double hysteresis;
    bool cooling;
    string differentialDevice;
    double engagementGap;
    double disengagementGap;


    config = m_AdvanceConfig.GetConfig(configName);
    name    = (*config)["configname"];
    output  = (*config)["output"];
    setpoint  = atof((*config)["setpoint"].c_str());
    tempDevice= (*config)["temperaturedevice"];
    hysteresis= atof((*config)["hysteresis"].c_str());
    cooling   = atoi((*config)["cooling"].c_str());
    differentialDevice = (*config)["differentialdevice"];
    engagementGap      = atof((*config)["differentialengagementgap"].c_str());
    disengagementGap   = atof((*config)["differentialdisengagementgap"].c_str());

    for(it=m_Thermostats.begin(); it!=m_Thermostats.end(); ++it)
    {
        if(it->GetName()==configName)
        {
            m_Cache.RemoveThermostat(&(*it));
            it->SetThermostat(output, setpoint, tempDevice, hysteresis, cooling, differentialDevice, engagementGap, disengagementGap);
            AddThermostatToCache(&(*it));
            return;
        }
    }

    try
    {
        m_Thermostats.emplace_back(name, output, setpoint, tempDevice, hysteresis, cooling, differentialDevice, engagementGap, disengagementGap);
    }
    catch(string e)
    {
        LOG_WARNING(m_Log) << "Bad thermostat format (" << e << ")";
    }

    it=m_Thermostats.end();
    --it;
    AddThermostatToCache(&(*it));
}

void xPLThermostat::AdvanceConfigure()
{
    list<Thermostat>::iterator it;
    int i, nb;

    LOG_ENTER;

    ConfigsDelete();

    nb = m_AdvanceConfig.GetNbConfig();
    for(i=0; i<nb; i++)
    {
        std::map<std::string, std::string>* config;
        string name;
        string output;
        double setpoint;
        string tempDevice;
        double hysteresis;
        bool cooling;
        string differentialDevice;
        double engagementGap;
        double disengagementGap;


        config = m_AdvanceConfig.GetConfig(i);

        name    = (*config)["configname"];
        output  = (*config)["output"];
        setpoint  = atof((*config)["setpoint"].c_str());
        tempDevice= (*config)["temperaturedevice"];
        hysteresis= atof((*config)["hysteresis"].c_str());
        cooling = atoi((*config)["cooling"].c_str());
        differentialDevice = (*config)["differentialdevice"];
        engagementGap      = atof((*config)["differentialengagementgap"].c_str());
        disengagementGap   = atof((*config)["differentialdisengagementgap"].c_str());

        LOG_VERBOSE(m_Log) << "Create " << name <<" : "<< output <<", setpoint " << setpoint << " +/- " << hysteresis << " mode cooling " << cooling;
        if(differentialDevice!="") LOG_VERBOSE(m_Log) << "Differential : Device " << differentialDevice << " gap ON " << engagementGap << " gap OFF " << disengagementGap;
        try
        {
            m_Thermostats.emplace_back(name, output, setpoint, tempDevice, hysteresis, cooling, differentialDevice, engagementGap, disengagementGap);
        }
        catch(string e)
        {
            LOG_WARNING(m_Log) << "Bad thermostat format (" << e << ")";
        }
    }

    for(it=m_Thermostats.begin(); it!=m_Thermostats.end(); ++it) AddThermostatToCache(&(*it));

	LOG_EXIT_OK;
}

bool xPLThermostat::MsgAnswer(xPL::SchemaObject& msg)
{
    xPL::ISchema::MsgType msgType;


    msgType = msg.GetMsgType();
    if((msgType==xPL::SchemaObject::trig)||(msgType==xPL::SchemaObject::stat)) return MsgAnswerTrig(msg);
    if(msgType==xPL::SchemaObject::cmnd) return MsgAnswerCmnd(msg);
    return true;
}

bool xPLThermostat::MsgAnswerCmnd(xPL::SchemaObject& msg)
{
    string device;
    string current;
    list<Thermostat>::iterator it;


    device = msg.GetValue("device");
    if(device=="") return true;

    current = msg.GetValue("current");
    if(current=="") return true;

    for(it=m_Thermostats.begin(); it!=m_Thermostats.end(); ++it)
    {
        if(it->GetName()==device)
        {
            it->SetMode(current);
            if(it->StateChange()) SendValue(&(*it));
            return true;
        }
    }

    return true;
}

bool xPLThermostat::MsgAnswerTrig(xPL::SchemaObject& msg)
{
    string source;
    string device;
    string current;
    std::vector<Thermostat*> lstThermostat;
    std::vector<Thermostat*>::iterator it;
    std::vector<Thermostat*>::const_iterator itEnd;


    device = msg.GetValue("device");
    if(device=="") return true;

    current = msg.GetValue("current");
    if(current=="") return true;

    source = msg.GetSource();
    if(source=="") return true;

    device = source+":"+device;

    lstThermostat = m_Cache.UpdateDevice(device, atof(current.c_str()));
    itEnd = lstThermostat.end();
    for(it=lstThermostat.begin(); it!=itEnd; ++it)
    {
        if(!ValoriseThemostat(*it)) continue;
        if(!(*it)->StateChange()) continue;
        SendValue(*it);
    }

    return true;
}

bool xPLThermostat::ValoriseThemostat(Thermostat* thermostat)
{
    double value;
    string device;

    if(!m_Cache.GetValue(thermostat->GetTemperatureDevice(), &value)) return false;
    thermostat->SetTemperatureValue(value);

    device = thermostat->GetDifferentialDevice();
    if(device == "") return true;
    if(!m_Cache.GetValue(device, &value)) return false;
    thermostat->SetDifferentialValue(value);
    return true;
}

void xPLThermostat::AddThermostatToCache(Thermostat* thermostat)
{
    string device;

    device = thermostat->GetTemperatureDevice();
    if(m_Cache.AddDevice(device, thermostat)) RequestValue(device);

    device = thermostat->GetDifferentialDevice();
    if(device!="")
        if(m_Cache.AddDevice(device, thermostat)) RequestValue(device);
}

int xPLThermostat::ServiceStart(int argc, char* argv[])
{
    m_bServiceStop = false;
    if(argc > 1) m_xPLDevice.SetConfigFileName(argv[1]);
    m_xPLDevice.Open();

    while(!m_bServiceStop)
    {
        if(m_bServicePause)
            Plateforms::delay(500);
        else
            m_xPLDevice.WaitRecv(500);
    }

    m_xPLDevice.Close();
    return 0;
}

void xPLThermostat::ServicePause(bool bPause)
{
    m_bServicePause = bPause;
}

void xPLThermostat::ServiceStop()
{
    m_bServiceStop = true;
}
