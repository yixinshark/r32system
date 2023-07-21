//
// Created by yixin on 2023/6/12.
//

#ifndef R32_HANDLEANALYSERDATA_H
#define R32_HANDLEANALYSERDATA_H

#include "handledatabase.h"

#include <QTimer>

class SerialPortCom;
/*!
 * @brief R32分析仪器数据
 */
class HandleAnalyserData : public HandleDataBase
{
    Q_OBJECT
public:
    explicit HandleAnalyserData(SerialPortCom *serialPortCom, QObject *parent = nullptr);
    ~HandleAnalyserData() override;

public:
    void startPeriodTask(bool enable) override;
    void setAddress(char address);
    void processReceivedData(const QByteArray &data) override;

protected:
    void addContent(char cmd, const QVariantMap &info, QByteArray &data) override;
    bool frameIsValid(const QByteArray &frameData) override;

private:
    void handleContent(const QByteArray &content);

private:
    char m_address = 0x00;
    QTimer *m_timer = nullptr;
};

#endif //R32_HANDLEANALYSERDATA_H
