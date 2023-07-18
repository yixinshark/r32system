//
// Created by zyz on 23-5-12.
//

#ifndef R32_OPERATEWIDGET_H
#define R32_OPERATEWIDGET_H

#include <QWidget>


class QLabel;
class QTableWidget;
class ConnectWidget;
class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);
    ~OperateWidget() override;

private:
    void initUI();
    void showMsg(const QString &msg);

private:
    // r32传感器串口连接
    ConnectWidget *m_r32ConnectWidget;
    // r32分析仪串口连接
    ConnectWidget *m_r32AnaConnectWidget;
    // mcu串口连接
    ConnectWidget *m_mcuConnectWidget;

    QTableWidget *m_tableWidget;
    //QLabel *m_msgLabel;
};

#endif //R32_OPERATEWIDGET_H
