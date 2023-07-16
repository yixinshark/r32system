//
// Created by zyz on 23-5-13.
//

#ifndef R32_OPERATEBASEWIDGET_H
#define R32_OPERATEBASEWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include <QSerialPort>

#include "serialportsettingsdialog.h"

class QPushButton;
class QLineEdit;

class StatusWidget;
class HandleDataBase;
class SerialPortCom;

/**
 * @brief 操作基类
 */
class OperateBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateBaseWidget(HandleDataBase *handleData, QWidget *parent = nullptr);
    ~OperateBaseWidget() override;

    void setTitle(const QString &title) { m_title = title; }

signals:
    void cmdCompleted(quint8 cmd);
    void operatedMsg(const QString &msg);
    void serialPortClosed();

protected:
    void initUI();
    bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    // 发送数据
    virtual void sendCmdData(quint8 cmd, const QVariantMap &info = QVariantMap());

    virtual void recvAckData(quint8 cmd, const QVariantMap &info) = 0;
    void operateMsg(const QString &msg);

private:
    bool connectSerialPort();
    void updateAvailablePorts() const;

protected:
    QPushButton *m_connectBtn;
    QPushButton *m_paramSettingsBtn;
    StatusWidget *m_cntStatusWidget;
    QComboBox *m_portComboBox;
    SerialPortCom *m_serialPortCom;
    HandleDataBase *m_handleData;

    QString m_title;
    SerialPortParams m_serialPortParams;
};

#endif //R32_OPERATEBASEWIDGET_H
