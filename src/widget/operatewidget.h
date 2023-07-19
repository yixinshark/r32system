//
// Created by zyz on 23-5-12.
//

#ifndef R32_OPERATEWIDGET_H
#define R32_OPERATEWIDGET_H

#include <QWidget>


class QLabel;
class QTextEdit;
class QPushButton;
class QTableWidget;
class ConnectWidget;
class ControlCmdFlow;

class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);
    ~OperateWidget() override;

private:
    void initUI();
    void initTableWidget();
    void showMsg(const QString &msg);
    void showOperateMsg(const QString &msg);

private:
    // r32传感器串口连接
    ConnectWidget *m_r32ConnectWidget;
    // r32分析仪串口连接
    ConnectWidget *m_r32AnaConnectWidget;
    // mcu串口连接
    ConnectWidget *m_mcuConnectWidget;

    QPushButton *m_startBtn;
    QTextEdit *m_optMsgLabel;
    QTableWidget *m_tableWidget;
    //QLabel *m_msgLabel;
    // 控制命令流程
    ControlCmdFlow *m_controlCmdFlow;

    QStringList m_msgList;
};

#endif //R32_OPERATEWIDGET_H
