#ifndef COURSETABBAR_H
#define COURSETABBAR_H

#include <QWidget>

class QPushButton;

class CourseTabBar : public QWidget
{
    Q_OBJECT
public:
    explicit CourseTabBar(QWidget* parent = nullptr);
    
    void setCurrentTab(int index);

signals:
    void tabChanged(int index);

private:
    QPushButton* infoBtn;
    QPushButton* taskBtn;
    QPushButton* fileBtn;
    QPushButton* statsBtn;
};

#endif
