//
// Created by yixin on 2023/7/16.
//

#include "mcuoperatewidget.h"

McuOperateWidget::McuOperateWidget(HandleDataBase *handleData, const QString &title, QWidget *parent)
    : OperateBaseWidget(handleData, parent)
{
    setTitle(title);
    initUI();
}

McuOperateWidget::~McuOperateWidget()
{

}

void McuOperateWidget::recvAckData(quint8 cmd, const QVariantMap &info)
{

}

void McuOperateWidget::initUI()
{
    OperateBaseWidget::initUI();
}
