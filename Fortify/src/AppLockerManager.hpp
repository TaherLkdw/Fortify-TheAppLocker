#pragma once

#include <map>
#include <memory>
#include <vector>

class AppLockerAndroid;

class AppLockerManager {
public:
    static AppLockerManager* GetInstance();
    static void ReleaseInstance();

    std::map<std::string, std::string> GetInstalledApps();
    void LockApps(const std::vector<std::string>& apps_list);
    void UnlockApps();
    void SetInstalledAppIconsDirectoryPath(const std::string& path);

private:
    AppLockerManager();
    AppLockerManager(const AppLockerManager&) {};
    AppLockerManager(const AppLockerManager&&) {};

    static AppLockerManager* m_app_locker_manager;
    std::shared_ptr<AppLockerAndroid> m_app_locker_android {nullptr};
};
