#ifndef MASCOTWIDGET_H
#define MASCOTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include "../../models/mascotstate.h"

class MascotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MascotWidget(QWidget *parent = nullptr);

public slots:
    void showPopup();

private:
    void createPopup();
    bool eventFilter(QObject *obj, QEvent *event) override;

    QLabel *mascotLabel = nullptr;
    QFrame *popupFrame = nullptr;
    bool popupVisible = false;
};

#endif // MASCOTWIDGET_H