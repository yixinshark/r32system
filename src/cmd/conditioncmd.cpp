//
// Created by yixin on 2023/7/19.
//

#include "conditioncmd.h"
#include "handledatabase.h"
#include "recorddata.h"

ConditionCmd::ConditionCmd()
{

}

ConditionCmd::~ConditionCmd()
{

}

void ConditionCmd::initCmd()
{

}

int ConditionCmd::waitSecs()
{
    return m_waitSecs;
}

QString ConditionCmd::cmdInfo()
{
    return QString("获取当前浓度，目标浓度：%1").arg(m_targetConcentration);
}

void ConditionCmd::execute()
{
    if (m_enable) {
        static bool executed = false;
        if (!executed) {
            executed = true;
            // 开启定时获取当前浓度
            m_sender->startPeriodTask(true);
        }
    } else {
        m_sender->startPeriodTask(false);
        m_overed = true;
    }
}

bool ConditionCmd::exeOvered()
{
    return m_overed;
}

bool ConditionCmd::exeSuccess()
{
    return false;
}

QString ConditionCmd::exeErrInfo()
{
    return QString();
}

void ConditionCmd::recvAckTimeout()
{
    // 获取当前浓度
    int concentration = RecordData::instance()->getCurrentConcentration();
    if (concentration >= m_targetConcentration) {
        m_overed = true;
    }
}

void ConditionCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        // 获取当前浓度
        int concentration = RecordData::instance()->getCurrentConcentration();
        if (concentration >= m_targetConcentration) {
            m_overed = true;
        }
    }
}
