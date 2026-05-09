#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class SidebarWidget;
class TopbarWidget;
class DashboardPage;
class TodoPage;
class CourseDetailDrawer;
class MascotWidget;
#include "models/course.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void initPages();
    void onNavigateToTodoPage();
    void showCourseDrawer(const Course& course);
    void handleAddTaskRequested(Course course);
    void handleEditCourseRequested(Course course);
    void showMascotPopup();
    void onSearchCourseRequested(const QString& courseName);
    void onSearchTaskRequested(int taskIndex);

private:
    QStackedWidget *stack;
    SidebarWidget *sidebar;
    TopbarWidget *topbar;
    DashboardPage *dashboardPage = nullptr;
    TodoPage *todoPage = nullptr;
    CourseDetailDrawer *courseDrawer = nullptr;
    MascotWidget *mascotWidget = nullptr;
    bool pagesInitialized = false;
    QString searchCourseName;
};

#endif