#ifndef TODOPAGE_H
#define TODOPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>

class QScrollArea;
class QVBoxLayout;
class QLabel;

class TodoPage : public QWidget
{
    Q_OBJECT

public:
    explicit TodoPage(QWidget *parent = nullptr);

private:
    QComboBox *courseFilter = nullptr;
    QComboBox *timeFilter = nullptr;
    QComboBox *statusFilter = nullptr;
    QLineEdit *searchEdit = nullptr;
    QLabel *summaryLabel = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *boardWidget = nullptr;
    QVBoxLayout *boardLayout = nullptr;

    QWidget* createFilterBar();
    QWidget* createSectionHeader(const QString &title, int count, const QString &accent);
    void refreshCourseFilter();
    void refreshTasks();
    void applyFilter();
    void rebuildBoard();
    void editTaskByIndex(int sourceIndex);
};

#endif