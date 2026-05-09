#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QDialog>
#include <QString>

class ConfirmDialog : public QDialog
{
    Q_OBJECT
public:
    static bool confirm(
        QWidget* parent,
        const QString& title,
        const QString& message,
        const QString& confirmText = "确认",
        bool isDangerous = false
    );

private:
    explicit ConfirmDialog(
        QWidget* parent,
        const QString& title,
        const QString& message,
        const QString& confirmText,
        bool isDangerous
    );
};

#endif