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
#include "channelcmd.h"
#include "calccrcmd.h"

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
    stop();
}

void ControlCmdFlow::start()
{
    m_isStarted = true;
    if (m_calibrationMode) {
        initCalibrationCmdFlow();
    }

    if (m_detectMode) {
        initDetectCmdFlow();
    }

    executeCmdFlow();
}

void ControlCmdFlow::stop()
{
    m_isStarted = false;
    if (m_timer->isActive())
        m_timer->stop();

    if (!m_calibrationCmdFlow.isEmpty()) {
        for (auto *cmd : m_calibrationCmdFlow) {
            delete cmd;
        }

        m_calibrationCmdFlow.clear();
    }
}

void ControlCmdFlow::recvAck(char cmd, const QVariantMap &info)
{
    if (!m_isStarted) {
        qWarning() << "ControlCmdFlow::recvAck is not started";
        return;
    }

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

void ControlCmdFlow::initCalibrationCmdFlow()
{
    //m_calibrationCmdFlow.append(initSwitchChannel(m_fromChannel));
    //m_calibrationCmdFlow.append(initReadSensorAddress());
    //m_calibrationCmdFlow.append(initReadFirmwareVersion());
    m_calibrationCmdFlow.append(initPowerOnDetect());
    m_calibrationCmdFlow.append(initWait3Minutes());
    m_calibrationCmdFlow.append(initOperateValve());
    m_calibrationCmdFlow.append(initOperateFan());
    m_calibrationCmdFlow.append(initWait5000Concentration());
    m_calibrationCmdFlow.append(initCloseValve());
    m_calibrationCmdFlow.append(initCloseFan());
    m_calibrationCmdFlow.append(initGasPointCalibration(1, 5000));
    // TODO 降低浓度
    m_calibrationCmdFlow.append(initGasPointCalibration(2, 1000));
    // TODO 降低浓度
    m_calibrationCmdFlow.append(initGasPointCalibration(3, 0));
    m_calibrationCmdFlow.append(initCalibrationOver());
}

void ControlCmdFlow::initDetectCmdFlow()
{
    if (!m_calibrationCmdFlow.isEmpty())
        m_calibrationCmdFlow.clear();

    // TODO: 检测流程
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
    if (!m_isStarted) {
        qWarning() << "ControlCmdFlow::timerTimeout is not started";
        return;
    }

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
        currentCmd->execute();
        Q_EMIT cmdexecuted(currentCmd->cmdInfo());
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
            cmd->execute();
            Q_EMIT cmdexecuted(m_calibrationCmdFlow.first()->cmdInfo());
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

BaseCmd *ControlCmdFlow::initPowerOnDetect()
{
    auto *compoundCmd = new CompoundCmd();
    compoundCmd->setLoopCount(m_totalChannel);
    compoundCmd->setBeginLoopIndex(m_fromChannel);

    // 切通道
    auto *channelCmd = new ChannelCmd();
    channelCmd->setFromChannel(m_fromChannel);
    channelCmd->setSender(m_mcuDataHandler);
    channelCmd->setCmdCode(MCU_CMD_CHANNEL);
    compoundCmd->addCmd(channelCmd);

    // 读取地址
    auto *readAddressCmd = initReadSensorAddress();
    compoundCmd->addCmd(readAddressCmd);

    // 读取固件版本
    auto *readFirmwareVersionCmd = initReadFirmwareVersion();
    compoundCmd->addCmd(readFirmwareVersionCmd);

    return compoundCmd;
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
    cmd->setWaitSecs(2);
    cmd->setSender(m_r32AnaDataHandler);
    cmd->setCmdCode(ANALYSER_CMD);

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
    cmd->setLoopCount(m_totalChannel);
    cmd->setBeginLoopIndex(m_fromChannel);

    // 切通道
    auto *channelCmd = new ChannelCmd();
    channelCmd->setFromChannel(m_fromChannel);
    channelCmd->setSender(m_mcuDataHandler);
    channelCmd->setCmdCode(MCU_CMD_CHANNEL);
    cmd->addCmd(channelCmd);

    // 标记浓度
    auto *singleCmd = initMarkConcentration(point);
    cmd->addCmd(singleCmd);

    // 读取电阻值
    singleCmd = initReadResistance(concentration);
    cmd->addCmd(singleCmd);

    return cmd;
}

BaseCmd *ControlCmdFlow::initMarkConcentration(int point)
{
    auto *singleCmd = new CalCcrCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_01);
    singleCmd->setCalCcrPoint(point);

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

BaseCmd *ControlCmdFlow::initCalibrationOver()
{
    auto *compoundCmd = new CompoundCmd();

    // 先切通道
    auto *cmd = new ChannelCmd();
    cmd->setFromChannel(m_fromChannel);
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_CHANNEL);
    compoundCmd->addCmd(cmd);

    // 再发送标定完成命令
    auto *singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_02);
    // TODO setSendData

    // 再查询标定状态
    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_ND_STATUS_03);
    // TODO setSendData

    return compoundCmd;
}