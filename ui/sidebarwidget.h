#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QLabel>
#include "../models/mascotstate.h"

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget *parent = nullptr);

signals:
    void pageChanged(int index);
    void mascotClicked();

public slots:
    void onMascotClicked() { emit mascotClicked(); }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *mascotLabel = nullptr;
};

#endif