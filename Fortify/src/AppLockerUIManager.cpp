#include "AppLockerUIManager.hpp"
#include "AppLockerManager.hpp"

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QStandardPaths>

#include <algorithm>

AppLockerUIManager::AppLockerUIManager(QObject* parent) : QObject(parent) {   
}

void AppLockerUIManager::Init() {
    m_apps_list_model = std::make_unique<AppsListModel>();
    emit appsListModelChanged();

    m_favourite_list_model = std::make_unique<FavouriteListModel>();
    emit favouriteListModelChanged();

    m_app_locker_manager = AppLockerManager::GetInstance();
    const auto& app_icon_dir_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::separator() + APP_ICON_DIR_NAME;
    QDir app_icon_dir(app_icon_dir_path);
    if (!app_icon_dir.exists()) {
        const bool ret = app_icon_dir.mkpath(app_icon_dir_path);
        if (!ret) {
            throw std::runtime_error("Failed to create app_icon_dir_path");
        }
    }
    m_installed_app_icons_dir_path = app_icon_dir.path() + QDir::separator();
    m_app_locker_manager->SetInstalledAppIconsDirectoryPath(m_installed_app_icons_dir_path.toStdString());
    emit installedAppIconsDirPathChanged();

    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [=](const Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive) {
            m_refresh_apps_list = true;
        }
    });
}

AppLockerUIManager::~AppLockerUIManager() {
//    QDir app_icon_dir(m_installed_app_icons_dir_path);
//    if (app_icon_dir.exists()) {
//        app_icon_dir.removeRecursively();
//    }
}

AppsListModel* AppLockerUIManager::appsListModel() const {
    return m_apps_list_model.get();
}

FavouriteListModel* AppLockerUIManager::favouriteListModel() const {
    return m_favourite_list_model.get();
}

QString AppLockerUIManager::installedAppIconsDirPath() const {
    return m_installed_app_icons_dir_path;
}

std::vector<AppInfo> AppLockerUIManager::GetInstalledAppsList() {
    if (m_refresh_apps_list) {
        m_refresh_apps_list = false;
        const auto& installed_apps_names = m_app_locker_manager->GetInstalledApps();
        qDebug() << TAG << __func__ << " installed apps list size: " << installed_apps_names.size();

        m_installed_apps_list.clear();
        m_installed_apps_list.resize(installed_apps_names.size());
        unsigned int index = 0;
        for(const auto& it : installed_apps_names) {
            AppInfo info(it.first, it.second);
            m_installed_apps_list[index] = info;
            ++index;
        }
        m_installed_apps_list.shrink_to_fit();

        std::sort(m_installed_apps_list.begin(), m_installed_apps_list.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.m_label.compare(rhs.m_label) < 0;
        });
    }
    return m_installed_apps_list;
}

void AppLockerUIManager::updateQuickLockView() {
    m_apps_list_model->PopuplateInstalledApps(GetInstalledAppsList());
}

void AppLockerUIManager::toggleAppLock(const int index) {
    m_apps_list_model->toggleAppLock(index);
}

void AppLockerUIManager::quickLockApps() {
    //Adding only apps which user wants to lock and removing apps which user has selected to keep unrestricted.
    const auto& installed_apps = m_apps_list_model->GetInstalledAppsList();
    std::vector<std::string> lock_apps(installed_apps.size());
    for (const auto& appInfo : installed_apps) {
        if (appInfo.m_locked) {
            lock_apps.push_back(appInfo.m_package_name);
        }
    }
    lock_apps.shrink_to_fit();
    m_app_locker_manager->LockApps(lock_apps);
}

void AppLockerUIManager::unlockApps() {
    m_app_locker_manager->UnlockApps();
}

void AppLockerUIManager::addFavourite(const QString& favourite_name) {
    const auto& apps_list = m_apps_list_model->GetInstalledAppsList();
    std::vector<std::string> unlocked_apps;
    //NOTE:- In favourites list we only save unlocked apps names
    for (const auto& app : apps_list) {
        if (!app.m_locked) {
            unlocked_apps.push_back(app.m_label);
        }
    }
    FavouriteInfo favourite_info(favourite_name.toStdString(), unlocked_apps);
    m_favourite_list_model->AddFavourite(favourite_info);
}

void AppLockerUIManager::removeFavourite(const int favourite_index) {
    m_favourite_list_model->RemoveFavourite(favourite_index);
}

void AppLockerUIManager::updateFavouriteAppsView(const int favourite_index) {
    m_viewed_favourited_index = favourite_index;
    m_apps_list_model->PopuplateInstalledApps(GetInstalledAppsList());
    const auto& unlocked_apps_name_list = m_favourite_list_model->GetUnlockedAppsListByIndex(favourite_index);

    for (unsigned int i = 0; i < m_installed_apps_list.size(); ++i) {
        const auto& unlocked_apps_name_it = find_if(unlocked_apps_name_list.begin(), unlocked_apps_name_list.end(), [=](const std::string& name) {
            return name == m_installed_apps_list[i].m_label;
        });
        //Updating unlock status in new installed apps list, for the apps, which are present in the unlocked list
        if (unlocked_apps_name_it != unlocked_apps_name_list.end()) {
            m_apps_list_model->toggleAppLock(i);
        }
    }
    emit favouriteViewUpdated();
}

void AppLockerUIManager::updateFavourite() {
    std::vector<std::string> unlocked_apps_name_list;
    const auto& installed_apps = m_apps_list_model->GetInstalledAppsList();
    for (const auto& appInfo : installed_apps) {
        if (!appInfo.m_locked) {
            unlocked_apps_name_list.push_back(appInfo.m_label);
        }
    }
    m_favourite_list_model->updateUnlockedAppsList(unlocked_apps_name_list, m_viewed_favourited_index);
}

void AppLockerUIManager::lockFavourite(const int favourite_index) {
    const auto& installed_apps_list = GetInstalledAppsList();
    const auto& unlocked_apps_name_list = m_favourite_list_model->GetUnlockedAppsListByIndex(favourite_index);
    std::vector<std::string> locked_apps(installed_apps_list.size());

    for (auto it = installed_apps_list.begin(); it != installed_apps_list.end(); ++it) {
        const auto& unlocked_apps_name_it = find_if(unlocked_apps_name_list.begin(), unlocked_apps_name_list.end(), [it](const std::string& name) {
            return name == it->m_label;
        });
        if (unlocked_apps_name_it != unlocked_apps_name_list.end()) {
            continue;
        }
        //Push only apps which are not in unlocked list
        locked_apps.push_back(it->m_package_name);
    }
    locked_apps.shrink_to_fit();
    m_app_locker_manager->LockApps(locked_apps);
}
