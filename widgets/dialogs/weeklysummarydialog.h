#ifndef WEEKLYSUMMARYDIALOG_H
#define WEEKLYSUMMARYDIALOG_H

#include <QDialog>
#include "../../services/weeklysummaryservice.h"

class WeeklySummaryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WeeklySummaryDialog(QWidget* parent = nullptr);
    ~WeeklySummaryDialog();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    class PrivateData;
    PrivateData *d;
};

#endif