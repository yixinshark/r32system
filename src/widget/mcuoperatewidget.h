//
// Created by yixin on 2023/7/16.
//

#ifndef R32SYSTEM_MCUOPERATEWIDGET_H
#define R32SYSTEM_MCUOPERATEWIDGET_H

#include "operatebasewidget.h"

class McuOperateWidget : public OperateBaseWidget
{
    Q_OBJECT
public:
    explicit McuOperateWidget(HandleDataBase *handleData, const QString &title, QWidget *parent = nullptr);
    ~McuOperateWidget() override;

protected:
    void recvAckData(quint8 cmd, const QVariantMap &info) override;

private:
    void initUI();
};

#endif //R32SYSTEM_MCUOPERATEWIDGET_H
