#ifndef STATSPAGE_H
#define STATSPAGE_H

#include <QWidget>
#include <QLayout>

class QFrame;
class QLabel;
class QProgressBar;
class QGridLayout;
class QScrollArea;
class EmptyStateWidget;
class Task;
class Course;

class StatsPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatsPage(QWidget *parent = nullptr);
    void refresh();
    void refreshData();

private:
    void updateEmptyState();
    void clearLayout(QLayout* layout);
    void updateSummary(const QList<Task>& tasks);
    void updateRanking(const QList<Task>& tasks);
    void updateHeatmap(const QList<Task>& tasks);
    void updateTrend(const QList<Task>& tasks);
    void updateSuggestions(const QList<Task>& tasks, const QList<Course>& courses);

    QFrame* cardTotal;
    QFrame* cardDone;
    QFrame* cardOnTime;
    QFrame* cardAvg;
    QVBoxLayout* rankContainer;
    QGridLayout* heatGrid;
    QHBoxLayout* trendContainer;
    QVBoxLayout* suggestContainer;
    QWidget* contentWidget;
    QScrollArea* scrollArea;
    EmptyStateWidget* emptyStateWidget = nullptr;
};

#endif // STATSPAGE_H