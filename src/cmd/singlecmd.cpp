//
// Created by zyz on 23-7-18.
//

#include "singlecmd.h"
#include "handledatabase.h"

SingleCmd::SingleCmd()
{
    m_cmdType = CMD_TYPE_SINGLE;
}

SingleCmd::~SingleCmd()
{

}

void SingleCmd::initCmd()
{

}

QString SingleCmd::cmdInfo()
{
    QString info;
    if (m_sender)
        info = QString("发送者:%1").arg(m_sender->senderName());
    info += QString::number(m_cmdCode);
    info += m_sendData.toHex();

    return info;
}

void SingleCmd::execute()
{
    if (m_sender) {
        m_sender->sendCmdData(m_sendData);
        m_sentCount++;
    } else {
        m_errInfo = QString("命令发送者为空");
    }
}

bool SingleCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        return true;
    } else {
        return false;
    }
}

bool SingleCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString SingleCmd::exeErrInfo()
{
    return m_errInfo;
}

void SingleCmd::recvCmdAckData(quint8 cmdCode)
{
    if (cmdCode == m_cmdCode) {
        m_executeSuccess = true;
    } else {
        m_errInfo = QString("获取错误ACK:%1").arg(cmdCode);
    }
}

void SingleCmd::setSendData(const QByteArray &data)
{
    m_sendData = data;
}

void SingleCmd::recvAckTimeout()
{
    m_errInfo = QString("命令执行超时");
    m_sentCount++;
}