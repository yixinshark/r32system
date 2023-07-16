//
// Created by yixin on 2023/7/16.
//

#ifndef R32SYSTEM_R32ANALYTICALWIDGET_H
#define R32SYSTEM_R32ANALYTICALWIDGET_H

#include "operatebasewidget.h"

/*!
 * @brief R32分析仪界面
 */
class R32AnalyticalWidget : public OperateBaseWidget
{
    Q_OBJECT
public:
    explicit R32AnalyticalWidget(HandleDataBase *handleData, const QString &title, QWidget *parent = nullptr);
    ~R32AnalyticalWidget() override;

protected:
    void recvAckData(quint8 cmd, const QVariantMap &info) override;

private:
    void initUI();
};

#endif //R32SYSTEM_R32ANALYTICALWIDGET_H
