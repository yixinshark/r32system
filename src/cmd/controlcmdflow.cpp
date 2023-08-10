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
#include "setaddresscmd.h"

#include <QDebug>
#include <QVariantMap>
#include "detectpointcmd.h"

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
        initCalibrationCmdFlow1();
    }

    if (m_detectMode) {
        initDetectCmdFlow1();
    }

    executeCmdFlow();
}

void ControlCmdFlow::stop()
{
    m_isStarted = false;
    if (m_timer->isActive())
        m_timer->stop();

    m_r32AnaDataHandler->startPeriodTask(false);

    if (!m_controlCmdFlow.isEmpty()) {
        for (auto *cmd : m_controlCmdFlow) {
            delete cmd;
        }

        m_controlCmdFlow.clear();
    }
}

void ControlCmdFlow::recvAck(char cmd, const QVariantMap &info)
{
    if (!m_isStarted) {
        qWarning() << "ControlCmdFlow::recvAck is not started";
        return;
    }

    auto *currentCmd = m_controlCmdFlow.first();
    if (!currentCmd) {
        qWarning() << "currentCmd is null";
        return;
    }

    currentCmd->recvCmdAckData(cmd);
    if (currentCmd->exeOvered()) {
        if(!currentCmd->exeErrInfo().isEmpty())
            Q_EMIT cmdexecuted(currentCmd->exeErrInfo());
        m_controlCmdFlow.removeFirst();
        delete currentCmd;
    }

    executeCmdFlow();
}

void ControlCmdFlow::initCalibrationCmdFlow()
{
    // 轮询上电检测
    m_controlCmdFlow.append(initPowerOnDetect());
    // 等待3分钟
    m_controlCmdFlow.append(initWaitSecs(3 * 60));
    // 加气体，并搅拌
    m_controlCmdFlow.append(initOperateValve(false, true,false, true));
    m_controlCmdFlow.append(initOperateFan(true, true, true, true));

    for (int i = 0; i < m_calibrationValues.count(); i++) {
        int waitValue = m_calibrationValues.at(i) > 0 ? m_calibrationValues.at(i) + 100 : 0;
        // 等待浓度达到标定点1
        m_controlCmdFlow.append(initWaitConcentration(waitValue));
        // 关闭气体进入,关闭所有电磁阀
        m_controlCmdFlow.append(initOperateValve(false, false, false, false));
        // 1分钟关闭风扇
        m_controlCmdFlow.append(initOperateFan(false, false, false, false, 1 * 60));
        // 开始标记浓度1
        m_controlCmdFlow.append(initGasPointCalibration(i + 1, m_calibrationValues.at(i)));
        // 降低浓度，打开风扇搅拌
        m_controlCmdFlow.append(initOperateValve(false, false, true, true));
        m_controlCmdFlow.append(initOperateFan(true, true, true, true));
    }

    // 关闭R32分析仪获取数据
    m_controlCmdFlow.append(initCloseR32AnaGetGasData());
    // 标定完成
    m_controlCmdFlow.append(initCalibrationOver());

    // TODO 是否要打开箱体
}

void ControlCmdFlow::initDetectCmdFlow()
{
    // 加气体，并搅拌
    m_controlCmdFlow.append(initOperateValve(false, true,false, true));
    m_controlCmdFlow.append(initOperateFan(true, true, true, true));

    for (int i = 0; i < m_detectionValues.count(); i++) {
        int waitValue = m_detectionValues.at(i) > 0 ? m_detectionValues.at(i) + 100 : 0;
        // 等待浓度达到检测点
        m_controlCmdFlow.append(initWaitConcentration(waitValue));
        // 关闭气体进入,关闭所有电磁阀
        m_controlCmdFlow.append(initOperateValve(false, false, false, false));
        // 1分钟关闭风扇
        m_controlCmdFlow.append(initOperateFan(false, false, false, false, 1 * 60));
        // 开始获取浓度
        m_controlCmdFlow.append(initGetGasConcentration(m_detectionValues.at(i)));
        // 降低浓度，打开风扇搅拌
        m_controlCmdFlow.append(initOperateValve(false, false, true, true));
        m_controlCmdFlow.append(initOperateFan(true, true, true, true));
    }

    // 关闭R32分析仪获取数据
    m_controlCmdFlow.append(initCloseR32AnaGetGasData());
}

void ControlCmdFlow::initCalibrationCmdFlow1()
{
    // 轮询上电检测
    m_controlCmdFlow.append(initPowerOnDetect());
    // 等待3分钟
    m_controlCmdFlow.append(initWaitSecs(3 * 60));
    // 加气体，并搅拌
    m_controlCmdFlow.append(initOperateFan(true, true, true, true));
    m_controlCmdFlow.append(initOperateValve(false, true, false, true));

    // 等待到达最高浓度+10%;加气体快，等到值达到最高点后，停止关闭加气，等到标定时仍然很高，会导致第一个标定点失败。所以先升高再降低再标定
    m_controlCmdFlow.append(initWaitConcentration(m_calibrationValues.first() + (int)(m_calibrationValues.first() * 0.1)));
    // 关闭气体进入,关闭所有电磁阀
    m_controlCmdFlow.append(initOperateValve(false, false, false, false));
    // 等待10s
    m_controlCmdFlow.append(initWaitSecs(10));

    for (int i = 0; i < m_calibrationValues.count(); i++) {
        int waitValue = m_calibrationValues.at(i) > 0 ? m_calibrationValues.at(i) + 100 : 0;
        // 降低浓度，风扇没关在搅拌
        m_controlCmdFlow.append(initOperateValve(false, false, true, true));
        // 等待浓度达到标定点
        m_controlCmdFlow.append(initWaitConcentration(waitValue));
        // 关闭气体进入,关闭所有电磁阀
        m_controlCmdFlow.append(initOperateValve(false, false, false, false));
        // 等待1分钟,风扇没关在搅拌
        m_controlCmdFlow.append(initWaitSecs(1 * 60));
        // 开始标记浓度
        m_controlCmdFlow.append(initGasPointCalibration(i + 1, m_calibrationValues.at(i)));
    }

    // 关闭R32分析仪获取数据
    //m_controlCmdFlow.append(initCloseR32AnaGetGasData());
    // 标定完成
    m_controlCmdFlow.append(initCalibrationOver());
    // 关闭风扇
    m_controlCmdFlow.append(initOperateFan(false, false, false, false));
}

void ControlCmdFlow::initDetectCmdFlow1()
{
    // 轮询上电检测
    if (!m_calibrationMode) {
        // 轮询上电检测
        m_controlCmdFlow.append(initPowerOnDetect());
        // TODO 等待3分钟
    }

    // 加气体，并搅拌
    m_controlCmdFlow.append(initOperateFan(true, true, true, true));
    m_controlCmdFlow.append(initOperateValve(false, true,false, true));

    // 等待到达最高浓度+10%;加气体快，等到值达到最高点后，停止关闭加气，等到标定时仍然很高，会导致第一个标定点失败。所以先升高再降低再标定
    m_controlCmdFlow.append(initWaitConcentration(m_detectionValues.first() + (int)(m_detectionValues.first() * 0.1)));
    // 关闭气体进入,关闭所有电磁阀
    m_controlCmdFlow.append(initOperateValve(false, false, false, false));
    // 等待10s
    m_controlCmdFlow.append(initWaitSecs(10));

    for (int i = 0; i < m_detectionValues.count(); i++) {
        int waitValue = m_detectionValues.at(i);
        if (waitValue == 0) {
            waitValue = 50;
        } else if (waitValue > 0 && waitValue <= 1000) {
            waitValue += 50;
        } else {
            waitValue += 100;
        }

        // 降低浓度，风扇没关在搅拌
        m_controlCmdFlow.append(initOperateValve(false, false, true, true));
        // 等待浓度达到检测点
        m_controlCmdFlow.append(initWaitConcentration(waitValue, 0.05));
        // 关闭气体进入,关闭所有电磁阀
        m_controlCmdFlow.append(initOperateValve(false, false, false, false));
        // 等待1分钟，风扇没关在搅拌
        m_controlCmdFlow.append(initWaitSecs(1 * 60));
        // 开始获取浓度
        m_controlCmdFlow.append(initGetGasConcentration(m_detectionValues.at(i)));
    }

    // 关闭R32分析仪获取数据
    m_controlCmdFlow.append(initCloseR32AnaGetGasData());
    // 关闭风扇
    m_controlCmdFlow.append(initOperateFan(false, false, false, false));
    // 开始排气体
    m_controlCmdFlow.append(initOperateValve(false, false, false, true));
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

    //qInfo() << Q_FUNC_INFO;
    auto *currentCmd = m_controlCmdFlow.first();
    if (!currentCmd) {
        qWarning() << "currentCmd is null";
        return;
    }

    currentCmd->recvAckTimeout(); // 执行后修改waitSecs = 0
    QString errorInfo = currentCmd->exeErrInfo();
    if (!errorInfo.isEmpty()) {
        qWarning() << errorInfo;
        Q_EMIT cmdexecuted(errorInfo);
    } else {
        Q_EMIT cmdexecuted("命令执行超时!");
        qWarning() << "currentCmd executed timeout";
    }

    if (currentCmd->exeOvered()) {
        if (!currentCmd->exeErrInfo().isEmpty())
            Q_EMIT cmdexecuted(currentCmd->exeErrInfo());
        m_controlCmdFlow.removeFirst();
        delete currentCmd;
    }

    executeCmdFlow();
}

void ControlCmdFlow::executeCmdFlow()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }

    if (!m_controlCmdFlow.isEmpty()) {
        auto cmd = m_controlCmdFlow.first();
        if (!cmd) {
            qWarning() << "cmd is null";
            return;
        }

        if (cmd->waitSecs() == 0) {
            Q_EMIT cmdexecuted(cmd->cmdInfo());
            cmd->execute();
        }

        int waitSecs = cmd->waitSecs() ? cmd->waitSecs() * 1000 : 200;
        // qInfo() << "executeCmdFlow() wait" << waitSecs << "ms";
        m_timer->setInterval(waitSecs);
        m_timer->start();
        if (cmd->waitSecs() > 0) {
            Q_EMIT cmdexecuted(cmd->cmdInfo());
        }
    } else {
        Q_EMIT exceuteOvered();
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

    // 设置地址
    auto *singleCmd = new SetAddressCmd();
    singleCmd->setFromChannel(m_fromChannel);
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_01);
    compoundCmd->addCmd(singleCmd);

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

BaseCmd *ControlCmdFlow::initWaitSecs(int secs)
{
    auto *cmd = new TimerCmd();
    cmd->setWaitSecs(secs); // 等待secs后执行

    return cmd;
}

BaseCmd *ControlCmdFlow::initOperateValve(bool open1, bool open2, bool open3, bool open4)
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_VALVE);

    QByteArray data;
    data.append(open1 ? (char)0x01 : (char)0x00); // 打开1号电磁阀，即关闭箱体
    data.append(open2 ? (char)0x01 : (char)0x00); // 打开2号电磁阀，R32加气阀
    data.append(open3 ? (char)0x01 : (char)0x00); // 关闭3号电磁阀，空气进气阀
    data.append(open4 ? (char)0x01 : (char)0x00); // 打开4号电磁阀，箱体出气口
    data.append((char)0x00);

    QVariantMap info;
    info.insert(MCU_VALVE, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_VALVE, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initOperateFan(bool open1, bool open2, bool open3, bool open4, int waitSecs)
{
    auto *cmd = new SingleCmd();
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_FAN);
    if (waitSecs > 0)
        cmd->setWaitSecs(waitSecs);

    QByteArray data; // TODO: 风扇控制命令
    data.append(open1 ? (char)0x01 : (char)0x00); // 打开1号风扇
    data.append(open2 ? (char)0x01 : (char)0x00); // 打开2号风扇
    data.append(open3 ? (char)0x01 : (char)0x00); // 打开3号风扇
    data.append(open4 ? (char)0x01 : (char)0x00); // 打开4号风扇

    QVariantMap info;
    info.insert(MCU_FAN, data);
    cmd->setSendData(m_mcuDataHandler->getSendData(MCU_CMD_FAN, info));

    return cmd;
}

BaseCmd *ControlCmdFlow::initWaitConcentration(int point, float precision)
{
    auto *cmd = new ConditionCmd();
    cmd->setCondition(point);
    cmd->setPrecision(precision);
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

BaseCmd *ControlCmdFlow::initCloseR32AnaGetGasData()
{
    auto *cmd = new ConditionCmd();
    cmd->setEnable(false);
    cmd->setSender(m_r32AnaDataHandler);
    cmd->setCmdCode(ANALYSER_CMD);

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

    // TODO 读取电阻值命令码,当用户自定义标定点怎么处理？
    if (concentration == 5000)
        singleCmd->setCmdCode(CMD_READ_5000PPM_06);
    else if (concentration == 1000)
        singleCmd->setCmdCode(CMD_READ_1000PPM_05);
    else
        singleCmd->setCmdCode(CMD_READ_R0_04);

    QVariantMap info;
    singleCmd->setSendData(m_r32DataHandler->getSendData((char)singleCmd->cmdCode(), info));

    return singleCmd;
}

BaseCmd *ControlCmdFlow::initCalibrationOver()
{
    auto *compoundCmd = new CompoundCmd();
    compoundCmd->setLoopCount(m_totalChannel);
    compoundCmd->setBeginLoopIndex(m_fromChannel);

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
    QVariantMap info;
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_02, info));
    compoundCmd->addCmd(singleCmd);

    // 再查询标定状态
    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_ND_STATUS_03);
    info.clear();
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_ND_STATUS_03, info));
    compoundCmd->addCmd(singleCmd);

    // 读取p,p1,p2,温湿度参数
    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_READ_PARMA_P_07);
    info.clear();
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_READ_PARMA_P_07, info));
    compoundCmd->addCmd(singleCmd);

    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_READ_PARMA_P1_08);
    info.clear();
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_READ_PARMA_P1_08, info));
    compoundCmd->addCmd(singleCmd);

    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_READ_PARMA_P2_09);
    info.clear();
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_READ_PARMA_P2_09, info));
    compoundCmd->addCmd(singleCmd);

    singleCmd = new SingleCmd();
    singleCmd->setSender(m_r32DataHandler);
    singleCmd->setCmdCode(CMD_READ_TEMP_HUM_21);
    info.clear();
    singleCmd->setSendData(m_r32DataHandler->getSendData(CMD_READ_TEMP_HUM_21, info));
    compoundCmd->addCmd(singleCmd);

    return compoundCmd;
}

BaseCmd *ControlCmdFlow::initGetGasConcentration(int concentration)
{
    auto *compoundCmd = new CompoundCmd();
    compoundCmd->setLoopCount(m_totalChannel);
    compoundCmd->setBeginLoopIndex(m_fromChannel);

    // 先切通道
    auto *cmd = new ChannelCmd();
    cmd->setFromChannel(m_fromChannel);
    cmd->setSender(m_mcuDataHandler);
    cmd->setCmdCode(MCU_CMD_CHANNEL);
    compoundCmd->addCmd(cmd);

    // 获取气体浓度
    auto *detectCmd = new DetectPointCmd();
    detectCmd->setSender(m_r32DataHandler);
    detectCmd->setDetectPoint(concentration);
    compoundCmd->addCmd(detectCmd);

    return compoundCmd;
}
