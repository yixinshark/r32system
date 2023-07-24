//
// Created by zyz on 23-7-18.
//

#include "compoundcmd.h"

CompoundCmd::CompoundCmd()
{
    m_cmdType = CMD_TYPE_MULTI;
}

CompoundCmd::~CompoundCmd()
{
    if (m_cmdQueue.isEmpty())
        return;

    for (auto *cmd : m_cmdQueue)
        delete cmd;
}

void CompoundCmd::initCmd()
{

}

int CompoundCmd::waitSecs()
{
    if (m_curCmd)
        return m_curCmd->waitSecs();

    auto *cmd = m_cmdQueue.first();
    if (cmd)
        return cmd->waitSecs();

    return 0;
}

QString CompoundCmd::cmdInfo()
{
    QString info;
    info = QString("通道%1: ").arg(m_loopIndex);

    if (m_curCmd)
        info += m_curCmd->cmdInfo();
    else {
        auto *cmd = m_cmdQueue.first();
        if (cmd)
            info += cmd->cmdInfo();
    }

    return info;
}

void CompoundCmd::execute()
{
    if (m_curCmd) {
        m_curCmd->execute();
    } else {
       if (!m_cmdQueue.isEmpty()) {
          m_curCmd = m_cmdQueue.dequeue();
          m_curCmd->execute();
          m_cmdQueueBak.enqueue(m_curCmd);
       }
    }

    if (m_curCmd->exeOvered()) {
        m_curCmd = nullptr;
    } else {
        return;
    }

    if (m_cmdQueue.isEmpty()) {
        if (m_loopIndex >= m_loopCount) {
            m_overed = true;
        } else {
            m_cmdQueue = m_cmdQueueBak;
            m_cmdQueueBak.clear();
        }

        m_loopIndex++;
    }
}

bool CompoundCmd::exeOvered()
{
    return m_overed;
}

bool CompoundCmd::exeSuccess()
{
    return false;
}

QString CompoundCmd::exeErrInfo()
{
    return QString();
}

void CompoundCmd::recvCmdAckData(quint8 cmdCode)
{
    if (m_curCmd)
        m_curCmd->recvCmdAckData(cmdCode);

//    if (m_curCmd->exeOvered()) {
//        m_curCmd = nullptr;
//        execute();
//    }
}

void CompoundCmd::recvAckTimeout()
{
    if (m_curCmd)
        m_curCmd->recvAckTimeout();

//    if (m_curCmd->exeOvered()) {
//        m_curCmd = nullptr;
//        execute();
//    }
}
