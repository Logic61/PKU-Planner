#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H

#include <QObject>
#include <QString>
#include <QDate>

class ConfigService : public QObject
{
    Q_OBJECT

public:
    static ConfigService& instance();

    bool isReminderEnabled() const;
    void setReminderEnabled(bool enabled);

    int getReminderHours() const;
    void setReminderHours(int hours);

    bool isDetailDrawerMode() const;
    void setDetailDrawerMode(bool drawerMode);

    QString getExportPath() const;
    void setExportPath(const QString& path);

    bool isOnboardingShown() const;
    void setOnboardingShown(bool shown);

    QDate getSemesterStart() const;
    void setSemesterStart(const QDate& date);

    QDate getSemesterEnd() const;
    void setSemesterEnd(const QDate& date);

    QDate getLastSummaryDate() const;
    void setLastSummaryDate(const QDate& date);

    int getCurrentWeek() const;
    bool isSingleWeek() const;

    void resetOnboarding();
    void resetAllData();

    QString getDataPath() const;

signals:
    void configChanged();

private:
    ConfigService();
    void load();
    void save();

    bool m_reminderEnabled;
    int m_reminderHours;
    bool m_detailDrawerMode;
    QString m_exportPath;
    bool m_onboardingShown;
    QDate m_semesterStart;
    QDate m_semesterEnd;
    QDate m_lastSummaryDate;
};

#endif