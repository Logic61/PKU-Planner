#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;
class QHBoxLayout;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

private:
    QGridLayout *grid;

    QLabel *weekLabel;
    QLabel *timeLabel;
    QProgressBar *semesterProgress;

    int currentWeek = 9;
    int realWeek = 9;

    void initGrid();
    QWidget* createTopBar();
    QWidget* createBottomStats();
    QWidget* createRightPanel();

    void updateWeekInfo();
};

#endif