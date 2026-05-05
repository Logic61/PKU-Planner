#ifndef COURSEHEADERWIDGET_H
#define COURSEHEADERWIDGET_H

#include <QWidget>
#include "../../models/course.h"

class QLabel;
class QPushButton;

class CourseHeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CourseHeaderWidget(QWidget* parent = nullptr);

    void setCourse(const Course& course);

signals:
    void closeClicked();
    void addTaskClicked();
    void editClicked();
    void folderClicked();

private:
    QLabel* titleLabel;
    QLabel* subtitleLabel;

    QPushButton* addTaskBtn;
    QPushButton* editBtn;
    QPushButton* folderBtn;
};

#endif
