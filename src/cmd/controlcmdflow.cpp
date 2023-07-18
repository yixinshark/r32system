//
// Created by yixin on 2023/7/18.
//

#include "controlcmdflow.h"
#include "handlemcudata.h"
#include "singlecmd.h"
#include "compoundcmd.h"
#include "mcuconstant.h"

#include <QVariantMap>

ControlCmdFlow::ControlCmdFlow(QObject *parent)
    : QObject(parent)
{

}

ControlCmdFlow::~ControlCmdFlow()
{

}

void ControlCmdFlow::start()
{

}

void ControlCmdFlow::initCmdFlow()
{

}

void ControlCmdFlow::setR32AnaDataHandler(HandleDataBase *handler)
{
    m_r32AnaDataHandler = handler;
}

void ControlCmdFlow::setMcuDataHandler(HandleDataBase *handler)
{
    m_mcuDataHandler = handler;
}

void ControlCmdFlow::setR32DataHandler(HandleDataBase *handler)
{
    m_r32DataHandler = handler;
}

BaseCmd *ControlCmdFlow::initGasPointCalibration(int point, quint16 gasConcentration) {
    return nullptr;
}

BaseCmd *ControlCmdFlow::initSwitchSensorChannel(int channel)
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_CHANNEL);

    QVariantMap info;
    info.insert(MCU_SET_CHANNEL, channel);
    QByteArray data = m_mcuDataHandler->getSendData(MCU_CMD_CHANNEL, info);
    cmd->setSendData(data);

    return cmd;
}

