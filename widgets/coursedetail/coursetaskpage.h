#ifndef COURSETASKPAGE_H
#define COURSETASKPAGE_H

#include <QWidget>
#include <vector>

#include "../../models/task.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QVBoxLayout;

class CourseTaskPage : public QWidget
{
    Q_OBJECT

public:
    explicit CourseTaskPage(QWidget* parent = nullptr);

    void loadCourseTasks(const QString& courseName);

signals:
    void taskUpdated();
    void addTaskRequested(QString courseName);

private:
    QString currentCourseName;

    QLineEdit* searchEdit = nullptr;
    QComboBox* sortBox = nullptr;
    QCheckBox* hideCompletedBox = nullptr;

    QVBoxLayout* taskListLayout = nullptr;
    QLabel* summaryLabel = nullptr;

    std::vector<Task> currentTasks;
    std::vector<int> sourceIndices;

    QWidget* createToolbar();
    void loadTasksFromJson();
    void renderTasks();
    void openAddTaskDialog();
    void openEditTaskDialog(int sourceIndex);
};

#endif // COURSETASKPAGE_H