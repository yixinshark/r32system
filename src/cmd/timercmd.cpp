//
// Created by yixin on 2023/7/19.
//

#include "timercmd.h"

#include <QTimer>
#include <QDebug>

TimerCmd::TimerCmd() {

}

TimerCmd::~TimerCmd() {

}

void TimerCmd::initCmd() {

}

int TimerCmd::waitSecs()
{
    return m_waitSecs;
}

QString TimerCmd::cmdInfo()
{
    return QString("等待%1分钟中").arg(m_waitSecs / 60);
}

void TimerCmd::execute()
{
    qInfo() << "TimerCmd::execute() wait" << m_waitSecs << "seconds";
#if 0
    if (m_timer) {
        if (m_timer->isActive())
            m_timer->stop();

        m_timer->start(m_waitSecs * 1000);
    } else {
        qWarning() << "TimerCmd::execute() m_timer is null";
    }
#endif
    m_overed = true;
}

bool TimerCmd::exeOvered()
{
    return m_overed;
}

bool TimerCmd::exeSuccess()
{
    return true;
}

QString TimerCmd::exeErrInfo()
{
    // nothing to do
    return QString();
}

void TimerCmd::recvAckTimeout()
{
    m_overed = true;
}

void TimerCmd::recvCmdAckData(quint8 cmd)
{
    // nothing to do
}
