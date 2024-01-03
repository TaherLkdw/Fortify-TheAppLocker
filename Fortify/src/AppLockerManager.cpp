#include "AppLockerManager.hpp"
#include "android_files/AppLockerAndroid.hpp"

#include <QCoreApplication>

/*!
   \class AppLockerManager
   \brief The singleton AppLockerManager class holds the instance of a platform level class to call platform level interfaces.
   \inmodule FortiyMainApplication

   The AppLockerAndroid class provide interfaces to interact with platform level code like Android.
   It holds the instance of a class AppLockerAndroid and when UI calls the interfaces of this class, it passes to the platform level classes like AppLockerAndroid.
*/

AppLockerManager* AppLockerManager::m_app_locker_manager = nullptr;

AppLockerManager::AppLockerManager() {
    m_app_locker_android = std::make_shared<AppLockerAndroid>();
    QCoreApplication::connect(qApp, &QCoreApplication::aboutToQuit, [=]() {
        ReleaseInstance();
    });
}

/*!
   \fn static AppLockerManager* AppLockerManager::GetInstance()
   \brief The GetInstance() static function initialises this classe's instance if not already and returns the pointer.
*/
AppLockerManager* AppLockerManager::GetInstance() {
    if (m_app_locker_manager == nullptr) {
        m_app_locker_manager = new AppLockerManager();
    }
    return m_app_locker_manager;
}

/*!
   \fn static void AppLockerManager::ReleaseInstance()
   \brief The ReleaseInstance() static function releases this classe's instance.
*/
void AppLockerManager::ReleaseInstance() {
    if (m_app_locker_manager) {
        delete m_app_locker_manager;
        m_app_locker_manager = nullptr;
    }
}

/*!
   \fn std::map<std::string, std::string> AppLockerManager::GetInstalledApps()
   \brief The GetInstalledApps() function fetches the list from platform level class and returns the list.
*/
std::map<std::string, std::string> AppLockerManager::GetInstalledApps() {
    return m_app_locker_android->GetInstalledApps();
}

/*!
   \fn void AppLockerManager::LockApps(const std::vector<std::string>& apps_list)
   \brief The LockApps(\a apps_list) function passes the locked apps list to platform level class.
*/
void AppLockerManager::LockApps(const std::vector<std::string>& apps_list) {
    m_app_locker_android->LockApps(apps_list);
}

/*!
   \fn void AppLockerManager::UnlockApps()
   \brief The UnlockApps() function calls the platform level interface to unlock apps.
*/
void AppLockerManager::UnlockApps() {
    m_app_locker_android->UnlockApps();
}

/*!
   \fn void AppLockerManager::SetInstalledAppIconsDirectoryPath(const std::string& path)
   \brief The SetInstalledAppIconsDirectoryPath(\a path) function sets the path to save app icons.
*/
void AppLockerManager::SetInstalledAppIconsDirectoryPath(const std::string& path) {
    m_app_locker_android->SetInstalledAppIconsDirectoryPath(path);
}

bool AppLockerManager::IsAppLockRunning() const {
    return m_app_locker_android->IsAppLockRunning();
}
