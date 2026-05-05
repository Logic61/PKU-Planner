#ifndef COURSESTATSWIDGET_H
#define COURSESTATSWIDGET_H

#include <QWidget>
#include "../../models/course.h"

class CourseStatsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CourseStatsWidget(QWidget *parent = nullptr);
    void loadCourse(const Course &course);
};

#endif // COURSESTATSWIDGET_H
