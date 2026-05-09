#ifndef SEARCHPOPUP_H
#define SEARCHPOPUP_H

#include <QWidget>
#include <QVector>
#include <QMouseEvent>
#include <QFrame>
#include "../../services/searchservice.h"

class QVBoxLayout;
class QLabel;

class ClickableFrame : public QFrame {
    Q_OBJECT
public:
    explicit ClickableFrame(QWidget* parent = nullptr) : QFrame(parent) {}
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event) override {
        QFrame::mousePressEvent(event);
        emit clicked();
    }
};

class SearchPopup : public QWidget
{
    Q_OBJECT
public:
    explicit SearchPopup(QWidget* parent = nullptr);
    void showResults(const QVector<SearchResult>& results);

signals:
    void courseSelected(const QString& courseName);
    void taskSelected(int taskIndex);
    void fileSelected(const QString& filePath);

private:
    void clearResults();
    QVBoxLayout* resultsLayout;
    QWidget* contentWidget;
};

#endif