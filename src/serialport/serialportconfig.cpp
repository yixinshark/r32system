//
// Created by yixin on 2023/8/11.
//

#include <QSettings>
#include <QCoreApplication>
#include "serialportconfig.h"

#if 0
[r32]
PortName=COM1
BaudRate=115200
Parity=None
DataBits=8
StopBits=1

[r32Analyser]
PortName=COM2
BaudRate=9600
Parity=Even
DataBits=7
StopBits=2

[mcu]
PortName=COM3
BaudRate=57600
Parity=None
DataBits=8
StopBits=1

#endif

SerialPortConfig::SerialPortConfig()
{

}

SerialPortConfig::~SerialPortConfig()
{

}

void SerialPortConfig::saveSettings(const QString &beginGroup, const SerialPortParams &serialPortParams)
{
    // 可执行程序目录下的serial_config.ini文件
    QString path = QCoreApplication::applicationDirPath() + "/serial_config.ini";
    // Write settings to INI file
    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup(beginGroup); // Use the selected port as the group name
    settings.setValue("PortName", serialPortParams.portName);
    settings.setValue("BaudRate", serialPortParams.baudRate);
    settings.setValue("Parity", serialPortParams.parity);
    settings.setValue("DataBits", serialPortParams.dataBits);
    settings.setValue("StopBits", serialPortParams.stopBits);
    settings.endGroup();
}

SerialPortParams SerialPortConfig::loadSettings(const QString &beginGroup)
{
    SerialPortParams serialPortParams;
    // 可执行程序目录下的serial_config.ini文件
    QString path = QCoreApplication::applicationDirPath() + "/serial_config.ini";
    // Read settings from INI file
    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup(beginGroup); // Use the selected port as the group name
    serialPortParams.portName = settings.value("PortName").toString();
    serialPortParams.baudRate = settings.value("BaudRate").toInt();
    serialPortParams.parity = static_cast<QSerialPort::Parity>(settings.value("Parity").toInt());
    serialPortParams.dataBits = static_cast<QSerialPort::DataBits>(settings.value("DataBits").toInt());
    serialPortParams.stopBits = static_cast<QSerialPort::StopBits>(settings.value("StopBits").toInt());
    settings.endGroup();

    return serialPortParams;
}

bool SerialPortConfig::isValid() const
{
    QString path = QCoreApplication::applicationDirPath() + "/serial_config.ini";
    QSettings settings(path, QSettings::IniFormat);

    return settings.contains("/r32/PortName") && settings.contains("/r32Analyser/PortName") && settings.contains("/mcu/PortName");
}
