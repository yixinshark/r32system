//
// Created by yixin on 2023/5/20.
//

#include "r32recordvalue.h"
#include "dbutil.h"
#include "sqls.h"

const char * const SQL_R32TABLE = "R32Table";

QList<R32Info> R32RecordValueDao::queryByDateTime(const QString &beginTime, const QString &endTime) {
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

int R32RecordValueDao::insert(const R32Info &data) {
    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "insert");
    qInfo() << "sql:" << sql;

    QVariantMap params = parmsValue(data);
    return DBUtil::insert(sql, params);
}

R32Info R32RecordValueDao::mapToR32(const QMap<QString, QVariant> &rowMap) {
    R32Info data;
    data.dateTime = rowMap.value("dateTime").toString();
    data.channel = rowMap.value("sensor_id").toInt();
    data.valid = rowMap.value("valid").toString() == "合格";
    data.on = rowMap.value("on_off").toString() == "ON";
    data.softVersion = rowMap.value("soft_version").toString();
    data.point1 = rowMap.value("point1").toInt();
    data.point2 = rowMap.value("point2").toInt();
    data.point3 = rowMap.value("point3").toInt();
    data.calStatus = rowMap.value("cal_status").toString();
    data.R0 = rowMap.value("R0").toFloat();
    data.R1000 = rowMap.value("R1000").toFloat();
    data.R5000 = rowMap.value("R5000").toFloat();
    data.p = rowMap.value("p").toFloat();
    data.p1 = rowMap.value("p1").toFloat();
    data.p2 = rowMap.value("p2").toFloat();
    data.temperature = rowMap.value("temperature").toFloat();
    data.humidity = rowMap.value("humidity").toFloat();
    data.ccr5000 = rowMap.value("ccr5000").toInt();
    data.r32Ccr5000 = rowMap.value("r32Ccr5000").toInt();
    data.ccr3000 = rowMap.value("ccr3000").toInt();
    data.r32Ccr3000 = rowMap.value("r32Ccr3000").toInt();
    data.ccr1000 = rowMap.value("ccr1000").toInt();
    data.r32Ccr1000 = rowMap.value("r32Ccr1000").toInt();
    data.ccr500 = rowMap.value("ccr500").toInt();
    data.r32Ccr500 = rowMap.value("r32Ccr500").toInt();
    data.ccr0 = rowMap.value("ccr0").toInt();
    data.r32Ccr0 = rowMap.value("r32Ccr0").toInt();

    return data;
}

QMap<QString, QVariant> R32RecordValueDao::parmsValue(const R32Info &data) {
    QVariantMap info;
    info.insert("dateTime", data.dateTime);
    info.insert("sensor_id", QString::number(data.channel, 16));
    info.insert("valid", data.valid ? "合格":"不合格");
    info.insert("on_off", data.on ? "ON":"OFF");
    info.insert("soft_version", data.softVersion);
    info.insert("point1", data.point1);
    info.insert("point2", data.point2);
    info.insert("point3", data.point3);
    info.insert("cal_status", data.calStatus);
    info.insert("R0", data.R0);
    info.insert("R1000", data.R1000);
    info.insert("R5000", data.R5000);
    info.insert("p", data.p);
    info.insert("p1", data.p1);
    info.insert("p2", data.p2);
    info.insert("temperature", data.temperature);
    info.insert("humidity", data.humidity);
    info.insert("ccr5000", data.ccr5000);
    info.insert("r32Ccr5000", data.r32Ccr5000);
    info.insert("ccr3000", data.ccr3000);
    info.insert("r32Ccr3000", data.r32Ccr3000);
    info.insert("ccr1000", data.ccr1000);
    info.insert("r32Ccr1000", data.r32Ccr1000);
    info.insert("ccr500", data.ccr500);
    info.insert("r32Ccr500", data.r32Ccr500);
    info.insert("ccr0", data.ccr0);
    info.insert("r32Ccr0", data.r32Ccr0);

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
