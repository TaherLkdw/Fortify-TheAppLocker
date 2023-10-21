#pragma once

#include <string>
#include <vector>

class FavouriteInfo {
public:
    FavouriteInfo() {
        m_name = "";
        m_icon = "";
    }
    FavouriteInfo(const std::string& name, const std::string& icon = "") {
        m_name = name;
        m_icon = icon;
    }
    FavouriteInfo(const std::string& name, const std::vector<std::string>& unlocked_apps_list, const std::string& icon = "") {
        m_name = name;
        m_unlocked_apps_list = unlocked_apps_list;
        m_icon = icon;
    }
    FavouriteInfo(const FavouriteInfo& arg) {
        m_name = arg.m_name;
        m_unlocked_apps_list = arg.m_unlocked_apps_list;
        m_icon = arg.m_icon;
    }
    FavouriteInfo(const FavouriteInfo&& arg) {
        m_name = arg.m_name;
        m_unlocked_apps_list = arg.m_unlocked_apps_list;
        m_icon = arg.m_icon;
    }
    FavouriteInfo& operator=(const FavouriteInfo& arg) {
        if (&arg != this) {
            m_name = arg.m_name;
            m_unlocked_apps_list = arg.m_unlocked_apps_list;
            m_icon = arg.m_icon;
        }
        return *this;
    }
    FavouriteInfo& operator=(const FavouriteInfo&& arg) {
        if (&arg != this) {
            m_name = arg.m_name;
            m_unlocked_apps_list = arg.m_unlocked_apps_list;
            m_icon = arg.m_icon;
        }
        return *this;
    }
    bool operator<(const FavouriteInfo& arg) {
        return m_name.compare(arg.m_name) < 0;
    }
    bool operator>(const FavouriteInfo& arg) {
        return m_name.compare(arg.m_name) > 0;
    }

    std::string m_name;
    std::string m_icon;
    std::vector<std::string> m_unlocked_apps_list;
};
