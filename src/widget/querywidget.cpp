//
// Created by zyz on 2023/5/21.
//

#include "querywidget.h"
#include "sqls.h"
#include "connectionpool.h"
#include "pagewidget.h"
#include "r32recordvalue.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QSqlQueryModel>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFileDialog>
#include <QAxObject>

const int recordsPerPage = 10; // Number of records to display per page

const char * const SQL_R32TABLE = "R32Table";
QueryWidget::QueryWidget(QWidget *parent)
    : QWidget(parent)
    , m_startTimeEdit(new QDateTimeEdit(this))
    , m_endTimeEdit(new QDateTimeEdit(this))
    , m_concentrationEdit(new QLineEdit(this))
    , m_precisionEdit(new QLineEdit(this))
    , m_queryButton(new QPushButton("查询",this))
    , m_exportButton(new QPushButton("导出",this))
    , m_tableView(new QTableView(this))
    , m_pageWidget(new PageWidget(this))
//    , m_queryModel(new QSqlQueryModel(this))
{
    initUI();

    // 设置连接槽函数
    connect(m_queryButton, &QPushButton::clicked, this, &QueryWidget::handleQueryButton);
    connect(m_exportButton, &QPushButton::clicked, this, &QueryWidget::handleExportButton);

    // pageWidget
    connect(m_pageWidget, &PageWidget::previousPageRequested, [=](){
        if (m_currentPage > 1)
            loadPage(m_currentPage - 1);
    });
    connect(m_pageWidget, &PageWidget::nextPageRequested, [=](){
        if (m_currentPage < m_totalPages)
            loadPage(m_currentPage + 1);
    });
    connect(m_pageWidget, &PageWidget::goToPageRequested, [=](int page){
        if (page >= 1 && page <= m_totalPages)
            loadPage(page);
    });
}

QueryWidget::~QueryWidget()
{

}

void QueryWidget::initUI() {
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_startTimeEdit->setCalendarPopup(true);
    m_endTimeEdit->setCalendarPopup(true);
    m_startTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    m_endTimeEdit->setDateTime(QDateTime::currentDateTime());

    m_concentrationEdit->setMaximumWidth(40);
    m_precisionEdit->setMaximumWidth(40);

    auto *hLayout = new QHBoxLayout;
    hLayout->setSpacing(10);
    hLayout->addWidget(new QLabel("开始时间:"));
    hLayout->addWidget(m_startTimeEdit);
    hLayout->addWidget(new QLabel("结束时间:"));
    hLayout->addWidget(m_endTimeEdit);
    hLayout->addWidget(new QLabel("标准浓度值:"));
    hLayout->addWidget(m_concentrationEdit);
    hLayout->addWidget(new QLabel("精度值%:"));
    hLayout->addWidget(m_precisionEdit);
    hLayout->addStretch();
    hLayout->addWidget(m_queryButton);
    hLayout->addWidget(m_exportButton);

    auto *vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_tableView);
    vLayout->addWidget(m_pageWidget);
}

void QueryWidget::handleQueryButton()
{
    int totalCount = canQueryDataCount();
    if (totalCount == 0) {
        return;
    }

    m_totalPages = (totalCount + recordsPerPage - 1) / recordsPerPage;
    m_pageWidget->setTotalPage(m_totalPages);
    m_pageWidget->setPageNumber(1);

    QString startTime = m_startTimeEdit->text();
    QString endTime = m_endTimeEdit->text();

    loadPage(1, totalCount, startTime, endTime);
}

# if 0
void QueryWidget::handleQueryButton() {

    m_queryModel->clear();

    QString startTime = m_startTimeEdit->text();
    QString endTime = m_endTimeEdit->text();
    QString concentration = m_concentrationEdit->text();
    QString precision = m_precisionEdit->text();

    // 检查时间范围是否为两天内
    QDateTime startDateTime = QDateTime::fromString(startTime, "yyyy-MM-dd hh:mm:ss");
    QDateTime endDateTime = QDateTime::fromString(endTime, "yyyy-MM-dd hh:mm:ss");
    if (startDateTime.daysTo(endDateTime) > 2) {
        QMessageBox::warning(this, "错误", "时间范围必须在两天之内");
        return;
    }

    QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "findByDateTime");
    sql = sql.arg(startTime).arg(endTime);
    qDebug() << "sql:" << sql;

    QSqlDatabase db = Singleton<ConnectionPool>::getInstance().openConnection();
    QSqlQuery query(db);
    query.prepare(sql);
    if (!query.exec()) {
        Singleton<ConnectionPool>::getInstance().closeConnection(db);
        QMessageBox::warning(this, "查询失败", "查询失败");
        return;
    }

    // 设置查询结果到查询模型
    m_queryModel->setQuery(query);
    Singleton<ConnectionPool>::getInstance().closeConnection(db);
}
#endif

void QueryWidget::handleExportButton() {
    QString filename = QFileDialog::getSaveFileName(this, "选择保存路径", "", "Excel 文件 (*.xlsx)");
    if (filename.isEmpty()) {
        return;
    }

    int totalCount = canQueryDataCount();
    if (totalCount == 0) {
        return;
    }

    QString startTime = m_startTimeEdit->text();
    QString endTime = m_endTimeEdit->text();

    exportToExcel(filename, startTime, endTime);
}

void QueryWidget::exportToExcel(const QString &filename, const QString &startTime, const QString &endTime)
{
    auto getAllDataToModel = [this](QSqlQueryModel *model, const QString &startTime, const QString &endTime) {
        QString sql = Singleton<Sqls>::getInstance().getSql(SQL_R32TABLE, "findByDateTime");
        sql = sql.arg(startTime).arg(endTime);
        qDebug() << "sql:" << sql;

        QSqlDatabase db = Singleton<ConnectionPool>::getInstance().openConnection();
        QSqlQuery query(db);
        query.prepare(sql);
        if (!query.exec()) {
            Singleton<ConnectionPool>::getInstance().closeConnection(db);
            QMessageBox::warning(this, "获取数据失败", "获取查询数据失败！");
            return;
        }

        // 设置查询结果到查询模型
        model->setQuery(query);
        Singleton<ConnectionPool>::getInstance().closeConnection(db);
    };

    auto *excel = new QAxObject("Excel.Application", this);
    if (!excel) {
        QMessageBox::warning(this, "错误", "无法导出Excel文件，请检查是否安装Excel!");
        return;
    }

    auto *queryModel = new QSqlQueryModel(this);
    getAllDataToModel(queryModel, startTime, endTime);

    excel->dynamicCall("SetVisible(bool)", false);
    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Add");
    QAxObject *worksheets = workbook->querySubObject("Worksheets");
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);

    // 写入表头
    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", 1, 1);
    cell->setProperty("Value", "ID");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 2);
    cell->setProperty("Value", "流水号");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 3);
    cell->setProperty("Value", "时间");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 4);
    cell->setProperty("Value", "传感器ID");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 5);
    cell->setProperty("Value", "传感器地址");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 6);
    cell->setProperty("Value", "通道");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 7);
    cell->setProperty("Value", "点位");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 8);
    cell->setProperty("Value", "标定浓度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 9);
    cell->setProperty("Value", "温度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 10);
    cell->setProperty("Value", "湿度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 11);
    cell->setProperty("Value", "气体ADC");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 12);
    cell->setProperty("Value", "获取浓度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 13);
    cell->setProperty("Value", "结果信息");

    // 写入数据
    for (int row = 0; row < queryModel->rowCount(); ++row) {
        for (int column = 0; column < queryModel->columnCount(); ++column) {
            cell = worksheet->querySubObject("Cells(int,int)", row + 2, column + 1);
            cell->setProperty("Value", queryModel->data(queryModel->index(row, column)).toString());
        }

        // 计算结果信息
        QString concentration = queryModel->data(queryModel->index(row, 6)).toString();
        double userConcentration = m_concentrationEdit->text().toDouble();
        double precision = m_precisionEdit->text().toDouble();
        // 根据需要计算结果信息并写入单元格
        bool in = isWithinRange(concentration.toDouble(), userConcentration, precision);
        if (in) {
            cell = worksheet->querySubObject("Cells(int,int)", row + 2, 13);
            cell->setProperty("Value", "合格");
        } else {
            cell = worksheet->querySubObject("Cells(int,int)", row + 2, 13);
            cell->setProperty("Value", "不合格");
        }
    }

    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filename));
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");

    excel->deleteLater();
    queryModel->deleteLater();
}

bool QueryWidget::isWithinRange(double value, double standardValue, double precision) {
    qInfo() << "value:" << value << "standardValue:" << standardValue << "precision:" << precision;
    double difference = qAbs(value - standardValue);
    double percentDifference = (difference / standardValue) * 100;

    return percentDifference <= precision;
}

void QueryWidget::loadPage(int page, int totalCount, const QString &bTime, const QString &eTime)
{
    static int totalRecords = 0;
    static QString beginTime;
    static QString endTime;
    if (totalCount > 0) {
        totalRecords = totalCount;
        beginTime = bTime;
        endTime = eTime;
    }

    QSqlQueryModel *lastModel = m_queryModel;
    m_currentPage = page;

    QSqlDatabase db = Singleton<ConnectionPool>::getInstance().openConnection();
    QSqlQuery query(db);
    query.prepare("SELECT * FROM r32table WHERE dateTime BETWEEN :beginTime AND :endTime "
                  "ORDER BY dateTime OFFSET :offset ROWS FETCH NEXT :limit ROWS ONLY");
    query.bindValue(":beginTime", beginTime);
    query.bindValue(":endTime", endTime);
    query.bindValue(":offset", (page - 1) * recordsPerPage);
    query.bindValue(":limit", recordsPerPage);
    if (query.exec()) {
        m_queryModel = new QSqlQueryModel(this);
        setQueryModelHeader();
        m_queryModel->setQuery(query);
        m_tableView->setModel(m_queryModel);
    }

    Singleton<ConnectionPool>::getInstance().closeConnection(db);
    m_pageWidget->setPageNumber(page);

    if (lastModel) {
        lastModel->deleteLater();
    }
}

int QueryWidget::canQueryDataCount()
{
    QString startTime = m_startTimeEdit->text();
    QString endTime = m_endTimeEdit->text();
    QString concentration = m_concentrationEdit->text();
    QString precision = m_precisionEdit->text();

    // 检查时间范围是否为两天内
    QDateTime startDateTime = QDateTime::fromString(startTime, "yyyy-MM-dd hh:mm:ss");
    QDateTime endDateTime = QDateTime::fromString(endTime, "yyyy-MM-dd hh:mm:ss");
    if (startDateTime.daysTo(endDateTime) > 2) {
        QMessageBox::warning(this, "错误", "时间范围必须在两天之内");
        return 0;
    }

    int totalCount = R32RecordValueDao::datasCountByDateTime(startTime, endTime);
    qInfo() << "totalCount:" << totalCount;
    if (totalCount == 0) {
        QMessageBox::warning(this, "错误", "没有查询到数据");
        return 0;
    }

    return totalCount;
}

void QueryWidget::setQueryModelHeader()
{
    m_queryModel->setHeaderData(0, Qt::Horizontal, "id");
    m_queryModel->setHeaderData(1, Qt::Horizontal, "流水号");
    m_queryModel->setHeaderData(2, Qt::Horizontal, "时间");
    m_queryModel->setHeaderData(3, Qt::Horizontal, "传感器id");
    m_queryModel->setHeaderData(4, Qt::Horizontal, "传感器地址");
    m_queryModel->setHeaderData(5, Qt::Horizontal, "通道");
    m_queryModel->setHeaderData(6, Qt::Horizontal, "点位");
    m_queryModel->setHeaderData(7, Qt::Horizontal, "标定浓度");
    m_queryModel->setHeaderData(8, Qt::Horizontal, "温度");
    m_queryModel->setHeaderData(9, Qt::Horizontal, "湿度");
    m_queryModel->setHeaderData(10, Qt::Horizontal, "气体ADC");
    m_queryModel->setHeaderData(11, Qt::Horizontal, "获取浓度");
}
