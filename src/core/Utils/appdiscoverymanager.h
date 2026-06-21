#ifndef APPDISCOVERYMANAGER_H
#define APPDISCOVERYMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class AppDiscoveryManager : public QObject
{
    Q_OBJECT
public:
    explicit AppDiscoveryManager(QObject *parent = nullptr);
    static AppDiscoveryManager* instance();

    // Re-scans the system for installed applications
    void scanInstalledApps();

    // Returns a map of App Name -> Executable Path
    QMap<QString, QString> getInstalledApps() const;

private:
    QMap<QString, QString> installedApps;
    
    void scanDirectoryForShortcuts(const QString& directoryPath);
    QString resolveShortcutTarget(const QString& shortcutPath);
};

#endif // APPDISCOVERYMANAGER_H
