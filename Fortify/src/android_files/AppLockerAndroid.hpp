#pragma once

#include <QString>

#include <map>

class AppLockerAndroid {
public:
    AppLockerAndroid();
    std::map<std::string, std::string> GetInstalledApps();
    void LockApps(const std::vector<std::string>& apps_list);
    void UnlockApps();
    void SetInstalledAppIconsDirectoryPath(const std::string& path);
    bool IsAppLockRunning() const;

private:
    const QString TAG {"AppLockerAndroid: "};
};
