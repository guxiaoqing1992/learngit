#ifndef COMBOBUTTON_H
#define COMBOBUTTON_H

#include <QComboBox>


/*自定义combobox继承自QComboBox,
 *并重写wheelevent来实现combobox忽略鼠标滚轮事件，其他正常*/

class ComboButton : public QComboBox
{
public:
    ComboButton();

protected:
    void wheelEvent(QWheelEvent*){}
};

#endif // COMBOBUTTON_H
