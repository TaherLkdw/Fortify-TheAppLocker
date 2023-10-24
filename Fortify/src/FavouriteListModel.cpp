#include "AppSettings.hpp"
#include "FavouriteListModel.hpp"

#include <QJsonDocument>
#include <QJsonObject>

/*!
   \class FavouriteListModel
   \brief The FavouriteListModel class is a list model class to provide data in qml.
   \inmodule FortiyMainApplication

   The FavouriteListModel class provides interfaces to provide data to qml ListView.
*/

/*!
    \enum FavouriteListModel::FavouriteListRoles

    \value NameRole
           Name of the favourite.
    \value IconRole
           Icon of the favourite.
*/

/*!
 * \brief Constructor reads the app settings and updates the favourite list when app starts.
 */
FavouriteListModel::FavouriteListModel() {
    ReadFavouriteListFromSettings();
}

/*! \reimp */
QVariant FavouriteListModel::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= (int)m_favourite_list.size()) {
        return QVariant();
    }

    switch (role) {
        case NameRole: {
            return m_favourite_list[index.row()].m_name.c_str();
        }
        case IconRole: {
            return m_favourite_list[index.row()].m_icon.c_str();
        }
        default:
            return QVariant();
    }
    return QVariant();
}

/*! \reimp */
int FavouriteListModel::rowCount(const QModelIndex&) const {
    return m_favourite_list.size();
}

/*! \reimp */
QHash<int, QByteArray> FavouriteListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "nameRole";
    roles[IconRole] = "iconRole";
    return roles;
}

/*!
   \fn void FavouriteListModel::PopulateFavouriteList(const std::vector<FavouriteInfo>& favourite_info_list)
   \brief The PopulateFavouriteList(\a favourite_info_list) function copies the favourite info list and resets the model.
*/
void FavouriteListModel::PopulateFavouriteList(const std::vector<FavouriteInfo>& favourite_info_list) {
    beginResetModel();
    m_favourite_list = favourite_info_list;
    endResetModel();
}

/*!
   \fn void FavouriteListModel::AddFavourite(const FavouriteInfo& favourite_info)
   \brief The AddFavourite(\a favourite_info) function inserts favourite info element in the list, updates the view and save in the app settings.
*/
void FavouriteListModel::AddFavourite(const FavouriteInfo& favourite_info) {
    beginInsertRows(QModelIndex(), m_favourite_list.size(), m_favourite_list.size());
    m_favourite_list.push_back(favourite_info);
    endInsertRows();
    SaveFavouriteListInSettings();
}

/*!
   \fn void FavouriteListModel::RemoveFavourite(const int index)
   \brief The RemoveFavourite(\a index) function removes favourite info element from the list, updates the view and save in the app settings.
*/
void FavouriteListModel::RemoveFavourite(const int index) {
    if (index < 0 || index >= (int)m_favourite_list.size()) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_favourite_list.erase(m_favourite_list.begin()+index);
    endRemoveRows();
    SaveFavouriteListInSettings();
}

/*!
   \fn void FavouriteListModel::updateUnlockedAppsList(const std::vector<std::string>& unlocked_apps_list, const int index)
   \brief The updateUnlockedAppsList(\a unlocked_apps_list, \a index) function copies the unlocked apps list names and save in the app settings.
*/
void FavouriteListModel::updateUnlockedAppsList(const std::vector<std::string>& unlocked_apps_list, const int index) {
    if (index < 0 || index >= (int)m_favourite_list.size()) {
        return;
    }
    m_favourite_list[index].m_unlocked_apps_list = unlocked_apps_list;
    SaveFavouriteListInSettings();
}

/*!
   \fn std::vector<std::string> FavouriteListModel::GetUnlockedAppsListByIndex(const int index)
   \brief The GetUnlockedAppsListByIndex(\a index) function returns the unlocked apps list names by index from the list.
*/
std::vector<std::string> FavouriteListModel::GetUnlockedAppsListByIndex(const int index) {
    if (index < 0 || index >= (int)m_favourite_list.size()) {
        return std::vector<std::string>();
    }
    return m_favourite_list[index].m_unlocked_apps_list;
}

QString FavouriteListModel::ConvertFavouriteInfoToJsonString(const FavouriteInfo& favourite_info) {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(favourite_info.m_name);
    obj["icon"] = QString::fromStdString(favourite_info.m_icon);
    QJsonObject unlocked_apps_json_obj;
    for (unsigned int i = 0; i < favourite_info.m_unlocked_apps_list.size(); ++i) {
        unlocked_apps_json_obj[QString::number(i)] = QString::fromStdString(favourite_info.m_unlocked_apps_list[i]);
    }
    obj["unlocked_apps"] = unlocked_apps_json_obj;

    QJsonDocument doc(obj);
    auto json_doc = doc.toJson();
    return QString(json_doc.data());
}

std::shared_ptr<FavouriteInfo> FavouriteListModel::ConvertJsonStringToFavouriteInfo(const QString& data) {
    std::shared_ptr<FavouriteInfo> favourite_info = std::make_shared<FavouriteInfo>();
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data.toStdString().c_str());
        QJsonObject obj = doc.object();
        QVariantMap json_map = obj.toVariantMap();

        if (data == nullptr || json_map.isEmpty()) {
            qDebug() <<__func__ << " Failed to convert, Data sent for conversion is null.";
            return nullptr;
        }
        favourite_info->m_name = json_map["name"].toString().toStdString();
        favourite_info->m_icon = json_map["icon"].toString().toStdString();
        auto unlocked_apps_json_map = json_map["unlocked_apps"].toJsonObject().toVariantMap();
        for (unsigned int i = 0; i < unlocked_apps_json_map.size(); ++i) {
            favourite_info->m_unlocked_apps_list.push_back(unlocked_apps_json_map[QString::number(i)].toString().toStdString());
        }

    } catch (std::exception e) {
        qDebug() << __func__ << " Failed to convert: " << e.what();
    }
    return favourite_info;
}

void FavouriteListModel::SaveFavouriteListInSettings() {
    QJsonObject favourite_list_obj;
    for (unsigned int i = 0; i < m_favourite_list.size(); ++i) {
        auto favourite_info_str = ConvertFavouriteInfoToJsonString(m_favourite_list[i]);
        favourite_list_obj[QString::number(i)] = favourite_info_str;
    }
    QJsonDocument doc(favourite_list_obj);
    auto json_doc = doc.toJson();
    qDebug() <<__func__ << " data to save: " << QString::fromLatin1(json_doc.data());
    AppSettings::Write(AppSettings::Key::FavouriteInfo, QString(json_doc.data()));
}

void FavouriteListModel::ReadFavouriteListFromSettings() {
    const auto& favourite_info_list_str = AppSettings::Read(AppSettings::Key::FavouriteInfo);
    try {
        qDebug() <<__func__ << " reading data: " << QString::fromStdString(favourite_info_list_str);
        QJsonDocument doc = QJsonDocument::fromJson(favourite_info_list_str.c_str());
        QJsonObject obj = doc.object();
        QVariantMap json_map = obj.toVariantMap();

        if (favourite_info_list_str.empty() || json_map.isEmpty()) {
            qDebug() <<__func__ << " Failed to convert, Data sent for conversion is null.";
            return;
        }

        for (unsigned int i = 0; i < json_map.size(); ++i) {
            auto favourite_info = ConvertJsonStringToFavouriteInfo(json_map[QString::number(i)].toString());
            m_favourite_list.push_back(*favourite_info);
        }
        //To populate list on UI. Working same as PopulateFavouriteList function;
        beginResetModel();
        endResetModel();

    } catch (std::exception e) {
        qDebug() << __func__ << " Failed to convert: " << e.what();
    }
}
