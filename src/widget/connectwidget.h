//
// Created by yixin on 2023/7/18.
//

#ifndef R32SYSTEM_CONNECTWIDGET_H
#define R32SYSTEM_CONNECTWIDGET_H

#include <QWidget>

#include "serialportsettingsdialog.h"

class QPushButton;
class StatusWidget;
class QComboBox;
class SerialPortCom;

/*!
 * \brief 串口连接界面
 */
class ConnectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectWidget(const QString &title, QWidget *parent = nullptr);
    ~ConnectWidget() override;

    SerialPortCom *serialPortCom() const { return m_serialPortCom; }
    bool isConnected() const;

    void setSerialPortParams(const SerialPortParams &serialPortParams);

signals:
    void operatedMsg(const QString &msg);
    void serialPortClosed();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    bool connectSerialPort();
    void updateAvailablePorts() const;

private:
    QPushButton *m_connectBtn;
    QPushButton *m_paramSettingsBtn;
    StatusWidget *m_cntStatusWidget;
    QComboBox *m_portComboBox;
    SerialPortCom *m_serialPortCom;

    QString m_title;
    SerialPortParams m_serialPortParams;
};

#endif //R32SYSTEM_CONNECTWIDGET_H
