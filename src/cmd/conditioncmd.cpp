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
    int waitValue = m_targetConcentration == 0 ? 50 : m_targetConcentration;
    return QString("获取当前浓度%1，目标浓度：%2").arg(RecordData::instance()->getCurrentConcentration()).arg(waitValue);
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
    return QString("等待浓度到达目标浓度: %1").arg(m_targetConcentration == 0 ? 50 : m_targetConcentration);
}

void ConditionCmd::recvAckTimeout()
{
    // 获取当前浓度
    int concentration = RecordData::instance()->getCurrentConcentration();
    // 如果当前浓度大于等于目标浓度小于等于目标浓度的10%
    int waitValue = m_targetConcentration == 0 ? 50 : m_targetConcentration + (int)(m_targetConcentration * m_precision);
    if (concentration >= m_targetConcentration && concentration <= waitValue) {
        m_overed = true;
    }
}

void ConditionCmd::recvCmdAckData(quint8 cmd)
{
    if (cmd == m_cmdCode) {
        // 获取当前浓度
        int concentration = RecordData::instance()->getCurrentConcentration();
        int waitValue = m_targetConcentration == 0 ? 50 :m_targetConcentration + (int)(m_targetConcentration * m_precision);;
        if (concentration >= m_targetConcentration && concentration <= waitValue) {
            m_overed = true;
        }
    }
}
