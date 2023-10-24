#include "AppsListModel.hpp"

#include <QDebug>

/*!
   \class AppsListModel
   \brief The AppsListModel class is list model class to provide data in qml.
   \inmodule FortiyMainApplication

   The AppsListModel class provides interfaces to provide data to qml GridView.
*/

/*!
    \enum AppsListModel::AppsListRoles

    \value PackageNameRole Name of the application package.
    \value LabelRole Label of the application.
    \value IconRole Icon of the application.
    \value LockedRole Is application locked.
*/

/*! \reimp */
QVariant AppsListModel::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= (int)m_apps_list.size()) {
        return QVariant();
    }

    switch (role) {
        case PackageNameRole: {
            return m_apps_list[index.row()].m_package_name.c_str();
        }
        case LabelRole: {
            return m_apps_list[index.row()].m_label.c_str();
        }
        case IconRole: {
            return m_apps_list[index.row()].m_icon.c_str();
        }
        case LockedRole: {
            return m_apps_list[index.row()].m_locked;
        }
        default:
            return QVariant();
    }
    return QVariant();
}

/*! \reimp */
int AppsListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_apps_list.size();
}

/*! \reimp */
QHash<int, QByteArray> AppsListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PackageNameRole] = "packageNameRole";
    roles[LabelRole] = "labelRole";
    roles[IconRole] = "iconRole";
    roles[LockedRole] = "lockedRole";
    return roles;
}

/*!
   \fn void AppsListModel::PopuplateInstalledApps(const std::vector<AppInfo>& installed_apps)
   \brief The PopuplateInstalledApps(\a installed_apps) function copies the installed apps list and resets the model.
*/
void AppsListModel::PopuplateInstalledApps(const std::vector<AppInfo>& installed_apps) {
    qDebug() << TAG << __func__ << " installed_apps: " << installed_apps.size();
    beginResetModel();
    m_apps_list = installed_apps;
    endResetModel();
}

/*!
   \fn void AppsListModel::toggleAppLock(const int index)
   \brief The toggleAppLock(\a index) function toggles the m_locked variable of the class AppInfo from the list by index.
*/
void AppsListModel::toggleAppLock(const int index) {
    if (index < 0 || index >= (int)m_apps_list.size()) {
        qWarning() << TAG << __func__ << " Index out of bound: " << index;
        return;
    }
    qDebug() << TAG << __func__ << " index: " << index << " package: " << m_apps_list[index].m_package_name.c_str();
    m_apps_list[index].m_locked = !m_apps_list[index].m_locked;
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), QVector<int>() << AppsListRoles::LockedRole);
}

/*!
    \fn std::vector<AppInfo> AppsListModel::GetInstalledAppsList() const
    \brief The GetInstalledAppsList() function returns the apps list.
*/
std::vector<AppInfo> AppsListModel::GetInstalledAppsList() const {
    return m_apps_list;
}
