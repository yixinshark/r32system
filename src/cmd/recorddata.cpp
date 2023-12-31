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
        info.channel = addr;
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
    // m_concentrationCache缓存少于4个值，返回0，否则返回4个值的平均值
    int num = 5; // 5个值的平均值
    if (m_concentrationCache.size() < num) {
        return 0;
    } else {
        int sum = 0;
        for (int i = 0; i < num; ++i) {
            sum += m_concentrationCache.at(i);
        }
        return sum / num;
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
                // 当前浓度值在1000的10%范围内，认为是有效值
                if (m_recordData[m_currentChanel].point2 >= 900 && m_recordData[m_currentChanel].point2 <= 1100) {
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

void RecordData::setDetectPoint(int point)
{
    m_curDetectPoint = point;
    if (m_recordData.contains(m_currentChanel)) {
        switch (point) {
            case 0:
                m_recordData[m_currentChanel].ccr0 = getCurrentConcentration();
                break;
            case 500:
                m_recordData[m_currentChanel].ccr500 = getCurrentConcentration();
                break;
            case 1000:
                m_recordData[m_currentChanel].ccr1000 = getCurrentConcentration();
                break;
            case 3000:
                m_recordData[m_currentChanel].ccr3000 = getCurrentConcentration();
                break;
            case 5000:
                m_recordData[m_currentChanel].ccr5000 = getCurrentConcentration();
                break;
        }
    } else {
        qWarning() << "RecordData::setDetectPoint: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setDetectPointValue(int concentration)
{
    if (m_recordData.contains(m_currentChanel)) {
        switch (m_curDetectPoint) {
            case 0:
                setDetectPoint0(concentration);
                break;
            case 500:
                setDetectPoint500(concentration);
                break;
            case 1000:
                setDetectPoint1000(concentration);
                break;
            case 3000:
                setDetectPoint3000(concentration);
                break;
            case 5000:
                setDetectPoint5000(concentration);
                break;
        }
    } else {
        qWarning() << "RecordData::setDetectPointValue: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR0Value(float r0)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R0 = QString::number(r0);
    } else {
        qWarning() << "RecordData::setR0Value: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR1000Value(float r1000)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R1000 = QString::number(r1000);
    } else {
        qWarning() << "RecordData::setR1000Value: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setR5000Value(float r5000)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].R5000 = QString::number(r5000);
    } else {
        qWarning() << "RecordData::setR5000Value: channel" << m_currentChanel << "not exists";
    }
}

R32Info RecordData::getR32Info(int channel) const
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

void RecordData::setCalibrationStatus(const QString &result)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].calStatus = result;
    } else {
        qWarning() << "RecordData::setCalibrationStatus: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setDetectPoint0(int concentration)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].r32Ccr0 = concentration;
        //m_recordData[m_currentChanel].ccr0 = getCurrentConcentration();
        // 当前浓度值在0的5%范围内，认为是有效值
        if (m_recordData[m_currentChanel].r32Ccr0 >= 0 && m_recordData[m_currentChanel].r32Ccr0 <= 50) {
            m_recordData[m_currentChanel].ccr0Valid = true;
        }
        // 判定结果
        m_recordData[m_currentChanel].checkIsValid();
    } else {
        qWarning() << "RecordData::setDetectPoint0: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::clearConcentrationCache()
{
    m_concentrationCache.clear();
}

void RecordData::setDetectPoint500(int concentration)
{{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].r32Ccr500 = concentration;
        //m_recordData[m_currentChanel].ccr500 = getCurrentConcentration();
        // 当前浓度值在500的5%范围内，认为是有效值
        if (m_recordData[m_currentChanel].r32Ccr500 >= 475 && m_recordData[m_currentChanel].r32Ccr500 <= 525) {
            m_recordData[m_currentChanel].ccr500Valid = true;
        }
    } else {
        qWarning() << "RecordData::setDetectPoint500: channel" << m_currentChanel << "not exists";
    }
}
}

void RecordData::setDetectPoint1000(int concentration)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].r32Ccr1000 = concentration;
        //m_recordData[m_currentChanel].ccr1000 = getCurrentConcentration();
        // 当前浓度值在1000的5%范围内，认为是有效值
        if (m_recordData[m_currentChanel].r32Ccr1000 >= 950 && m_recordData[m_currentChanel].r32Ccr1000 <= 1050) {
            m_recordData[m_currentChanel].ccr1000Valid = true;
        }
    } else {
        qWarning() << "RecordData::setDetectPoint1000: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setDetectPoint3000(int concentration)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].r32Ccr3000 = concentration;
        //m_recordData[m_currentChanel].ccr3000 = getCurrentConcentration();
        // 当前浓度值在3000的5%范围内，认为是有效值
        if (m_recordData[m_currentChanel].r32Ccr3000 >= 2850 && m_recordData[m_currentChanel].r32Ccr3000 <= 3150) {
            m_recordData[m_currentChanel].ccr3000Valid = true;
        }
    } else {
        qWarning() << "RecordData::setDetectPoint3000: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setDetectPoint5000(int concentration)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].r32Ccr5000 = concentration;
        //m_recordData[m_currentChanel].ccr5000 = getCurrentConcentration();
        // 当前浓度值在5000的5%范围内，认为是有效值
        if (m_recordData[m_currentChanel].r32Ccr5000 >= 4750 && m_recordData[m_currentChanel].r32Ccr5000 <= 5250) {
            m_recordData[m_currentChanel].ccr5000Valid = true;
        }
    } else {
        qWarning() << "RecordData::setDetectPoint5000: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setTemperatureAndHumidity(float temperature, float humidity)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].temperature = QString::number(temperature);
        m_recordData[m_currentChanel].humidity = QString::number(humidity);
    } else {
        qWarning() << "RecordData::setTemperatureAndHumidity: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setPValue(float p)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].p = QString::number(p);
    } else {
        qWarning() << "RecordData::setPValue: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setP1Value(float p1)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].p1 = QString::number(p1);
    } else {
        qWarning() << "RecordData::setP1Value: channel" << m_currentChanel << "not exists";
    }
}

void RecordData::setP2Value(float p2)
{
    if (m_recordData.contains(m_currentChanel)) {
        m_recordData[m_currentChanel].p2 = QString::number(p2);
    } else {
        qWarning() << "RecordData::setP2Value: channel" << m_currentChanel << "not exists";
    }
}

bool RecordData::hasData() const
{
    return !m_recordData.isEmpty();
}

void RecordData::clear()
{
    m_recordData.clear();
    m_currentChanel = 0;
    m_concentrationCache.clear();
}
