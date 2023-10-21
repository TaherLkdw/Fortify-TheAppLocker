#pragma once

#include "FavouriteInfo.hpp"

#include <QAbstractListModel>
#include <vector>

class FavouriteListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum FavouriteListRoles {
        NameRole = Qt::UserRole,
        IconRole
    };

    explicit FavouriteListModel();

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

    void PopulateFavouriteList(const std::vector<FavouriteInfo>& favourite_info_list);
    void AddFavourite(const FavouriteInfo& favourite_info);
    void RemoveFavourite(const int index);
    void updateUnlockedAppsList(const std::vector<std::string>& unlocked_apps_list, const int index);
    std::vector<std::string> GetUnlockedAppsListByIndex(const int index);

private:
    std::vector<FavouriteInfo> m_favourite_list;

    QString ConvertFavouriteInfoToJsonString(const FavouriteInfo& favourite_info);
    std::shared_ptr<FavouriteInfo> ConvertJsonStringToFavouriteInfo(const QString& data);
    void SaveFavouriteListInSettings();
    void ReadFavouriteListFromSettings();
};
