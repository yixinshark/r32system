//
// Created by yixin on 2023/7/20.
//

#include "channelcmd.h"
#include "recorddata.h"

ChannelCmd::ChannelCmd()
{

}

ChannelCmd::~ChannelCmd()
{

}

void ChannelCmd::initCmd()
{

}

int ChannelCmd::waitSecs()
{
    return 0;
}

QString ChannelCmd::cmdInfo()
{
    QString info;
    if (m_sender)
        info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 切通道:" + QString::number(m_currentChannel == 0 ? m_fromChannel : m_currentChannel);
    info += " 命令码:0x" + QString::number(m_cmdCode,16);
    info += " 内容:" + getSendData().toHex();

    return info;
}

void ChannelCmd::execute()
{
    if (m_currentChannel == 0) {
        m_currentChannel = m_fromChannel;
    } else {
        m_sendData[2] = static_cast<char>(m_currentChannel);
    }

    if (m_sender) {
        m_sendData = getSendData();

        m_sender->sendCmdData(m_sendData);
        RecordData::instance()->setCurrentChannel(m_currentChannel);
        m_sentCount++;
    }
}

bool ChannelCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        if(!m_executeSuccess) {
            m_errInfo = QString("切通道%1命令0x%2,执行失败").arg(m_currentChannel).arg(QString::number(m_cmdCode,16));
        } else {
            m_errInfo = QString("切通道%1命令0x%2,执行成功").arg(m_currentChannel).arg(QString::number(m_cmdCode,16));
        }
        m_currentChannel++;
        m_sentCount = 0;
        // 组合命令中复用了该命令，所以需要重置
        m_executeSuccess = false;
        return true;
    } else {
        return false;
    }
}

bool ChannelCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString ChannelCmd::exeErrInfo()
{
    return m_errInfo;
}

void ChannelCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令0x%2,执行超时").arg(m_sender->senderName()).arg(QString::number(m_cmdCode,16));
}

void ChannelCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        m_executeSuccess = true;
    } else {
        m_errInfo = QString("获取错误ACK:0x%1").arg(QString::number(cmd,16));
    }
}

QByteArray ChannelCmd::getSendData() const
{
    QByteArray data;

    data.append( 0x61);
    data.append(static_cast<char>(m_cmdCode));

    data.append(static_cast<char>(m_currentChannel == 0 ? m_fromChannel : m_currentChannel));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    addCheckSum(data);

    return data;
}

void ChannelCmd::addCheckSum(QByteArray &data) const
{
    quint16 checksum = 0;
    for (char i : data)
        checksum += static_cast<quint8>(i);

    data.append(static_cast<char>((checksum >> 8) & 0xFF));
    data.append(static_cast<char>(checksum & 0xFF));
}