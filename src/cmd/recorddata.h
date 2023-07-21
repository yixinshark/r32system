//
// Created by yixin on 2023/7/19.
//

#ifndef R32SYSTEM_RECORDDATA_H
#define R32SYSTEM_RECORDDATA_H

#include <QString>
#include <QMap>

struct R32Info {
    int channel = 0;
    int addr = 0;
    bool on = true; // on/off
    int point1 = 0;
    bool point1Valid = false;
    int point2 = 0;
    bool point2Valid = false;
    int point3 = 0;
    bool point3Valid = false;
    // 标定状态
    bool calStatus = false;
    float R0 = 0.0;
    float R1000 = 0.0;
    float R5000 = 0.0;
    float p = 0.0;
    float p1 = 0.0;
    float p2 = 0.0;
    // 温度
    float temperature = 0.0;
    // 湿度
    float humidity = 0.0;

    // 5000浓度
    int r32Ccr5000 = 0;
    int ccr5000 = 0;
    // 3000浓度
    int r32Ccr3000 = 0;
    int ccr3000 = 0;
    // 1000浓度
    int r32Ccr1000 = 0;
    int ccr1000 = 0;
    // 500浓度
    int r32Ccr500 = 0;
    int ccr500 = 0;
    // 0浓度
    int r32Ccr0 = 0;
    int ccr0 = 0;
    bool valid= false; // 判定结果
    QString softVersion;
};

/*!
 * \brief 记录数据，单例模式
 */
class RecordData
{
public:
    static RecordData *instance();

public:
    void reset();

    // 存储当前分析仪浓度值
    void setCurrentConcentration(int concentration);
    // 获取当前分析仪浓度值
    int getCurrentConcentration() const;

    void setCurrentChannel(int channel) { m_currentChanel = channel; }
    void setModuleAddress(int addr);
    void setFirmwareVersion(const QString &version);

private:
    RecordData();
    ~RecordData();

private:
    int m_currentChanel = 0;
    // 当前分析仪浓度值缓存
    QList<int> m_concentrationCache;
    QMap<int, R32Info> m_recordData;
};

#endif //R32SYSTEM_RECORDDATA_H
