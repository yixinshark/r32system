//
// Created by yixin on 2023/7/19.
//

#ifndef R32SYSTEM_RECORDDATA_H
#define R32SYSTEM_RECORDDATA_H

#include <QString>
#include <QMap>
#include <QDateTime>

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
    bool ccr5000Valid = false;
    // 3000浓度
    int r32Ccr3000 = 0;
    int ccr3000 = 0;
    bool ccr3000Valid = false;
    // 1000浓度
    int r32Ccr1000 = 0;
    int ccr1000 = 0;
    bool ccr1000Valid = false;
    // 500浓度
    int r32Ccr500 = 0;
    int ccr500 = 0;
    bool ccr500Valid = false;
    // 0浓度
    int r32Ccr0 = 0;
    int ccr0 = 0;
    bool ccr0Valid = false;

    bool valid= false; // 判定结果
    // 标定状态
    QString calStatus;
    QString softVersion;
    QString dateTime;

    void checkIsValid() {
        dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        valid = (point1Valid && point2Valid && point3Valid && ccr0Valid
                && ccr500Valid && ccr1000Valid && ccr3000Valid && ccr5000Valid);
    }
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
    void clearConcentrationCache();
    bool hasData() const;

    int recordDataCount() const;
    bool hasChannel(int channel) const;
    const R32Info &getR32Info(int channel) const;

    // 存储当前分析仪浓度值
    void setCurrentConcentration(int concentration);
    // 获取当前分析仪浓度值
    int getCurrentConcentration() const;

    int getcurrentChanel() const { return m_currentChanel; }
    void setCurrentChannel(int channel) { m_currentChanel = channel; }
    void setModuleAddress(int addr);
    void setFirmwareVersion(const QString &version);
    void setCalPoint(int point);
    void setR0Value(float r0);
    void setR1000Value(float r1000);
    void setR5000Value(float r5000);

    void setPValue(float p);
    void setP1Value(float p1);
    void setP2Value(float p2);
    void setTemperatureAndHumidity(float temperature, float humidity);

    void setCalibrationStatus(const QString &result);

    void setDetectPoint0(int concentration);
    void setDetectPoint500(int concentration);
    void setDetectPoint1000(int concentration);
    void setDetectPoint3000(int concentration);
    void setDetectPoint5000(int concentration);

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