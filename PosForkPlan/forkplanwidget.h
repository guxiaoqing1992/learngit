#ifndef FORKPLANWIDGET_H
#define FORKPLANWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QCloseEvent>
#include <database.h>

namespace Ui {
class ForkPlanWidget;
}

class ForkPlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ForkPlanWidget(QWidget *parent = 0);
    ~ForkPlanWidget();

private:
    Ui::ForkPlanWidget *ui;

    int partnerID;


protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void closeEvent(QCloseEvent *e);


private slots:
    void on_pushButton_clicked();
    void on_CheckBtn_clicked();

};

#endif // FORKPLANWIDGET_H
