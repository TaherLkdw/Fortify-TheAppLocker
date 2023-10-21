#pragma once

#include "AppInfo.hpp"

#include <QAbstractListModel>
#include <vector>

class AppsListModel : public QAbstractListModel {
    Q_OBJECT
    enum AppsListRoles {
        PackageNameRole = Qt::UserRole,
        LabelRole,
        IconRole,
        LockedRole
    };
public:
    AppsListModel() = default;

    void PopuplateInstalledApps(const std::vector<AppInfo>& installed_apps);

    void toggleAppLock(const int index);

    std::vector<AppInfo> GetInstalledAppsList() const;

protected:
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    const QString TAG {"AppsListModel: "};
    std::vector<AppInfo> m_apps_list;
};
