//
// Created by zyz on 23-5-12.
//

#ifndef R32_OPERATEWIDGET_H
#define R32_OPERATEWIDGET_H

#include <QWidget>

class OperateR32Widget;
class R32AnalyticalWidget;
class McuOperateWidget;
class QLabel;
class QTableWidget;

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
    OperateR32Widget *m_operateR32Widget;
    R32AnalyticalWidget *m_r32AnalyticalWidget;
    McuOperateWidget *m_mcuOperateWidget;

    QTableWidget *m_tableWidget;
    QLabel *m_msgLabel;
};

#endif //R32_OPERATEWIDGET_H
