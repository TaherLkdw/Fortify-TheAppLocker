#include "AppLockerManager.hpp"
#include "android_files/AppLockerAndroid.hpp"

#include <QCoreApplication>

AppLockerManager* AppLockerManager::m_app_locker_manager = nullptr;

AppLockerManager::AppLockerManager() {
    m_app_locker_android = std::make_shared<AppLockerAndroid>();
    QCoreApplication::connect(qApp, &QCoreApplication::aboutToQuit, [=]() {
        ReleaseInstance();
    });
}

AppLockerManager* AppLockerManager::GetInstance() {
    if (m_app_locker_manager == nullptr) {
        m_app_locker_manager = new AppLockerManager();
    }
    return m_app_locker_manager;
}

void AppLockerManager::ReleaseInstance() {
    if (m_app_locker_manager) {
        delete m_app_locker_manager;
        m_app_locker_manager = nullptr;
    }
}

std::map<std::string, std::string> AppLockerManager::GetInstalledApps() {
    return m_app_locker_android->GetInstalledApps();
}

void AppLockerManager::LockApps(const std::vector<std::string>& apps_list) {
    m_app_locker_android->LockApps(apps_list);
}

void AppLockerManager::UnlockApps() {
    m_app_locker_android->UnlockApps();
}

void AppLockerManager::SetInstalledAppIconsDirectoryPath(const std::string& path) {
    m_app_locker_android->SetInstalledAppIconsDirectoryPath(path);
}
