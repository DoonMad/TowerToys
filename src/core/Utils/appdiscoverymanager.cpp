#include "appdiscoverymanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <windows.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <comdef.h>

AppDiscoveryManager::AppDiscoveryManager(QObject *parent) : QObject(parent)
{
    // Initialize COM on this thread (required for IShellLink)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}

AppDiscoveryManager* AppDiscoveryManager::instance()
{
    static AppDiscoveryManager instance;
    return &instance;
}

void AppDiscoveryManager::scanInstalledApps()
{
    if (!installedApps.isEmpty()) return;
    
    installedApps.clear();

    // 1. Scan standard Start Menu locations
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    locations.append(qgetenv("ProgramData") + "\\Microsoft\\Windows\\Start Menu\\Programs");
    locations.append(qgetenv("APPDATA") + "\\Microsoft\\Windows\\Start Menu\\Programs");
    locations.removeDuplicates();

    for (const QString& path : locations) {
        scanDirectoryForShortcuts(path);
    }
    
    // 2. Scan UWP / Windows Store App Execution Aliases
    QString windowsAppsPath = qgetenv("LOCALAPPDATA") + "\\Microsoft\\WindowsApps";
    QDir appsDir(windowsAppsPath);
    if (appsDir.exists()) {
        QFileInfoList exeFiles = appsDir.entryInfoList(QStringList() << "*.exe", QDir::Files);
        for (const QFileInfo& fileInfo : exeFiles) {
            QString exeName = fileInfo.fileName().toLower();
            QString fullPath = fileInfo.absoluteFilePath();
            
            // Map common UWP names to nice names
            QString niceName = fileInfo.completeBaseName();
            if (exeName == "notepad.exe") niceName = "Notepad (UWP)";
            else if (exeName == "mspaint.exe") niceName = "Paint";
            else if (exeName == "snippingtool.exe") niceName = "Snipping Tool";
            else if (exeName == "wt.exe") niceName = "Windows Terminal";
            else if (exeName == "calculator.exe" || exeName == "calc.exe") niceName = "Calculator";
            
            if (!installedApps.contains(niceName)) {
                installedApps.insert(niceName, fullPath);
            }
        }
    }
    
    // 3. Add Common Core Windows Utilities
    QString sys32 = qgetenv("WINDIR") + "\\System32\\";
    installedApps.insert("Command Prompt", sys32 + "cmd.exe");
    installedApps.insert("Windows PowerShell", sys32 + "WindowsPowerShell\\v1.0\\powershell.exe");
    installedApps.insert("File Explorer", qgetenv("WINDIR") + "\\explorer.exe");
    installedApps.insert("Task Manager", sys32 + "taskmgr.exe");
    installedApps.insert("Control Panel", sys32 + "control.exe");
    installedApps.insert("Registry Editor", qgetenv("WINDIR") + "\\regedit.exe");
}

QMap<QString, QString> AppDiscoveryManager::getInstalledApps() const
{
    return installedApps;
}

void AppDiscoveryManager::scanDirectoryForShortcuts(const QString& directoryPath)
{
    QDir dir(directoryPath);
    if (!dir.exists()) return;

    QDirIterator it(directoryPath, QStringList() << "*.lnk", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString lnkPath = it.next();
        QString targetPath = resolveShortcutTarget(lnkPath);

        // We only care about executable files (.exe)
        if (!targetPath.isEmpty() && targetPath.endsWith(".exe", Qt::CaseInsensitive)) {
            QFileInfo fileInfo(lnkPath);
            QString appName = fileInfo.completeBaseName(); // Gets the name without .lnk
            
            // Clean up name (e.g., "Google Chrome" instead of "Google Chrome.lnk")
            if (!installedApps.contains(appName)) {
                installedApps.insert(appName, targetPath);
            }
        }
    }
}

QString AppDiscoveryManager::resolveShortcutTarget(const QString& shortcutPath)
{
    QString targetPath = "";
    IShellLink* psl;

    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres)) {
        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
        if (SUCCEEDED(hres)) {
            // Load the shortcut
            hres = ppf->Load((LPCWSTR)shortcutPath.utf16(), STGM_READ);
            if (SUCCEEDED(hres)) {
                // Resolve the link (this might show UI if the target is missing, so we pass SLR_NO_UI)
                hres = psl->Resolve(NULL, SLR_NO_UI | SLR_NOUPDATE | SLR_NOSEARCH);
                if (SUCCEEDED(hres)) {
                    WCHAR szGotPath[MAX_PATH];
                    WIN32_FIND_DATA wfd;
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_UNCPRIORITY | SLGP_RAWPATH);
                    if (SUCCEEDED(hres)) {
                        targetPath = QString::fromWCharArray(szGotPath);
                    }
                }
            }
            ppf->Release();
        }
        psl->Release();
    }
    return targetPath;
}
