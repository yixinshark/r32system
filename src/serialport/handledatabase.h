//
// Created by zyz on 23-6-1.
//

#ifndef R32_HANDLEDATABASE_H
#define R32_HANDLEDATABASE_H

#include <QObject>

class SerialPortCom;
class HandleDataBase : public QObject
{
    Q_OBJECT

public:
    explicit HandleDataBase(SerialPortCom *serialPortCom, QObject *parent = nullptr);
    ~HandleDataBase() override;

signals:
    void frameReceived(char cmd, const QVariantMap &info);
    void recvedFrameData(const QByteArray &frameData);

public:
    QString senderName() const { return m_senderName; }
    void setSenderName(const QString &senderName) { m_senderName = senderName; }
    virtual void sendCmdData(const QByteArray &data);
    // 开启周期任务
    virtual void startPeriodTask(bool enable) { }
    virtual QByteArray getSendData(char cmd, const QVariantMap &info);
    virtual void processReceivedData(const QByteArray &data) = 0;

protected:
    // 填充发送内容
    virtual void addContent(char cmd, const QVariantMap &info, QByteArray &data) = 0;
    virtual void addCheckSum(QByteArray &data);

    // 帧数据是否有效
    virtual bool frameIsValid(const QByteArray &frameData) = 0;

protected:
    QString m_senderName;
    QByteArray m_receivedData;
    SerialPortCom *m_serialPortCom;
};

#endif //R32_HANDLEDATABASE_H
