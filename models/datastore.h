#pragma once

#include <QObject>
#include <QList>
#include "course.h"
#include "task.h"

// High‑cohesion interface for the in‑memory store
class IDataStore {
public:
    virtual ~IDataStore() = default;
    virtual QList<Course> courses() const = 0;
    virtual void addCourse(const Course& c) = 0;
    virtual void updateCourse(int index, const Course& c) = 0;
    virtual void deleteCourse(int index) = 0;

    virtual QList<Task> tasks() const = 0;
    virtual void addTask(const Task& t) = 0;
    virtual void updateTask(int index, const Task& t) = 0;
    virtual void deleteTask(int index) = 0;
    virtual void markTaskCompleted(int index, bool completed) = 0;
};

class DataStore : public QObject, public IDataStore {
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = nullptr);
    // IDataStore implementation
    QList<Course> courses() const override;
    void addCourse(const Course& c) override;
    void updateCourse(int index, const Course& c) override;
    void deleteCourse(int index) override;

    QList<Task> tasks() const override;
    void addTask(const Task& t) override;
    void updateTask(int index, const Task& t) override;
    void deleteTask(int index) override;
    void markTaskCompleted(int index, bool completed) override;

    // Additional methods used by DataManager
    void clear() { m_courses.clear(); m_tasks.clear(); emit coursesChanged(); emit tasksChanged(); }
    void setCourses(const QList<Course>& c) { m_courses = c; emit coursesChanged(); }
    void setTasks(const QList<Task>& t) { m_tasks = t; emit tasksChanged(); }

signals:
    void coursesChanged();
    void tasksChanged();

private:
    QList<Course> m_courses;
    QList<Task> m_tasks;
};