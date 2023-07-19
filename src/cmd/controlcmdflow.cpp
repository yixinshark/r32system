//
// Created by yixin on 2023/7/18.
//

#include "controlcmdflow.h"
#include "handlemcudata.h"
#include "singlecmd.h"
#include "compoundcmd.h"
#include "mcuconstant.h"
#include "r32constant.h"
#include "recorddata.h"
#include "timercmd.h"
#include "conditioncmd.h"
#include "analyserconstant.h"

#include <QDebug>
#include <QVariantMap>

ControlCmdFlow::ControlCmdFlow(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_recordData = RecordData::instance();

    m_timer->setSingleShot(false);
    m_timer->setInterval(200); // 默认200ms
    connect(m_timer, &QTimer::timeout, this, &ControlCmdFlow::timerTimeout);
}

ControlCmdFlow::~ControlCmdFlow()
{

}

void ControlCmdFlow::start()
{
   executeCmdFlow();
}

void ControlCmdFlow::recvAck(char cmd, const QVariantMap &info)
{
    auto *currentCmd = m_calibrationCmdFlow.first();
    if (!currentCmd) {
        qWarning() << "currentCmd is null";
        return;
    }

    currentCmd->recvCmdAckData(cmd);
    if (currentCmd->exeOvered()) {
        m_calibrationCmdFlow.removeFirst();
        delete currentCmd;

        executeCmdFlow();
    } else {
        currentCmd->execute();
        Q_EMIT cmdexecuted(currentCmd->cmdInfo());
    }
}

void ControlCmdFlow::initCmdFlow()
{
    m_calibrationCmdFlow.append(initSwitchChannel(1));
    m_calibrationCmdFlow.append(initReadSensorAddress());
    m_calibrationCmdFlow.append(initReadFirmwareVersion());
    m_calibrationCmdFlow.append(initWait3Minutes());
    m_calibrationCmdFlow.append(initOperateValve());
    m_calibrationCmdFlow.append(initOperateFan());
    m_calibrationCmdFlow.append(initWait5000Concentration());
    m_calibrationCmdFlow.append(initCloseValve());
    m_calibrationCmdFlow.append(initCloseFan());
    m_calibrationCmdFlow.append(initGasPointCalibration(1, 5000));
    m_calibrationCmdFlow.append(initGasPointCalibration(2, 1000));
    m_calibrationCmdFlow.append(initGasPointCalibration(3, 0));
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

void ControlCmdFlow::timerTimeout()
{
    qInfo() << Q_FUNC_INFO;
    auto *currentCmd = m_calibrationCmdFlow.first();
    if (!currentCmd) {
        qWarning() << "currentCmd is null";
        return;
    }

    currentCmd->recvAckTimeout(); // TODO tips
    QString errorInfo = currentCmd->exeErrInfo();
    if (!errorInfo.isEmpty()) {
        qWarning() << errorInfo;
        Q_EMIT cmdexecuted(errorInfo);
    }

    if (currentCmd->exeOvered()) {
        m_calibrationCmdFlow.removeFirst();
        delete currentCmd;

        executeCmdFlow();
    } else {
        Q_EMIT cmdexecuted(currentCmd->cmdInfo());
        currentCmd->execute();
    }
}

void ControlCmdFlow::executeCmdFlow()
{
    if (!m_calibrationCmdFlow.isEmpty()) {
        if (m_timer->isActive()) {
            m_timer->stop();
        }

        auto cmd = m_calibrationCmdFlow.first();
        if (!cmd) {
            qWarning() << "cmd is null";
            return;
        }

        if (cmd->waitSecs() == 0) {
            Q_EMIT cmdexecuted(m_calibrationCmdFlow.first()->cmdInfo());
            cmd->execute();
        }

        int waitSecs = cmd->waitSecs() ? cmd->waitSecs() * 1000 : 200;
        qInfo() << "executeCmdFlow() wait" << waitSecs << "seconds";
        m_timer->setInterval(waitSecs);
        m_timer->start();
        if (cmd->waitSecs() > 0) {
            Q_EMIT cmdexecuted(m_calibrationCmdFlow.first()->cmdInfo());
        }
    } else {
        if (m_timer->isActive())
            m_timer->stop();
    }
}

BaseCmd *ControlCmdFlow::initSwitchChannel(int channel)
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

BaseCmd *ControlCmdFlow::initReadSensorAddress()
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_r32DataHandler);
    cmd->setCmdCode(CMD_02);

    QVariantMap info;
    info.insert(MODULE_ADDRESS, true);
    QByteArray data = m_r32DataHandler->getSendData(CMD_02, info);
    cmd->setSendData(data);

    return cmd;
}

BaseCmd *ControlCmdFlow::initReadFirmwareVersion()
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_r32DataHandler);
    cmd->setCmdCode(CMD_READ_FIRMWARE_VERSION_41);

    QVariantMap info;
    QByteArray data = m_r32DataHandler->getSendData(CMD_READ_FIRMWARE_VERSION_41, info);
    cmd->setSendData(data);

    return cmd;
}

BaseCmd *ControlCmdFlow::initWait3Minutes()
{
    auto *cmd = new TimerCmd();
    cmd->setWaitSecs(3 * 60); // 等待3分钟后执行

    return cmd;
}

BaseCmd *ControlCmdFlow::initOperateValve()
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_VALVE);

    QByteArray data;
    data.append((char)0x00); // 打开1号电磁阀，即关闭箱体
    data.append((char)0x01); // 打开2号电磁阀，R32加气阀
    data.append((char)0x00); // 关闭3号电磁阀，空气进气阀
    data.append((char)0x01); // 打开4号电磁阀，箱体出气口
    data.append((char)0x00);

    QVariantMap info;
    info.insert(MCU_VALVE, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_VALVE, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initOperateFan()
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_FAN);

    QByteArray data; // TODO: 风扇控制命令
    data.append((char)0x01); // 打开1号风扇
    data.append((char)0x01); // 打开2号风扇
    data.append((char)0x01); // 打开3号风扇
    data.append((char)0x01); // 打开4号风扇

    QVariantMap info;
    info.insert(MCU_FAN, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_FAN, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initWait5000Concentration()
{
    auto *cmd = new ConditionCmd();
    cmd->setCondition(5100);
    cmd->setSender(m_r32AnaDataHandler);
    cmd->setCmdCode(ANALYSER_CMD);

    QVariantMap info;
    cmd->setSendData(m_r32AnaDataHandler->getSendData(ANALYSER_CMD, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initCloseValve()
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_VALVE);

    QByteArray data; // TODO: 电磁阀控制命令
    data.append((char)0x01); // 打开1号电磁阀，即打开箱体
    data.append((char)0x00); // 打开2号电磁阀，R32加气阀
    data.append((char)0x00); // 关闭3号电磁阀，空气进气阀
    data.append((char)0x00); // 打开4号电磁阀，箱体出气口
    data.append((char)0x00); // 关闭5号电磁阀，TODO: 5号电磁阀是什么阀

    QVariantMap info;
    info.insert(MCU_VALVE, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_VALVE, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initCloseFan()
{
    auto *cmd = new SingleCmd();
    cmd->setWaitSecs(1 * 60); // 等待1分钟后执行
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_FAN);

    QByteArray data; // TODO: 风扇控制命令
    data.append((char)0x00); // 打开1号风扇
    data.append((char)0x00); // 打开2号风扇
    data.append((char)0x00); // 打开3号风扇
    data.append((char)0x00); // 打开4号风扇

    QVariantMap info;
    info.insert(MCU_FAN, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_FAN, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initGasPointCalibration(int point, int concentration)
{
    auto *cmd = new CompoundCmd();
    cmd->setLoopCount(64); // TODO 循环次数使用用户设置
    cmd->setBeginLoopIndex(1);

    // 切通道 TODO
    auto *singleCmd = initSwitchChannel(1);
    cmd->addCmd(dynamic_cast<SingleCmd *>(singleCmd));

    // 标记浓度
    singleCmd = initMarkConcentration(point, concentration);
    cmd->addCmd(dynamic_cast<SingleCmd *>(singleCmd));

    // 读取电阻值
    singleCmd = initReadResistance(concentration);
    cmd->addCmd(dynamic_cast<SingleCmd *>(singleCmd));

    return cmd;
}

BaseCmd *ControlCmdFlow::initMarkConcentration(int point, int concentration)
{
    auto *singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_01);

    QVariantMap info;
    info.insert(SEND_CAL_POINT, point);
    info.insert(SEND_CAL_CONCENTRATION, concentration);
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_01, info));

    return singleCmd;
}

BaseCmd *ControlCmdFlow::initReadResistance(int concentration)
{
    auto *singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);

    if (concentration == 5000)
        singleCmd->setCmdCode(CMD_READ_5000PPM_08);
    else if (concentration == 1000)
        singleCmd->setCmdCode(CMD_READ_1000PPM_07);
    else
        singleCmd->setCmdCode(CMD_READ_R0_04);

    QVariantMap info;
    singleCmd->setSendData(m_r32DataHandler->getSendData((char)singleCmd->cmdCode(), info));

    return singleCmd;
}