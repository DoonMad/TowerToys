#ifndef MACROMANAGERWIDGET_H
#define MACROMANAGERWIDGET_H

#include <QWidget>

namespace Ui {
class MacroManagerWidget;
}

class MacroManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MacroManagerWidget(QWidget *parent = nullptr);
    ~MacroManagerWidget();

private:
    Ui::MacroManagerWidget *ui;
};

#endif // MACROMANAGERWIDGET_H
