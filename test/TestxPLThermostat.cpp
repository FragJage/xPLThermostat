#include<iostream>

#include "TimeMock.h"
#include "TestxPLThermostat.h"

using namespace std;

TestxPLThermostat::TestxPLThermostat() : TestClass("xPLThermostat", this)
{
	addTest("Start", &TestxPLThermostat::Start);
	addTest("StdConfig", &TestxPLThermostat::StdConfig);
	addTest("SetAdvConfig", &TestxPLThermostat::SetAdvConfig);
	addTest("Stop", &TestxPLThermostat::Stop);
	addTest("ReStart", &TestxPLThermostat::ReStart);
	addTest("ModeAuto", &TestxPLThermostat::ModeAuto);
	addTest("ModeForceON", &TestxPLThermostat::ModeForceON);
	addTest("ModeForceOFF", &TestxPLThermostat::ModeForceOFF);
	addTest("GetAdvConfig", &TestxPLThermostat::GetAdvConfig);
	addTest("ModifyAdvConfig", &TestxPLThermostat::ModifyAdvConfig);
	addTest("DelAdvConfig", &TestxPLThermostat::DelAdvConfig);
	addTest("ReStop", &TestxPLThermostat::ReStop);
}

TestxPLThermostat::~TestxPLThermostat()
{
}

void TestxPLThermostat::ThreadStart(xPLThermostat* pxPLDev)
{
    char exeName[] = "test";
    char confName[] = "config";
    char* argv[2];

    argv[0]= exeName;
    argv[1]= confName;
    pxPLDev->ServiceStart(2, argv);
}

bool TestxPLThermostat::Start()
{
    string msg;
    xPL::SchemaObject sch;

    remove("config");

    SetMockTime(2015, 1, 1, 0, 0, 0);
    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xPL Thermostat"==sch.GetValue("appname"));

    return true;
}

bool TestxPLThermostat::StdConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");

    schCfg.SetValue("interval", "30");
    schCfg.SetValue("newconf", "test");
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-thermost.default");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);     //Pass Hbeat message
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("30"==sch.GetValue("interval"));
    assert("fragxpl-thermost.test"==sch.GetSource());

    return true;
}

bool TestxPLThermostat::SetAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "request");

    schAdvCfg.SetValue("configname", "PoolTh");
    schAdvCfg.SetValue("output", "fragxpl-onewire.default:poolheating");
    schAdvCfg.SetValue("setpoint", "28");
    schAdvCfg.SetValue("temperaturedevice", "fragxpl-onewire.default:pooltemp");
    schAdvCfg.SetValue("hysteresis", "0.5");
    schAdvCfg.SetValue("cooling", "0");
    schAdvCfg.SetValue("differentialdevice", "fragxpl-onewire.default:solartemp");
    schAdvCfg.SetValue("differentialengagementgap", "6");
    schAdvCfg.SetValue("differentialdisengagementgap", "3");

    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xpl-cmnd"==sch.GetMsgTypeStr());
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());
    assert("pooltemp"==sch.GetValue("device"));

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xpl-cmnd"==sch.GetMsgTypeStr());
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());
    assert("solartemp"==sch.GetValue("device"));

    return true;
}

void TestxPLThermostat::SetDeviceValue(string device, string current, string type, string source)
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schSensor(xPL::ISchema::trig, "sensor", "basic");


    schSensor.SetValue("device", device);
    schSensor.SetValue("current", current);
    schSensor.SetValue("type", type);
    msg = schSensor.ToMessage(source, "*");
    ControlSockMock::SetNextRecv(msg);
}

bool TestxPLThermostat::ModeAuto()
{
    string msg;
    xPL::SchemaObject sch;

    SetDeviceValue("pooltemp", "20.8", "temp", "fragxpl-onewire.default");
    Plateforms::delay(600);
    SetDeviceValue("solartemp", "65", "temp", "fragxpl-onewire.default");
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xpl-cmnd"==sch.GetMsgTypeStr());
    assert("control"==sch.GetClass());
    assert("basic"==sch.GetType());
    assert("poolheating"==sch.GetValue("device"));
    assert("high"==sch.GetValue("current"));

    SetDeviceValue("solartemp", "21", "temp", "fragxpl-onewire.default");
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("poolheating"==sch.GetValue("device"));
    assert("low"==sch.GetValue("current"));

    SetDeviceValue("solartemp", "45", "temp", "fragxpl-onewire.default");
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("poolheating"==sch.GetValue("device"));
    assert("high"==sch.GetValue("current"));

    SetDeviceValue("pooltemp", "28.6", "temp", "fragxpl-onewire.default");
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("poolheating"==sch.GetValue("device"));
    assert("low"==sch.GetValue("current"));

    return true;
}

bool TestxPLThermostat::ModeForceON()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCtrl(xPL::ISchema::cmnd, "control", "basic");


    schCtrl.SetValue("device", "PoolTh");
    schCtrl.SetValue("current", "HIGH");
    schCtrl.SetValue("type", "output");
    msg = schCtrl.ToMessage("fragxpl-test.default", "*");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xpl-cmnd"==sch.GetMsgTypeStr());
    assert("control"==sch.GetClass());
    assert("basic"==sch.GetType());
    assert("poolheating"==sch.GetValue("device"));
    assert("high"==sch.GetValue("current"));

    return true;
}

bool TestxPLThermostat::ModeForceOFF()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCtrl(xPL::ISchema::cmnd, "control", "basic");


    schCtrl.SetValue("device", "PoolTh");
    schCtrl.SetValue("current", "LOW");
    schCtrl.SetValue("type", "output");
    msg = schCtrl.ToMessage("fragxpl-test.default", "*");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xpl-cmnd"==sch.GetMsgTypeStr());
    assert("control"==sch.GetClass());
    assert("basic"==sch.GetType());
    assert("poolheating"==sch.GetValue("device"));
    assert("low"==sch.GetValue("current"));

    return true;
}

bool TestxPLThermostat::GetAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfg.SetValue("command", "request");
    schAdvCfg.SetValue("configname", "PoolTh");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("advanceconfig"==sch.GetClass());
    assert("current"==sch.GetType());
    assert("PoolTh"==sch.GetValue("configname"));
    assert("fragxpl-onewire.default:poolheating"==sch.GetValue("output"));
    assert("28"==sch.GetValue("setpoint"));
    assert("fragxpl-onewire.default:pooltemp"==sch.GetValue("temperaturedevice"));
    assert("0.5"==sch.GetValue("hysteresis"));

    return true;
}

bool TestxPLThermostat::ModifyAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfgReq(xPL::ISchema::cmnd, "advanceconfig", "request");
    xPL::SchemaObject schAdvCfgCur(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfgReq.SetValue("configname", "PoolTh");
    schAdvCfgReq.SetValue("output", "fragxpl-onewire.default:poolheating");
    schAdvCfgReq.SetValue("setpoint", "28");
    schAdvCfgReq.SetValue("temperaturedevice", "fragxpl-onewire.default:pooltemp");
    schAdvCfgReq.SetValue("hysteresis", "0.4");
    schAdvCfgReq.SetValue("cooling", "0");
    schAdvCfgReq.SetValue("differentialdevice", "");
    schAdvCfgReq.SetValue("differentialengagementgap", "");
    schAdvCfgReq.SetValue("differentialdisengagementgap", "");

    msg = schAdvCfgReq.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);
    msg = ControlSockMock::GetLastSend(10);     //Pass request state of device fragxpl-owfs.default:poolheating

    schAdvCfgCur.SetValue("command", "request");
    schAdvCfgCur.SetValue("configname", "PoolTh");
    msg = schAdvCfgCur.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("advanceconfig"==sch.GetClass());
    assert("current"==sch.GetType());
    assert("PoolTh"==sch.GetValue("configname"));
    assert("fragxpl-onewire.default:poolheating"==sch.GetValue("output"));
    assert("0.4"==sch.GetValue("hysteresis"));

    return true;
}

bool TestxPLThermostat::Stop()
{
    string msg;
    xPL::SchemaObject sch;

    xPLDev.ServicePause(true);
    Plateforms::delay(800);
    xPLDev.ServicePause(false);
    xPLDev.ServiceStop();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(200);
    return true;
}

bool TestxPLThermostat::ReStart()
{
    string msg;
    xPL::SchemaObject sch;

    SetMockTime(2016, 1, 1, 0, 0, 0);
    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("app"==sch.GetType());

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());

    return true;
}

bool TestxPLThermostat::DelAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfg.SetValue("command", "delete");
    schAdvCfg.SetValue("configname", "PoolTh");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(600);

    schAdvCfg.ClearValues();
    schAdvCfg.SetValue("command", "delete");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-thermost.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(600);

    return true;
}

bool TestxPLThermostat::ReStop()
{
    string msg;
    xPL::SchemaObject sch;

    xPLDev.ServiceStop();
    Plateforms::delay(600);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(200);

    remove("config");
    return true;
}

