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

    if (m_waitSecs > 0) {
        info = QString("等待%1秒").arg(m_waitSecs);
    } else {
        if (m_sender)
            info = QString("发送到:%1").arg(m_sender->senderName());
        info += " 命令码:0x" + QString::number(m_cmdCode, 16);
        info += " 内容:" + m_sendData.toHex();
    }

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
        if (!m_executeSuccess) {
            m_errInfo = QString("命令0x%1,执行失败").arg(QString::number(m_cmdCode, 16));
        } else {
            m_errInfo = QString("命令0x%1,执行成功").arg(QString::number(m_cmdCode,16));
        }
        m_sentCount = 0;
        // 组合命令中复用了该命令，所以需要重置
        m_executeSuccess = false;
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
        m_errInfo = QString("获取错误ACK:0x%1").arg(QString::number(cmdCode, 16));
    }
}

void SingleCmd::setSendData(const QByteArray &data)
{
    m_sendData = data;
}

void SingleCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令0x%2,执行超时").arg(m_sender->senderName()).arg(QString::number(m_cmdCode, 16));
    // 超时后，等待时间清零
    m_waitSecs = 0;
}