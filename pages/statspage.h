#ifndef STATSPAGE_H
#define STATSPAGE_H

#include <QWidget>

class StatsPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatsPage(QWidget *parent = nullptr);
    void refresh();
};

#endif // STATSPAGE_H
