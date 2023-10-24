#include "AppSettings.hpp"

/*!
   \class AppSettings
   \brief The AppSettings class handles writing and reading the app settings file.
   \inmodule FortiyMainApplication

   The AppSettings class provide interfaces to interact with app settings file.

*/

/*!
    \enum AppSettings::Key

    \value FavouriteInfo
           FavouriteInfo structure is saved in settings file
*/

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

/*!
   \fn static AppSettings* AppSettings::getInstance(std::shared_ptr<QSettings> settings)
   \brief The getInstance(\a settings) function takes QSettings object as an argument and creates AppSettings static instance.
*/
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

/*!
   \fn static void AppSettings::Remove(const Key key)
   \brief The Write(\a key) function removes the key value data from settings file.
*/
void AppSettings::Remove(const Key key) {
    getInstance()->m_app_settings->remove(QString::fromStdString(m_keys[key].first));
}

/*!
   \fn template <typename T> static void Write(const Key key, const T &value)
   \fn static void AppSettings::Write(const Key key, const std::string &value)
   \brief The Write(\a key, \a value) function takes enum as key and value of std::string type as arguments and writes it into settings file.
*/
void AppSettings::Write(const Key key, const std::string &value) {
    QVariant variant = QVariant::fromValue(QString::fromStdString(value));
    getInstance()->m_app_settings->setValue(QString::fromStdString(m_keys[key].first), variant);
}

/*!
   \fn template <typename T> static T Read(const Key key)
   \fn static std::string AppSettings::Read(const Key key)
   \brief The Read(\a key) function takes enum as key to retrieve data from settings file.
*/
std::string AppSettings::Read(const Key key) {
    QVariant variant = getInstance()->m_app_settings->value(QString::fromStdString(m_keys[key].first), m_keys[key].second);
    return variant.toString().toStdString();
}

/*!
   \fn static std::string AppSettings::GetString(const Key key)
   \brief The GetString(\a key) function takes enum as key to retrieve the string version of enum value.
*/
std::string AppSettings::GetString(const Key key) {
    return m_keys[key].first;
}
