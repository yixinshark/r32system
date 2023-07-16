//
// Created by yixin on 2023/5/20.
//

#include "r32recordvalue.h"
#include "dbutil.h"
#include "sqls.h"

const char * const SQL_R32TABLE = "R32Table";

QList<R32RecordValue> R32RecordValueDao::queryByDateTime(const QString &beginTime, const QString &endTime) {
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "findByDateTime");
    sql = sql.arg(beginTime).arg(endTime);
    qDebug() << "sql:" << sql;

    return DBUtil::selectBeans(mapToR32, sql);
}

int R32RecordValueDao::datasCountByDateTime(const QString &beginTime, const QString &endTime) {
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "datasCount");
    sql = sql.arg(beginTime).arg(endTime);

    int count = DBUtil::selectInt(sql);
    //qDebug() << "datasCount" << count;
    return count;
}

int R32RecordValueDao::deleteByDateTime(quint32 dateTime) {
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "deleteByDateTime");
    sql = sql.arg(dateTime);

    return DBUtil::update(sql);
}

int R32RecordValueDao::insert(const R32RecordValue &data) {
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "insert");
    qInfo() << "sql:" << sql;

    QVariantMap params = parmsValue(data);
    return DBUtil::insert(sql, params);
}

R32RecordValue R32RecordValueDao::mapToR32(const QMap<QString, QVariant> &rowMap) {
    R32RecordValue data;
    data.serial_number = rowMap.value("serial_number").toString();
    data.dateTime = rowMap.value("dateTime").toString();
    data.sensor_id = rowMap.value("sensor_id").toString();
    data.sensor_addr = rowMap.value("sensor_addr").toString();
    data.channel = rowMap.value("channel").toString();
    data.point = rowMap.value("point").toInt();
    data.cal_concentration = rowMap.value("cal_concentration").toString();
    data.temperature = rowMap.value("temperature").toString();
    data.humidity = rowMap.value("humidity").toString();
    data.gas_adc = rowMap.value("gas_adc").toString();
    data.get_concentration = rowMap.value("get_concentration").toString();

    return data;
}

QMap<QString, QVariant> R32RecordValueDao::parmsValue(const R32RecordValue &data) {
    QVariantMap info;
    info.insert("serial_number", data.serial_number);
    info.insert("dateTime", data.dateTime);
    info.insert("sensor_id", data.sensor_id);
    info.insert("sensor_addr", data.sensor_addr);
    info.insert("channel", data.channel);
    info.insert("point", data.point);
    info.insert("cal_concentration", data.cal_concentration);
    info.insert("temperature", data.temperature);
    info.insert("humidity", data.humidity);
    info.insert("gas_adc", data.gas_adc);
    info.insert("get_concentration", data.get_concentration);

    return info;
}

bool R32RecordValueDao::createTable()
{
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "CheckTable");
    qDebug() << "sql:" << sql;
    int count = DBUtil::selectInt(sql);
    if (count > 0) {
        qDebug() << "table R32Table is exist";
        return true;
    }

    sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "CreatTable");
    qDebug() << "sql:" << sql;
    int id = DBUtil::insert(sql);

    return (id != -1);
}

bool R32RecordValueDao::update(const R32RecordValue &data)
{
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "update");
    qDebug() << "sql:" << sql;

    QVariantMap params = parmsValue(data);
    return DBUtil::update(sql, params);
}

bool R32RecordValueDao::isExist(const QString &sensor_id, int point)
{
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "countBySensorIdAndPoint");
    sql = sql.arg(sensor_id).arg(point);
    qDebug() << "sql:" << sql;

    int count = DBUtil::selectInt(sql);
    if (count > 0) {
        return true;
    }

    return false;
}

bool R32RecordValueDao::insertOrUpdate(const R32RecordValue &data) {
    if (isExist(data.sensor_id, data.point)) {
        return update(data);
    } else {
        return insert(data) > 0;
    }
}
