#pragma once

#include <QFrame>

class QTimer;

class DDLPreviewWidget : public QFrame
{
    Q_OBJECT
public:
    explicit DDLPreviewWidget(const QString &courseName, QWidget *parent = nullptr);
    ~DDLPreviewWidget() override = default;
    void showNear(const QPoint &globalPos);

signals:
    void requestNavigateToTodoPage();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void buildUI(const QString &courseName);
    void setHoverState(bool hovering);

    bool m_mouseInside = false;
    bool m_hideScheduled = false;
    QTimer *m_hideTimer = nullptr;
};
