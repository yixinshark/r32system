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

void RecordData::setCalPoint(int point)
{
    if (m_recordData.contains(m_currentChanel)) {
        switch (point) {
            case 1:
                m_recordData[m_currentChanel].point1 = getCurrentConcentration();
                // 当前浓度值在5000的10%范围内，认为是有效值
                if (m_recordData[m_currentChanel].point1 >= 4500 && m_recordData[m_currentChanel].point1 <= 5500) {
                    m_recordData[m_currentChanel].point1Valid = true;
                } else {
                    m_recordData[m_currentChanel].point1Valid = false;
                }
                break;
            case 2:
                m_recordData[m_currentChanel].point2 = getCurrentConcentration();
                // 当前浓度值在3000的10%范围内，认为是有效值
                if (m_recordData[m_currentChanel].point2 >= 2700 && m_recordData[m_currentChanel].point2 <= 3300) {
                    m_recordData[m_currentChanel].point2Valid = true;
                } else {
                    m_recordData[m_currentChanel].point2Valid = false;
                }
                break;
            case 3:
                m_recordData[m_currentChanel].point3 = getCurrentConcentration();
                // 当前浓度值在0的10%范围内，认为是有效值
                if (m_recordData[m_currentChanel].point3 >= 0 && m_recordData[m_currentChanel].point3 <= 50) {
                    m_recordData[m_currentChanel].point3Valid = true;
                } else {
                    m_recordData[m_currentChanel].point3Valid = false;
                }
                break;
        }
    } else {
        qWarning() << "RecordData::setCalPoint: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR0Value(float r0)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R0 = r0;
    } else {
        qWarning() << "RecordData::setR0Value: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR1000Value(float r1000)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R1000 = r1000;
    } else {
        qWarning() << "RecordData::setR1000Value: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR5000Value(float r5000)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R5000 = r5000;
    } else {
        qWarning() << "RecordData::setR5000Value: channel" << m_currentChanel << "not exists";
    }
}

const R32Info &RecordData::getR32Info(int channel) const
{
    return m_recordData[channel];
}

bool RecordData::hasChannel(int channel) const
{
    return m_recordData.contains(channel);
}

int RecordData::recordDataCount() const
{
    return m_recordData.size();
}
