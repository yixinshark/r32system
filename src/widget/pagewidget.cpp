//
// Created by yixin on 2023/7/14.
//

#include "pagewidget.h"

#include <QPushButton>
#include <QHBoxLayout>

PageWidget::PageWidget(QWidget *parent)
    : QWidget(parent)
    , m_pageNumberLabel(new QLabel(this))
    , m_pageTotalLabel(new QLabel(this))
    , m_pageNumberInput(new QLineEdit(this))
{
    auto *previousButton = new QPushButton("上一页", this);
    previousButton->setMaximumWidth(80);
    auto *nextButton= new QPushButton("下一页",this);
    nextButton->setMaximumWidth(80);
    m_pageNumberLabel->setMaximumWidth(40);
    m_pageNumberLabel->setText("第1页");
    m_pageNumberInput->setMaximumWidth(50);
    m_pageTotalLabel->setMaximumWidth(40);
    m_pageTotalLabel->setText("共1页");

    auto *hLayout = new QHBoxLayout;
    hLayout->setSpacing(10);
    hLayout->addStretch();
    hLayout->addWidget(previousButton);
    hLayout->addWidget(nextButton);
    hLayout->addWidget(m_pageNumberInput);
    hLayout->addWidget(m_pageNumberLabel);
    hLayout->addWidget(m_pageTotalLabel);
    setLayout(hLayout);

    connect(previousButton, &QPushButton::clicked, this, &PageWidget::previousPageRequested);
    connect(nextButton, &QPushButton::clicked, this, &PageWidget::nextPageRequested);
    connect(m_pageNumberInput, &QLineEdit::editingFinished, this, [this](){
        bool ok;
        int page = m_pageNumberInput->text().toInt(&ok);
        if (ok && page > 0 && page <= m_totalPage) {
            emit goToPageRequested(page);
        }
    });
}

PageWidget::~PageWidget()
{

}

void PageWidget::setPageNumber(int page)
{
    m_currentPage = page;
    m_pageNumberLabel->setText(QString("第%1页").arg(page));
}

void PageWidget::setTotalPage(int total)
{
    m_totalPage = total;
    m_pageTotalLabel->setText(QString("共%1页").arg(total));
}
