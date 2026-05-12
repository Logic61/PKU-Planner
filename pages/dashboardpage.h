#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include "../services/iconfigprovider.h"
#include "../models/course.h"
#include "../models/task.h"
#include <vector>

class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;
class QHBoxLayout;
class EmptyStateWidget;
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QProgressDialog>

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(IConfigProvider *configProvider, QWidget *parent = nullptr);

public slots:
    void createCourse(int row, int col);
    void editCourse(int index); // for editing/deleting
    void editCourseDirect(int index); // Direct edit without ActionDialog
    void applyCourseUpdate(const Course& updatedCourse);
    void refreshCourseUrgency();
    void importSchedule();
    void parseTimeString(const QString& timeStr, Course& c);

signals:
    void navigateToTodoPageRequested();
    void openCourseDetail(const Course& course);

private:
    IConfigProvider *m_configProvider; // not owned

    void renderCourses();

    int getNearestDDL(const QString& courseName);

    QGridLayout *grid;
    QWidget *gridContainer;

    QLabel *weekLabel;
    QLabel *timeLabel;
    QProgressBar *semesterProgress;

    int currentWeek = 9;
    int realWeek = 9;

    void initGrid();
    QWidget* createTopBar();
    QWidget* createBottomStats();
    QWidget* createRightPanel();
    void updateBottomStats();

    QVBoxLayout *ddlLayout;
    QVBoxLayout *todayCourseLayout = nullptr;
    QLabel *todayCourseValue = nullptr;
    QLabel *todayDdlValue = nullptr;
    QLabel *weekDdlValue = nullptr;

    void updateDDLWidget();
    void updateTodayCourses();
    void updateWeekInfo(bool useCurrentWeek = false);
    QWidget* createSuggestionCard();

    void importFromImage();
    void callGeminiAPI(const QString& apiKey, const QByteArray& imageData);
    void onGeminiReplyFinished(QNetworkReply* reply);
    QNetworkAccessManager* m_networkManager = nullptr;
    QProgressDialog* m_loadingDialog = nullptr;
};

#endif