#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class SidebarWidget;
class TopbarWidget;
class DashboardPage;
class TodoPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void initPages();
    void onNavigateToTodoPage();

private:
    QStackedWidget *stack;
    SidebarWidget *sidebar;
    TopbarWidget *topbar;
    DashboardPage *dashboardPage = nullptr;
    TodoPage *todoPage = nullptr;
    bool pagesInitialized = false;
};

#endif