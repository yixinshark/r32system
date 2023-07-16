//
// Created by yixin on 2023/7/16.
//

#ifndef R32SYSTEM_OPERATER32WIDGET_H
#define R32SYSTEM_OPERATER32WIDGET_H

#include "operatebasewidget.h"

/*!
 * @brief R32传感器操作界面
 */
class OperateR32Widget : public OperateBaseWidget
{
    Q_OBJECT
public:
    explicit OperateR32Widget(HandleDataBase *handleData, const QString &title, QWidget *parent = nullptr);
    ~OperateR32Widget() override;

protected:
    void recvAckData(quint8 cmd, const QVariantMap &info) override;

private:
    void initUI();
};

#endif //R32SYSTEM_OPERATER32WIDGET_H
