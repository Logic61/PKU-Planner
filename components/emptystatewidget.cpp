#include "emptystatewidget.h"
#include "../ui/theme.h"
#include <QGraphicsDropShadowEffect>

EmptyStateWidget::EmptyStateWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    QFrame *card = new QFrame(this);
    card->setStyleSheet("background:white; border-radius:20px;");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 20));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);
    cardLayout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel(card);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setStyleSheet("font-size:64px;");
    cardLayout->addWidget(m_iconLabel);

    m_titleLabel = new QLabel(card);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString("color:%1;font-size:20px;font-weight:700;").arg(Theme::TEXT_PRIMARY));
    cardLayout->addWidget(m_titleLabel);

    m_descLabel = new QLabel(card);
    m_descLabel->setAlignment(Qt::AlignCenter);
    m_descLabel->setStyleSheet(QString("color:%1;font-size:14px; line-height:22px;").arg(Theme::TEXT_TERTIARY));
    m_descLabel->setWordWrap(true);
    m_descLabel->setMaximumWidth(280);
    cardLayout->addWidget(m_descLabel);

    m_button = new QPushButton(card);
    m_button->setCursor(Qt::PointingHandCursor);
    m_button->setFixedHeight(44);
    m_button->setStyleSheet(QString(R"(
        QPushButton {
            background: %1;
            border: none;
            border-radius: 12px;
            color: white;
            font-size: 14px;
            font-weight: 600;
            padding: 0 32px;
        }
        QPushButton:hover {
            background: %2;
        }
        QPushButton:pressed {
            background: %3;
        }
    )").arg(Theme::PRIMARY).arg(Theme::PRIMARY_DARK).arg(Theme::PRIMARY_DARK));
    m_button->hide();
    cardLayout->addWidget(m_button);
    cardLayout->addSpacing(8);

    layout->addWidget(card);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(m_button, &QPushButton::clicked, this, &EmptyStateWidget::buttonClicked);
}

void EmptyStateWidget::setContent(const QString &icon, const QString &title, const QString &description, const QString &buttonText)
{
    m_iconLabel->setText(icon);
    m_titleLabel->setText(title);
    m_descLabel->setText(description);

    if (buttonText.isEmpty()) {
        m_button->hide();
    } else {
        m_button->setText(buttonText);
        m_button->show();
    }
}