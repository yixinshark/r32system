//
// Created by yixin on 2023/7/21.
//

#include "calccrcmd.h"
#include "handledatabase.h"
#include "r32constant.h"
#include "recorddata.h"

#include <QVariantMap>

CalCcrCmd::CalCcrCmd()
{

}

CalCcrCmd::~CalCcrCmd()
{

}

void CalCcrCmd::initCmd()
{

}

int CalCcrCmd::waitSecs()
{
    return 0;
}

QString CalCcrCmd::cmdInfo()
{
    QString info;
    info = QString("发送到:%1").arg(m_sender->senderName());
    info += " 标定点:" + QString::number(m_point);
    return info;
}

void CalCcrCmd::execute()
{
    QByteArray data;

    QVariantMap info;
    info.insert(SEND_CAL_POINT, m_point);
    data = m_sender->getSendData((char)m_cmdCode, info);
    m_sender->sendCmdData(data);

    m_sentCount++;
}

bool CalCcrCmd::exeOvered()
{
    if (m_executeSuccess || m_sentCount >= 3) {
        if (!m_executeSuccess) {
            m_errInfo = QString("标定点%1命令%2,执行失败").arg(m_point).arg(m_cmdCode);
        } else {
            m_errInfo = QString("标定点%1命令%2,执行成功").arg(m_point).arg(m_cmdCode);
        }
        return true;
    } else {
        return false;
    }
}

bool CalCcrCmd::exeSuccess()
{
    return m_executeSuccess;
}

QString CalCcrCmd::exeErrInfo()
{
    return m_errInfo;
}

void CalCcrCmd::recvAckTimeout()
{
    m_errInfo = QString("%1命令%2,执行超时").arg(m_sender->senderName()).arg(m_cmdCode);
}

void CalCcrCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        m_executeSuccess = true;
        RecordData::instance()->setCalPoint(m_point);
    } else {
        m_errInfo = QString("获取错误ACK:%1").arg(cmd);
    }
}
