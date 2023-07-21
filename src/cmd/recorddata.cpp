//
// Created by yixin on 2023/7/19.
//

#include "recorddata.h"

#include <QDebug>

RecordData *RecordData::instance()
{
    static RecordData *s_instance = nullptr;
    if (!s_instance) {
        s_instance = new RecordData();
    }

    return s_instance;
}

void RecordData::reset()
{
    m_recordData.clear();
    m_currentChanel = 0;
}

RecordData::RecordData()
{

}

RecordData::~RecordData()
{
    m_recordData.clear();
}

void RecordData::setModuleAddress(int addr)
{
    if (m_recordData.contains(m_currentChanel)) {
        qWarning() << "RecordData::setModuleAddress: channel" << m_currentChanel << "already exists";
        m_recordData[m_currentChanel].addr = addr;
    } else {
        R32Info info;
        info.addr = addr;
        m_recordData.insert(m_currentChanel, info);
    }
}

void RecordData::setFirmwareVersion(const QString &version)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].softVersion = version;
        m_recordData[m_currentChanel].on = true;
    } else {
        qWarning() << "RecordData::setFirmwareVersion: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setCurrentConcentration(int concentration)
{
    m_concentrationCache.append(concentration);
    if (m_concentrationCache.size() > 5) {
        m_concentrationCache.removeFirst();
    }
}

int RecordData::getCurrentConcentration() const
{
    // m_concentrationCache缓存少于5个值，返回0，否则返回五个值的平均值
    if (m_concentrationCache.size() < 5) {
        return 0;
    } else {
        int sum = 0;
        for (int i = 0; i < 5; ++i) {
            sum += m_concentrationCache.at(i);
        }
        return sum / 5;
    }
}