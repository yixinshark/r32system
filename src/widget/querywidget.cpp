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
    , m_queryButton(new QPushButton("查询",this))
    , m_exportButton(new QPushButton("导出",this))
    , m_tableView(new QTableView(this))
    , m_pageWidget(new PageWidget(this))
    // , m_queryModel(new QSqlQueryModel(this))
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

    auto *hLayout = new QHBoxLayout;
    hLayout->setSpacing(10);
    hLayout->addWidget(new QLabel("开始时间:"));
    hLayout->addWidget(m_startTimeEdit);
    hLayout->addWidget(new QLabel("结束时间:"));
    hLayout->addWidget(m_endTimeEdit);
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

#if 0
    auto *excel = new QAxObject("Excel.Application", this);
    if (!excel) {
        QMessageBox::warning(this, "错误", "无法导出Excel文件，请检查是否安装Excel!");
        return;
    }
#endif

    auto *excel = new QAxObject(this);
    bool hasExcel = excel->setControl("Excel.Application");
    if (!hasExcel) {
        bool hasWps = excel->setControl("kwps.Application"); // WPS excel->setControl("ket.Application");
        if (!hasWps) {
            QMessageBox::warning(this, "错误", "无法导出Excel文件，请检查是否安装Excel或WPS!");
            return;
        }
    }

    // 参考:https://blog.csdn.net/weixin_43712770/article/details/103479749
    // https://blog.csdn.net/qq_40194498/article/details/122100123

    auto *queryModel = new QSqlQueryModel(this);
    getAllDataToModel(queryModel, startTime, endTime);

    excel->dynamicCall("SetVisible(bool)", false);
    QAxObject *workbooks = excel->querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Add");
    QAxObject *worksheets = workbook->querySubObject("Worksheets");
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);

    const char *strValue = "Value2"; // just for excel "Value"; Value2 for excel and wps

    // 写入表头
    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", 1, 1);
    cell->setProperty(strValue, "时间");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 2);
    cell->setProperty(strValue, "传感器ID");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 3);
    cell->setProperty(strValue, "判定结果");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 4);
    cell->setProperty(strValue, "ON/OFF");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 5);
    cell->setProperty(strValue, "软件版本");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 6);
    cell->setProperty(strValue, "标定点1");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 7);
    cell->setProperty(strValue, "标定点2");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 8);
    cell->setProperty(strValue, "标定点3");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 9);
    cell->setProperty(strValue, "标定状态");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 10);
    cell->setProperty(strValue, "标定的零点阻值R0");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 11);
    cell->setProperty(strValue, "1000PPM的阻值");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 12);
    cell->setProperty(strValue, "5000PPM的阻值");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 13);
    cell->setProperty(strValue, "参数p");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 14);
    cell->setProperty(strValue, "参数p1");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 15);
    cell->setProperty(strValue, "参数p2");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 16);
    cell->setProperty(strValue, "温度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 17);
    cell->setProperty(strValue, "湿度");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 18);
    cell->setProperty(strValue, "仪器浓度-5000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 19);
    cell->setProperty(strValue, "产品浓度-5000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 20);
    cell->setProperty(strValue, "仪器浓度-3000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 21);
    cell->setProperty(strValue, "产品浓度-3000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 22);
    cell->setProperty(strValue, "仪器浓度-1000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 23);
    cell->setProperty(strValue, "产品浓度-1000");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 24);
    cell->setProperty(strValue, "仪器浓度-500");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 25);
    cell->setProperty(strValue, "产品浓度-500");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 26);
    cell->setProperty(strValue, "仪器浓度-0");
    cell = worksheet->querySubObject("Cells(int,int)", 1, 27);
    cell->setProperty(strValue, "产品浓度-0");

    // 写入数据
    for (int row = 0; row < queryModel->rowCount(); ++row) {
        // 从第一列开始，去掉数据库自增id列
        for (int column = 1; column < queryModel->columnCount(); ++column) {
            cell = worksheet->querySubObject("Cells(int,int)", row + 2, column);
            cell->setProperty(strValue, queryModel->data(queryModel->index(row, column)).toString());
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
        m_queryModel->setQuery(query);
        setQueryModelHeader();
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
    m_queryModel->setHeaderData(0, Qt::Horizontal, "数据库id");
    m_queryModel->setHeaderData(1, Qt::Horizontal, "时间");
    m_queryModel->setHeaderData(2, Qt::Horizontal, "传感器ID");
    m_queryModel->setHeaderData(3, Qt::Horizontal, "判定结果");
    m_queryModel->setHeaderData(4, Qt::Horizontal, "ON/OFF");
    m_queryModel->setHeaderData(5, Qt::Horizontal, "软件版本");
    m_queryModel->setHeaderData(6, Qt::Horizontal, "标定点1");
    m_queryModel->setHeaderData(7, Qt::Horizontal, "标定点2");
    m_queryModel->setHeaderData(8, Qt::Horizontal, "标定点3");
    m_queryModel->setHeaderData(9, Qt::Horizontal, "标定状态");
    m_queryModel->setHeaderData(10, Qt::Horizontal, "标定的零点阻值R0");
    m_queryModel->setHeaderData(11, Qt::Horizontal, "1000PPM的阻值");
    m_queryModel->setHeaderData(12, Qt::Horizontal, "5000PPM的阻值");
    m_queryModel->setHeaderData(13, Qt::Horizontal, "参数p");
    m_queryModel->setHeaderData(14, Qt::Horizontal, "参数p1");
    m_queryModel->setHeaderData(15, Qt::Horizontal, "参数p2");
    m_queryModel->setHeaderData(16, Qt::Horizontal, "温度");
    m_queryModel->setHeaderData(17, Qt::Horizontal, "湿度");
    m_queryModel->setHeaderData(18, Qt::Horizontal, "仪器浓度-5000");
    m_queryModel->setHeaderData(19, Qt::Horizontal, "产品浓度-5000");
    m_queryModel->setHeaderData(20, Qt::Horizontal, "仪器浓度-3000");
    m_queryModel->setHeaderData(21, Qt::Horizontal, "产品浓度-3000");
    m_queryModel->setHeaderData(22, Qt::Horizontal, "仪器浓度-1000");
    m_queryModel->setHeaderData(23, Qt::Horizontal, "产品浓度-1000");
    m_queryModel->setHeaderData(24, Qt::Horizontal, "仪器浓度-500");
    m_queryModel->setHeaderData(25, Qt::Horizontal, "产品浓度-500");
    m_queryModel->setHeaderData(26, Qt::Horizontal, "仪器浓度-0");
    m_queryModel->setHeaderData(27, Qt::Horizontal, "产品浓度-0");
}