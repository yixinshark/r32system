//
// Created by zyz on 2023/5/21.
//

#ifndef R32_QUERYWIDGET_H
#define R32_QUERYWIDGET_H

#include <QWidget>

class QDateTimeEdit;
class QLineEdit;
class QPushButton;
class QTableView;
class QSqlQueryModel;
class PageWidget;

class QueryWidget : public QWidget {
Q_OBJECT

public:
    explicit QueryWidget(QWidget *parent = nullptr);
    ~QueryWidget() override;

private slots:
    void handleQueryButton();
    void handleExportButton();

private:
    void initUI();
    void loadPage(int page, int totalCount = 0, const QString &beginTime= "", const QString &endTime = "");
    void exportToExcel(const QString &filename, const QString &beginTime, const QString &endTime);
    int canQueryDataCount();
    bool isWithinRange(double value, double standardValue, double precision);
    void setQueryModelHeader();

private:
    QDateTimeEdit *m_startTimeEdit;
    QDateTimeEdit *m_endTimeEdit;
    QPushButton *m_queryButton;
    QPushButton *m_exportButton;
    QTableView *m_tableView;
    PageWidget *m_pageWidget;

    QSqlQueryModel *m_queryModel = nullptr;

    int m_currentPage = 1;
    int m_totalPages = 0;

};

#endif //R32_QUERYWIDGET_H
