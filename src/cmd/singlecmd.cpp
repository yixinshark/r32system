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
        info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 命令码:" + QString::number(m_cmdCode);
    info += " 内容:" + m_sendData.toHex();

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
    m_errInfo = QString("%1命令%2,执行超时").arg(m_sender->senderName()).arg(m_cmdCode);
}