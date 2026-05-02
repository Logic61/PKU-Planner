#include "coursecellwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QTimer>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

CourseCellWidget::CourseCellWidget(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(80, 60);

    setStyleSheet(R"(
        QFrame {
            background:#FAFAFA;
            border-radius:10px;
        }
        QFrame:hover {
            background:#FFEAEA;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6,6,6,6);

    title = new QLabel("");
    title->setStyleSheet("font-weight:bold; font-size:12px;");

    info = new QLabel("");
    info->setStyleSheet("font-size:11px; color:#666;");

    layout->addWidget(title);
    layout->addWidget(info);
}

void CourseCellWidget::setCourse(QString name, QString location)
{
    title->setText(name);
    info->setText(location);

    setStyleSheet(R"(
        QFrame {
            background: #E8F0FF;
            border-radius: 10px;
        }
        QFrame:hover {
            background: #D6E4FF;
        }
    )");
}

void CourseCellWidget::enterEvent(QEnterEvent *)
{
    QTimer::singleShot(300, this, [this]() {
        if(!title->text().isEmpty())
        {
            QToolTip::showText(cursor().pos(),
                title->text() + "\n地点: " + info->text() +
                "\nDDL: 3天后");
        }
    });
}

void CourseCellWidget::leaveEvent(QEvent *)
{
    QToolTip::hideText();
}

void CourseCellWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    QDialog dialog;
    dialog.setWindowTitle("编辑课程");

    QFormLayout *layout = new QFormLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit(title->text());
    QLineEdit *teacherEdit = new QLineEdit();
    QLineEdit *roomEdit = new QLineEdit(info->text());
    QLineEdit *examEdit = new QLineEdit();

    layout->addRow("课程名称", nameEdit);
    layout->addRow("教师", teacherEdit);
    layout->addRow("教室", roomEdit);
    layout->addRow("考试时间", examEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );

    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() == QDialog::Accepted)
    {
        title->setText(nameEdit->text());
        info->setText(roomEdit->text());
    }
}
