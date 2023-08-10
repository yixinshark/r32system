//
// Created by yixin on 2023/8/10.
//

#include "detectpointcmd.h"
#include "handledatabase.h"
#include "r32constant.h"
#include "recorddata.h"

#include <QVariantMap>

DetectPointCmd::DetectPointCmd()
{

}

DetectPointCmd::~DetectPointCmd() {

}

void DetectPointCmd::initCmd() {

}

int DetectPointCmd::waitSecs()
{
    return 0;
}

QString DetectPointCmd::cmdInfo()
{
    QString info;
    info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 命令码:0x" + QString::number(m_cmdCode,16);
    info += " 检测点:" + QString::number(m_point);
    return info;
}

void DetectPointCmd::execute()
{
    RecordData::instance()->setDetectPoint(m_point);
    m_sender->sendCmdData(m_sendData);
    m_sentCount++;
}

bool DetectPointCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        if (!m_executeSuccess) {
            m_errInfo = QString("检测点%1命令0x%2,执行失败").arg(m_point).arg(QString::number(m_cmdCode,16));
        } else {
            m_errInfo = QString("检测点%1命令0x%2,执行成功").arg(m_point).arg(QString::number(m_cmdCode,16));
        }
        m_sentCount = 0;
        // 组合命令中复用了该命令，所以需要重置
        m_executeSuccess = false;
        return true;
    } else {
        return false;
    }
}

bool DetectPointCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString DetectPointCmd::exeErrInfo()
{
    return m_errInfo;
}

void DetectPointCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令0x%2,执行超时").arg(m_sender->senderName()).arg(QString::number(m_cmdCode,16));
}

void DetectPointCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        m_executeSuccess = true;
    } else {
        m_errInfo = QString("获取错误ACK:0x%1").arg(QString::number(cmd,16));
    }
}

void DetectPointCmd::setSender(HandleDataBase *sender)
{
    m_sender = sender;
    m_cmdCode = CMD_READ_GAS_CONCENTRATION_25;

    QVariantMap info;
    m_sendData = m_sender->getSendData(CMD_READ_GAS_CONCENTRATION_25, info);
}
