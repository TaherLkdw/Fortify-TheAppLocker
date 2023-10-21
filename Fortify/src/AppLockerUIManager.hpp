#pragma once

#include "AppInfo.hpp"
#include "AppsListModel.hpp"
#include "FavouriteListModel.hpp"

#include <QObject>
#include <QMap>

#include <memory>

class AppLockerManager;

class AppLockerUIManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(AppsListModel* appsListModel READ appsListModel NOTIFY appsListModelChanged);
    Q_PROPERTY(FavouriteListModel* favouriteListModel READ favouriteListModel NOTIFY favouriteListModelChanged);
    Q_PROPERTY(QString installedAppIconsDirPath READ installedAppIconsDirPath NOTIFY installedAppIconsDirPathChanged);

public:
    AppLockerUIManager(QObject* parent = nullptr);
    ~AppLockerUIManager();

    void Init();

    AppsListModel* appsListModel() const;
    FavouriteListModel* favouriteListModel() const;
    QString installedAppIconsDirPath() const;

    Q_INVOKABLE void updateQuickLockView();
    Q_INVOKABLE void toggleAppLock(const int index);
    Q_INVOKABLE void quickLockApps();
    Q_INVOKABLE void unlockApps();
    Q_INVOKABLE void addFavourite(const QString& favourite_name);
    Q_INVOKABLE void removeFavourite(const int favourite_index);
    Q_INVOKABLE void updateFavouriteAppsView(const int favourite_index);
    Q_INVOKABLE void updateFavourite();
    Q_INVOKABLE void lockFavourite(const int favourite_index);

signals:
    void appsListModelChanged();
    void favouriteListModelChanged();
    void installedAppIconsDirPathChanged();
    void favouriteViewUpdated();

private:
    const QString TAG = "AppLockerUIManager";
    const QString APP_ICON_DIR_NAME = "InstalledAppIcons";
    std::unique_ptr<AppsListModel> m_apps_list_model {nullptr};
    std::unique_ptr<FavouriteListModel> m_favourite_list_model {nullptr};
    AppLockerManager* m_app_locker_manager {nullptr};
    QString m_installed_app_icons_dir_path;
    int m_viewed_favourited_index {-1};
    bool m_refresh_apps_list {true};
    std::vector<AppInfo> m_installed_apps_list;

    std::vector<AppInfo> GetInstalledAppsList();
};
