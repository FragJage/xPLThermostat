#ifndef XPLTHERMOSTAT_H
#define XPLTHERMOSTAT_H

#include<list>
#include "Service/Service.h"
#include "xPLLib/xPLDevCfg.h"
#include "xPLLib/Extensions/AdvanceConfig.h"
#include "Thermostat.h"
#include "CacheManager.h"

class xPLThermostat : public Service::IService, public xPL::xPLDevice::IExtension, public xPL::AdvanceConfig::ICallBack
{
    public:
        xPLThermostat();
        ~xPLThermostat();

        void AdvanceConfigure();
        bool MsgAnswer(xPL::SchemaObject& msg);
        void ConfigChange(const std::string& device);
        void ConfigDelete(const std::string& device);
        void ConfigsDelete();

		int ServiceStart(int argc, char* argv[]);
		void ServicePause(bool bPause);
		void ServiceStop();

    private:
        void AddThermostatToCache(Thermostat* thermostat);
        bool ValoriseThemostat(Thermostat* thermostat);
        void RequestValue(const std::string& longDevice);
        void SendValue(Thermostat* thermostat);
        bool MsgAnswerCmnd(xPL::SchemaObject& msg);
        bool MsgAnswerTrig(xPL::SchemaObject& msg);

        SimpleLog* m_Log;
        xPL::xPLDevCfg m_xPLDevice;
        xPL::AdvanceConfig m_AdvanceConfig;
        bool m_bServicePause;
        bool m_bServiceStop;
        std::list<Thermostat> m_Thermostats;
        CacheManager m_Cache;
};

#endif // XPLTHERMOSTAT_H
