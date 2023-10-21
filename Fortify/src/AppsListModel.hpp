#pragma once

#include "AppInfo.hpp"

#include <QAbstractListModel>
#include <vector>

class AppsListModel : public QAbstractListModel {
    Q_OBJECT
public:

    enum AppsListRoles {
        PackageNameRole = Qt::UserRole,
        LabelRole,
        IconRole,
        LockedRole
    };

    /**
     * @brief data Virtual function redefined from QAbstractListModel which returns the data stored under the given role for the item referred to by the index.
     * @param index of row for which data is needed.
     * @param role is a type of data which is needed to be extracted.
     * @return QVariant The data stored in the list.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief rowCount Virtual function redefined from QAbstractListModel to give number of rows in the list.
     * @return int number of rows.
     */
    int rowCount(const QModelIndex&) const override;

    /**
     * @brief roleNames Virtual function redefined from QAbstractListModel to give list of roles which can be referred in qml.
     * @return The model's role names.
     */
    QHash<int, QByteArray> roleNames() const override;

    void PopuplateInstalledApps(const std::vector<AppInfo>& installed_apps);

//    void RePopuplateAppsList(QMap<std::string, AppInfo> installed_apps);

    void toggleAppLock(const int index);

    std::vector<AppInfo> GetInstalledAppsList() const;

private:
    const QString TAG {"AppsListModel: "};
    std::vector<AppInfo> m_apps_list;
};
