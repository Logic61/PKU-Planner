#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <QJsonObject>
#include <QString>
#include <QWidget>

class DataService {
public:
    static QJsonObject loadJson(const QString& path, const QJsonObject& defaultValue);
    static bool saveJson(const QString& path, const QJsonObject& data);
    static void ensureDataFiles();
    static void showErrorToast(QWidget* parent, const QString& message);

private:
    static QString getDataDir();
    static QString backupCorruptedFile(const QString& path);
};

#endif