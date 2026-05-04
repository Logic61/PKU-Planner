#ifndef TASKCARDWIDGET_H
#define TASKCARDWIDGET_H

#include <QFrame>
#include "../models/task.h"

class QLabel;
class QCheckBox;
class QPushButton;

class TaskCardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit TaskCardWidget(const Task &task, QWidget *parent = nullptr);

    void setTask(const Task &task);
    Task task() const;

signals:
    void completed(const Task &task);
    void edited(const Task &task);
    void deleted(const Task &task);

private:
    QString priorityText() const;
    QString countdownText() const;
    QString priorityColor() const;
    void updateVisualState();

    Task m_task;
    QLabel *courseTag = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *deadlineLabel = nullptr;
    QLabel *priorityBadge = nullptr;
    QLabel *statusBadge = nullptr;
    QCheckBox *doneBox = nullptr;
    QPushButton *completeBtn = nullptr;
};

#endif
