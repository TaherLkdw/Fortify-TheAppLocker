#include "AppsListModel.hpp"

#include <QDebug>

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

int AppsListModel::rowCount(const QModelIndex &) const {
    return m_apps_list.size();
}

QHash<int, QByteArray> AppsListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PackageNameRole] = "packageNameRole";
    roles[LabelRole] = "labelRole";
    roles[IconRole] = "iconRole";
    roles[LockedRole] = "lockedRole";
    return roles;
}

void AppsListModel::PopuplateInstalledApps(const std::vector<AppInfo>& installed_apps) {
    qDebug() << TAG << __func__ << " installed_apps: " << installed_apps.size();
    beginResetModel();
    m_apps_list = installed_apps;
    endResetModel();
}

//void AppsListModel::RePopuplateAppsList(QMap<std::string, AppInfo> installed_apps) {
//    qDebug() << TAG << __func__ << " installed_apps: " << installed_apps.size() << " m_apps_list: " << m_apps_list.size();
//    //To-Do: Need to test this logic.
//    beginResetModel();
//    m_unrestricted_apps_list.clear();
//    for (const auto& appInfo : m_apps_list) {
//        if (!installed_apps.contains(appInfo.m_package_name)) {
//          m_apps_list.remove(appInfo.m_package_name);
//        }
//        installed_apps.remove(appInfo.m_package_name);
//    }
//    if (installed_apps.size() > 0) {
//        for (const auto& appInfo : installed_apps) {
//            m_apps_list[appInfo.m_package_name] = appInfo;
//        }
//    }
//    endResetModel();
//}

void AppsListModel::toggleAppLock(const int index) {
    if (index < 0 || index >= (int)m_apps_list.size()) {
        qWarning() << TAG << __func__ << " Index out of bound: " << index;
        return;
    }
    qDebug() << TAG << __func__ << " index: " << index << " package: " << m_apps_list[index].m_package_name.c_str();
    m_apps_list[index].m_locked = !m_apps_list[index].m_locked;
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), QVector<int>() << AppsListRoles::LockedRole);
}

std::vector<AppInfo> AppsListModel::GetInstalledAppsList() const {
    return m_apps_list;
}
