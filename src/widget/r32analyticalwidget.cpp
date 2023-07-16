//
// Created by yixin on 2023/7/16.
//

#include "r32analyticalwidget.h"

R32AnalyticalWidget::R32AnalyticalWidget(HandleDataBase *handleData, const QString &title, QWidget *parent)
    : OperateBaseWidget(handleData, parent)
{
    setTitle(title);
    initUI();
}

R32AnalyticalWidget::~R32AnalyticalWidget()
{

}

void R32AnalyticalWidget::recvAckData(quint8 cmd, const QVariantMap &info)
{

}

void R32AnalyticalWidget::initUI()
{
    OperateBaseWidget::initUI();
}
