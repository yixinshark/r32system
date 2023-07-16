//
// Created by zyz on 2023/5/20.
//

#ifndef R32_R32RECORDVALUE_H
#define R32_R32RECORDVALUE_H

#include <QString>
#include <QVariant>
#include <QDate>

class R32RecordValue
{
public:
    // 流水号
    QString serial_number;
    QString dateTime;
    // 传感器ID
    QString sensor_id;
    // 传感器地址
    QString sensor_addr;
    // 通道
    QString channel;
    // 点位
    int point = 0;
    // 标定浓度
    QString cal_concentration;
    // 温度
    QString temperature;
    // 湿度
    QString humidity;
    // 气体ADC
    QString gas_adc;
    // 获取浓度 定义为float，存数据库会出错
    QString get_concentration;

    void reset() {
        serial_number.clear();
        dateTime.clear();
        sensor_id.clear();
        sensor_addr.clear();
        channel.clear();
        point = 0;
        cal_concentration.clear();
        temperature.clear();
        humidity.clear();
        gas_adc.clear();
        get_concentration.clear();
    }

    // 生成流水号
    bool generateSerialNumber(int batch) {
        if (point == 0)
            return false;
        // 流水号由 日期 + 批次号 + 通道 + 点位 组成
        // 获取当前日期
        QDate currentDate = QDate::currentDate();
        QString dateString = currentDate.toString("yyyyMMdd");
        // 生成批次号（4位）
        QString batchString = QString("%1").arg(batch, 4, 10, QChar('0'));
        // 生成通道号（2位）
        QString channelString = QString("%1").arg(channel.toInt(), 2, 10, QChar('0'));
        // 生成点位号（2位）
        QString pointString = QString("%1").arg(point, 2, 10, QChar('0'));
        // 组合流水号
        serial_number = dateString + batchString + channelString + pointString;
        return true;
    }

    QString toString() {
        return QString("serial_number:%1, dateTime:%2, sensor_id:%3,"
                       " sensor_addr:%4, channel:%5, point:%6,"
                       " cal_concentration:%7, temperature:%8,"
                       " humidity:%9, gas_adc:%10, get_concentration:%11")
                .arg(serial_number).arg(dateTime).arg(sensor_id).
                arg(sensor_addr).arg(channel).arg(point).arg(cal_concentration)
                .arg(temperature).arg(humidity).arg(gas_adc).arg(get_concentration);
    }
};

class R32RecordValueDao
{
public:
    static bool createTable();
    static int datasCountByDateTime(const QString &beginTime, const QString &endTime);
    static bool insertOrUpdate(const R32RecordValue &data);
    static int insert(const R32RecordValue &data);
    static bool update(const R32RecordValue &data);
    static QList<R32RecordValue> queryByDateTime(const QString &beginTime, const QString &endTime);
    static int deleteByDateTime(quint32);

    // sensor_id和point的组合是否存在
    static bool isExist(const QString &sensor_id, int point);

private:
    static R32RecordValue mapToR32(const QMap<QString, QVariant> &rowMap);
    static QMap<QString, QVariant> parmsValue(const R32RecordValue &data);
};

#endif //R32_R32RECORDVALUE_H
