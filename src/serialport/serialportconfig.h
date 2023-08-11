//
// Created by yixin on 2023/8/11.
//

#ifndef R32SYSTEM_SERIALPORTCONFIG_H
#define R32SYSTEM_SERIALPORTCONFIG_H

#include "serialportsettingsdialog.h"

class SerialPortConfig
{
public:
    SerialPortConfig();
    ~SerialPortConfig();

    bool isValid() const;

public:
    void saveSettings(const QString &beginGroup, const SerialPortParams &serialPortParams);
    SerialPortParams loadSettings(const QString &beginGroup);
};

#endif //R32SYSTEM_SERIALPORTCONFIG_H
