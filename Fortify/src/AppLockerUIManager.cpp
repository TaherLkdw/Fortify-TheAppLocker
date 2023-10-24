#include "AppLockerUIManager.hpp"
#include "AppLockerManager.hpp"

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QStandardPaths>

#include <algorithm>

/*!
   \class AppLockerUIManager
   \brief The AppLockerUIManager class interacts with UI implemented in qml.
   \inmodule FortiyMainApplication

   The AppLockerUIManager class provide interfaces to interact with Qmls.
   It has all the \b{Q_PROPERTY, Q_INVOKABLE} methods.
   It also instantiates class \b {AppsListModel} and \b {FavouriteListModel} to provide model in qml context.
*/

/*!
    Constructs an object with parent object \a parent.

    The destructor of a parent object destroys all child objects.

    Setting \a parent to \nullptr constructs an object with no parent. If the
    object is a widget, it will become a top-level window.
*/

AppLockerUIManager::AppLockerUIManager(QObject* parent) : QObject(parent) {   
}

/*!
   \fn void AppLockerUIManager::Init()
   \brief The Init() function initialises necessary classes and connects signal slots.
*/
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

/*!
   \property AppLockerUIManager::appsListModel
   \brief Returns pointer of AppsListModel class.
*/
/*!
  \fn void AppLockerUIManager::appsListModelChanged()
  \internal

  While not useful from C++ due to how appsListModel is normally set once on
  startup, this is still needed for QML so that bindings are reevaluated after
  that initial change.
*/
AppsListModel* AppLockerUIManager::appsListModel() const {
    return m_apps_list_model.get();
}

/*!
   \property AppLockerUIManager::favouriteListModel
   \brief Returns pointer of FavouriteListModel class.
*/
/*!
  \fn void AppLockerUIManager::favouriteListModel()
  \internal

  While not useful from C++ due to how favouriteListModel is normally set once on
  startup, this is still needed for QML so that bindings are reevaluated after
  that initial change.
*/
FavouriteListModel* AppLockerUIManager::favouriteListModel() const {
    return m_favourite_list_model.get();
}

/*!
   \property AppLockerUIManager::installedAppIconsDirPath
   \brief Returns directory path where application icons are saved.
*/
/*!
  \fn void AppLockerUIManager::installedAppIconsDirPath()
  \internal

  While not useful from C++ due to how installedAppIconsDirPath is normally set once on
  startup, this is still needed for QML so that bindings are reevaluated after
  that initial change.
*/
QString AppLockerUIManager::installedAppIconsDirPath() const {
    return m_installed_app_icons_dir_path;
}

/*!
   \fn std::vector<AppInfo> AppLockerUIManager::GetInstalledAppsList()
   \brief The GetInstalledAppsList() function updates the names and packages info of installed applications of the device.

   The function first get the information from class AppLockerManager.
   Then updates the m_installed_apps_list by creating and pushing class AppInfo objects.
   Then it sorts the list by name of the apps and returns std::vector<AppInfo>.
*/
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

/*!
   \fn void AppLockerUIManager::updateQuickLockView()
   \brief The updateQuickLockView() function updates the installed apps view by fetching the fresh list of installed apps and updating the model.
*/
void AppLockerUIManager::updateQuickLockView() {
    m_apps_list_model->PopuplateInstalledApps(GetInstalledAppsList());
}

/*!
   \fn void AppLockerUIManager::toggleAppLock(const int index)
   \brief The toggleAppLock(\a index) function toggles the lock property of the class AppInfo from the AppsListModel by index.
*/
void AppLockerUIManager::toggleAppLock(const int index) {
    m_apps_list_model->toggleAppLock(index);
}

/*!
   \fn void AppLockerUIManager::quickLockApps()
   \brief The quickLockApps() function sends the locked apps to class AppLockerManager.
*/
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

/*!
   \fn void AppLockerUIManager::unlockApps()
   \brief The unlockApps() function calls the UnlockApps() interface of class AppLockerManager.
*/
void AppLockerUIManager::unlockApps() {
    m_app_locker_manager->UnlockApps();
}

/*!
   \fn void AppLockerUIManager::addFavourite(const QString& favourite_name)
   \brief The addFavourite(\a favourite_name) function adds the favourite instance in the list of class FavouriteListModel.

   This function takes the favourite name as parameter. It creates the instance of class FavouriteInfo.
   Updates the name and unlocked apps list parameters of the class FavouriteInfo and sends that instance to class FavouriteListModel
   using AddFavourite() interface.
*/
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

/*!
   \fn void AppLockerUIManager::removeFavourite(const int favourite_index)
   \brief The removeFavourite(\a favourite_index) function removes the favourite instance from the list of class FavouriteListModel.
*/
void AppLockerUIManager::removeFavourite(const int favourite_index) {
    m_favourite_list_model->RemoveFavourite(favourite_index);
}

/*!
   \fn void AppLockerUIManager::updateFavouriteAppsView(const int favourite_index)
   \brief The updateFavouriteAppsView(\a favourite_index) function updates the installed apps view for a selected favourite.

   This function updates the installed apps view for a selected favourite. It fetches the fresh installed apps list from device.
   Updates the locked property from the selected favourite and notifies UI to update the view.
   This function will be called when user selects edit favourite button from favourite list view..
*/
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

/*!
   \fn void AppLockerUIManager::updateFavourite()
   \brief The updateFavourite() function updates the unlocked apps list for a selected favourite.

   This function will be called when user presses Update Favourite button.
*/
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

/*!
   \fn void AppLockerUIManager::lockFavourite(const int favourite_index)
   \brief The lockFavourite(\a favourite_index) function sends the locked apps to class AppLockerManager.

   This function will be called when user selects favourite from the list to lock apps selected by favourite.
*/
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
