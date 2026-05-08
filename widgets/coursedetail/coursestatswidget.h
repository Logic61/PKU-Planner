#ifndef COURSESTATSWIDGET_H
#define COURSESTATSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../../models/course.h"
#include <QLabel>
#include <QProgressBar>

class CourseStatsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CourseStatsWidget(QWidget *parent = nullptr);
    void loadCourse(const Course &course);

private:
    QString currentCourseName;
    QLabel *completionPercentLabel;
    QLabel *completionLabel;
    QLabel *completedTasksLabel;
    QLabel *pendingTasksLabel;
    QLabel *overdueTasksLabel;
    QLabel *avgTimeLabel;
    QLabel *onTimeRateLabel;
    QProgressBar *progressBar;

    QFrame* createStatItem(const QString& title, const QString& value, const QString& color);
    void refreshStats();
};

#endif // COURSESTATSWIDGET_H