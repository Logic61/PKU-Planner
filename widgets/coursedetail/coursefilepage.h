#ifndef COURSEFILEPAGE_H
#define COURSEFILEPAGE_H

#include <QWidget>
#include "../../models/course.h"

class QLabel;
class QListWidget;
class QPushButton;
class EmptyStateWidget;

class CourseFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit CourseFilePage(QWidget* parent=nullptr);

    void loadCourse(const Course& course);

signals:
    void courseUpdated(Course updatedCourse);

private:
    Course currentCourse;

    QLabel* pathLabel;
    QListWidget* fileList;

    QPushButton* bindFolderBtn;
    QPushButton* openFolderBtn;
    QPushButton* newNoteBtn;
    QPushButton* recordBtn;

    void refreshFileList();
    void bindFolder();
    void openFolder();
    void createNote();
    void showRecordingPlaceholder();
    EmptyStateWidget* emptyStateWidget = nullptr;
};

#endif // COURSEFILEPAGE_H