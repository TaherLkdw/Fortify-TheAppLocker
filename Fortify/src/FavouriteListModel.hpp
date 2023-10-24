#pragma once

#include "FavouriteInfo.hpp"

#include <QAbstractListModel>
#include <vector>

class FavouriteListModel : public QAbstractListModel {
    Q_OBJECT

    enum FavouriteListRoles {
        NameRole = Qt::UserRole,
        IconRole
    };

public:
    explicit FavouriteListModel();
    void PopulateFavouriteList(const std::vector<FavouriteInfo>& favourite_info_list);
    void AddFavourite(const FavouriteInfo& favourite_info);
    void RemoveFavourite(const int index);
    void updateUnlockedAppsList(const std::vector<std::string>& unlocked_apps_list, const int index);
    std::vector<std::string> GetUnlockedAppsListByIndex(const int index);

protected:
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex&) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<FavouriteInfo> m_favourite_list;

    QString ConvertFavouriteInfoToJsonString(const FavouriteInfo& favourite_info);
    std::shared_ptr<FavouriteInfo> ConvertJsonStringToFavouriteInfo(const QString& data);
    void SaveFavouriteListInSettings();
    void ReadFavouriteListFromSettings();
};
