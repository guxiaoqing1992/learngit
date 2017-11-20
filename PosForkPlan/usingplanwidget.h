#ifndef USINGPLANWIDGET_H
#define USINGPLANWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <database.h>
#include <QVector>

namespace Ui {
class UsingPlanWidget;
}

class UsingPlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsingPlanWidget(QWidget *parent = 0);
    ~UsingPlanWidget();

private:
    Ui::UsingPlanWidget *ui;
    int partnerId;
    int planId;
    QVector<int> UpdatePosId;

signals:
    void widgetOpen();
    void sendUpdatePosId(QVector<int>);
private slots:
    void on_pushButton_clicked();
    void onSendPartnerId(int,int);
    void on_StartBtn_clicked();
    void on_AllChooseBtn_clicked();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void paintEvent(QPaintEvent*);
};

#endif // USINGPLANWIDGET_H
