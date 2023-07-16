//
// Created by yixin on 2023/7/14.
//

#ifndef R32_PAGEWIDGET_H
#define R32_PAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

class PageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageWidget(QWidget *parent = nullptr);
    ~PageWidget() override;

    void setPageNumber(int page);
    void setTotalPage(int total);

signals:
    void previousPageRequested();
    void nextPageRequested();
    void goToPageRequested(int page);

private:
    QLabel *m_pageNumberLabel;
    QLabel *m_pageTotalLabel;
    QLineEdit *m_pageNumberInput;
    int m_currentPage = 1;
    int m_totalPage = 1;
};

#endif //R32_PAGEWIDGET_H
