//
// Created by yixin on 2023/7/16.
//

#include "operater32widget.h"

OperateR32Widget::OperateR32Widget(HandleDataBase *handleData, const QString &title, QWidget *parent)
    : OperateBaseWidget(handleData, parent)
{
    setTitle(title);
    initUI();
}

OperateR32Widget::~OperateR32Widget()
{

}

void OperateR32Widget::recvAckData(quint8 cmd, const QVariantMap &info)
{

}

void OperateR32Widget::initUI()
{
    OperateBaseWidget::initUI();
}
