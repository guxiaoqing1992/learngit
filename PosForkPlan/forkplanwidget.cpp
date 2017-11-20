#include "forkplanwidget.h"
#include "ui_forkplanwidget.h"

#include <QScrollArea>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <database.h>
#include <QMessageBox>


/*
 * 函数名称：ForkPlanWidget::paintEvent
 * 函数参数：QPaintEvent *e
 * 函数返回值：void
 * 函数功能：界面的绘图事件，用于绘制界面横向的分割线
 *
 */
void ForkPlanWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter;
    painter.begin(this);
    painter.setPen(QColor(190, 190, 190));
    painter.drawLine(10, 35, 580, 35);
    painter.end();
}


/*
 * 函数名称：ForkPlanWidget::ForkPlanWidge
 * 函数参数：QWidget *parent
 * 函数返回值：无
 * 函数功能：构造函数。设置界面中控件的属性
 *
 */
ForkPlanWidget::ForkPlanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ForkPlanWidget)
{
    ui->setupUi(this);
    this->setFixedSize(590,690);
    ui->comboBox->setEnabled(false);
    ui->tableView->hide();
    ui->planStartTime->setInputMask("0000-00-00");
    ui->planEndTime->setInputMask("0000-00-00");
    ui->lineEdit->setFocus();
}



/*
 * 函数名称：ForkPlanWidget::~ForkPlanWidge
 * 函数参数：无
 * 函数返回值：无
 * 函数功能：析构函数
 *
 */
ForkPlanWidget::~ForkPlanWidget()
{
    delete ui;
}


/*
 * 函数名称：ForkPlanWidget::closeEvent
 * 函数参数：QCloseEvent *e
 * 函数返回值：void
 * 函数功能：关闭事件
 *
 */
void ForkPlanWidget::closeEvent(QCloseEvent *e)
{
    ui->partnerWidget->setRowCount(0);
    ui->partnerWidget->setColumnCount(0);
    ui->fileUrlLe->clear();
    ui->planDescLe->clear();
    ui->planNameLe->clear();
    ui->planStartTime->clear();
    ui->planEndTime->clear();
    ui->lineEdit->clear();
    e->accept();
}


/*
 * 函数名称：ForkPlanWidget::on_pushButton_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：新增计划按钮按下时触发的槽函数，将界面中用户输入的信息获取到，并插入到数据库plan表中
 *
 */
void ForkPlanWidget::on_pushButton_clicked()
{
    bool focus = ui->partnerWidget->isItemSelected(ui->partnerWidget->currentItem()); // 判断是否选中一行
    if(focus == false)
    {
        QMessageBox::warning(NULL, "警告", "请选择要新增计划的商户", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    int row1 = ui->partnerWidget->currentItem()->row(); // 当前选中行

    int id = ui->partnerWidget->item(row1,0)->text().toInt();
    QString planName = ui->planNameLe->text();
    QString planDesc = ui->planDescLe->text();
    QString planStartTime = ui->planStartTime->text();
    QString planEndTime = ui->planEndTime->text();
    QString fileUrl = ui->fileUrlLe->text();
    int planState = 0;

    if(id == 0 || planName == NULL || planDesc == NULL || planStartTime == NULL || planEndTime == NULL || fileUrl == NULL)
    {
        QMessageBox::warning(NULL, "警告", "数据不能为空", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    ui->textEdit->setText("insert into [Freemud_PosUpgrade].[dbo].[tUpdatePlan]"
                          "(PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,PlanState) "
                          "values (" + QString::number(id) + "," + planName + "," + planDesc + "," + planStartTime + "," + planEndTime + "," + fileUrl + "," + QString::number(planState) + ")");
    QSqlQuery query;
    bool res = query.exec(QObject::tr("insert into [Freemud_PosUpgrade].[dbo].[tUpdatePlan]"
                                      "(PartnerId,PlanName,PlanDesc,PlanStartTime,PlanEndTime,FileUrl,PlanState) "
                                      "values (%1,'%2','%3','%4','%5','%6',%7);")
                          .arg(id).arg(planName).arg(planDesc).arg(planStartTime)
                          .arg(planEndTime).arg(fileUrl).arg(planState));
    if(!res)
    {
        qDebug()<<query.lastError();
        QMessageBox::warning(NULL, "警告", "SQL执行出错", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QMessageBox::information(NULL, "信息", "新增计划成功", QMessageBox::Yes, QMessageBox::Yes);
        ui->partnerWidget->setRowCount(0);
        ui->partnerWidget->setColumnCount(0);
        ui->fileUrlLe->clear();
        ui->planDescLe->clear();
        ui->planNameLe->clear();
        ui->planStartTime->clear();
        ui->planEndTime->clear();
        ui->lineEdit->clear();
    }

}



/*
 * 函数名称：ForkPlanWidget::on_CheckBtn_clicked
 * 函数参数：无
 * 函数返回值：void
 * 函数功能：查询商户按钮按下时触发的槽函数，将数据库中store表中的数据展示到表格中以供选择，还可以直接输入商户编号来进行查询
 *
 */
void ForkPlanWidget::on_CheckBtn_clicked()
{
    int id = ui->lineEdit->text().toInt();

    QSqlQueryModel *model = new QSqlQueryModel;
    if(id == 0)
    {
        ui->textEdit->setText("select PartnerId,StoreID,CompanyCode,StoreName,StoreType,"
                              "(case  Enable when 0 then '无效' when 1 then '有效' else '错误' end) as Enable "
                              " from [Freemud_PosUpgrade].[dbo].[tStore] ");
        model->setQuery(QObject::tr("select PartnerId,StoreID,CompanyCode,StoreName,StoreType,"
                                    "(case  Enable when 0 then '无效' when 1 then '有效' else '错误' end) "
                                    " from [Freemud_PosUpgrade].[dbo].[tStore] "));


        ui->tableView->setModel(model);

        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        ui->partnerWidget->setRowCount(rowCount);
        ui->partnerWidget->setColumnCount(columnCount);
        ui->partnerWidget->setHorizontalHeaderLabels(QStringList() <<tr("商户号")<<tr("门店编号")<<tr("门店编码")<<tr("门店名称")<<tr("门店类型")<<tr("商户状态"));
        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);
                ui->partnerWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }

        ui->partnerWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->partnerWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->partnerWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->partnerWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    }
    else
    {
        ui->textEdit->setText("select PartnerId,StoreID,CompanyCode,StoreName,StoreType,"
                              "(case  Enable when 0 then '无效' when 1 then '有效' else '错误' end) as Enable "
                              " from [Freemud_PosUpgrade].[dbo].[tStore] "
                              "where PartnerId = " + QString::number(id));
        model->setQuery(QObject::tr("select PartnerId,StoreID,CompanyCode,StoreName,StoreType,"
                                    "(case  Enable when 0 then '无效' when 1 then '有效' else '错误' end) "
                                    " from [Freemud_PosUpgrade].[dbo].[tStore] "
                                    "where PartnerId = %1").arg(id));

        ui->tableView->setModel(model);
        int rowCount = model->rowCount();
        int columnCount = model->columnCount();
        if(rowCount == 0)
        {
            QMessageBox::warning(NULL, "警告", "输入的PartnerId错误!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        ui->partnerWidget->setRowCount(rowCount);
        ui->partnerWidget->setColumnCount(columnCount);
        ui->partnerWidget->setHorizontalHeaderLabels(QStringList() <<tr("商户号")<<tr("门店编号")<<tr("门店编码")<<tr("门店名称")<<tr("门店类型"));
        int i = 0,j = 0;
        for(i = 0;i < rowCount;i++)
        {
            for(j = 0;j < columnCount;j++)
            {
                QModelIndex index = ui->tableView->model()->index(i,j);
                QVariant data = ui->tableView->model()->data(index);
                ui->partnerWidget->setItem(i,j,new QTableWidgetItem(data.toString()));
            }
        }

        ui->partnerWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->partnerWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        ui->partnerWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->partnerWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    }
}
