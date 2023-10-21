#pragma once

#include <string>

class AppInfo {
public:
    AppInfo() {
    }
    AppInfo(const std::string& package, const std::string& label, const std::string& icon = "") {
        m_package_name = package;
        m_label = label;
        m_icon = icon;
    }
    AppInfo(const AppInfo& arg) {
        m_package_name = arg.m_package_name;
        m_label = arg.m_label;
        m_icon = arg.m_icon;
        m_locked = arg.m_locked;
    }
    AppInfo(const AppInfo&& arg) {
        m_package_name = arg.m_package_name;
        m_label = arg.m_label;
        m_icon = arg.m_icon;
        m_locked = arg.m_locked;
    }
    AppInfo& operator=(const AppInfo& arg) {
        if (this != &arg) {
            m_package_name = arg.m_package_name;
            m_label = arg.m_label;
            m_icon = arg.m_icon;
            m_locked = arg.m_locked;
        }
        return *this;
    }
    AppInfo& operator=(const AppInfo&& arg) {
        if (this != &arg) {
            m_package_name = arg.m_package_name;
            m_label = arg.m_label;
            m_icon = arg.m_icon;
            m_locked = arg.m_locked;
        }
        return *this;
    }
    bool operator!=(const AppInfo& arg) {
        if (this != &arg) {
            return m_package_name != arg.m_package_name;
        }
        return false;
    }
    bool operator<(const AppInfo& arg) {
        return m_label.compare(arg.m_label) < 0;
    }
    bool operator>(const AppInfo& arg) {
        return m_label.compare(arg.m_label) > 0;
    }
    std::string m_package_name;
    std::string m_label;
    std::string m_icon;
    bool m_locked {true};
};
