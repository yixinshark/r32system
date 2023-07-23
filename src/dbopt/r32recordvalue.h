//
// Created by zyz on 2023/5/20.
//

#ifndef R32_R32RECORDVALUE_H
#define R32_R32RECORDVALUE_H

#include <QString>
#include <QVariant>
#include <QDate>
#include "recorddata.h"

class R32RecordValueDao
{
public:
    static bool createTable();
    static int datasCountByDateTime(const QString &beginTime, const QString &endTime);
    static int insert(const R32Info &data);
    static QList<R32Info> queryByDateTime(const QString &beginTime, const QString &endTime);
    static int deleteByDateTime(quint32);

    // sensor_id和point的组合是否存在
    static bool isExist(const QString &sensor_id, int point);

private:
    static R32Info mapToR32(const QMap<QString, QVariant> &rowMap);
    static QMap<QString, QVariant> parmsValue(const R32Info &data);
};

#endif //R32_R32RECORDVALUE_H
