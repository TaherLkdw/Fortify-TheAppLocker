#include "AppSettings.hpp"

std::map<AppSettings::Key, std::pair<std::string,QVariant>> AppSettings::m_keys = {
    { Key::FavouriteInfo,                   std::pair<std::string,QVariant>("favourite_info", QString()) }
};

std::shared_ptr<QSettings>  AppSettings::m_app_settings = nullptr;
AppSettings* AppSettings::singleton_instance = nullptr;

AppSettings* AppSettings::getInstance() {
    if (singleton_instance == nullptr) {
        singleton_instance = new AppSettings();
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, []{
            delete singleton_instance;
            singleton_instance = nullptr;
        });
    }
    return singleton_instance;
}

AppSettings* AppSettings::getInstance(std::shared_ptr<QSettings> settings) {
    if (singleton_instance == nullptr) {
        singleton_instance = new AppSettings(settings);
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, []{
            delete singleton_instance;
            singleton_instance = nullptr;
        });
    }
    return singleton_instance;
}


void AppSettings::Remove(const Key key) {
    getInstance()->m_app_settings->remove(QString::fromStdString(m_keys[key].first));
}

void AppSettings::Write(const Key key, const std::string &value) {
    QVariant variant = QVariant::fromValue(QString::fromStdString(value));
    getInstance()->m_app_settings->setValue(QString::fromStdString(m_keys[key].first), variant);
}

std::string AppSettings::Read(const Key key) {
    QVariant variant = getInstance()->m_app_settings->value(QString::fromStdString(m_keys[key].first), m_keys[key].second);
    return variant.toString().toStdString();
}

std::string AppSettings::GetString(const Key key) {
    return m_keys[key].first;
}
