#pragma once

#include <QCoreApplication>
#include <QSettings>

#include <memory>
#include <sstream>

class AppSettings {
    Q_GADGET
public :

    enum class Key {
        FavouriteInfo
    };

    ~AppSettings() = default;

    // Note: This is used only for test cases
    static AppSettings* getInstance(std::shared_ptr<QSettings> settings);

    template <typename T>
    static void Write(const Key key, const T &value) {
        Q_STATIC_ASSERT_X(QMetaTypeId2<T>::Defined,"Type is not registered as QMetatype");
        QVariant variant = QVariant::fromValue(value);
        getInstance()->m_app_settings->setValue(QString::fromStdString(m_keys[key].first),variant);
        getInstance()->m_app_settings->sync();
    }

    template <typename T>
    static T Read(const Key key) {
        Q_STATIC_ASSERT_X(QMetaTypeId2<T>::Defined,"Type is not registered as QMetatype");
        QVariant variant = getInstance()->m_app_settings->value(QString::fromStdString(m_keys[key].first), m_keys[key].second);
        return variant.value<T>();
    }

    static std::string Read(const Key key);
    static void Remove(const Key key);
    static void Write(const Key key, const std::string &value);

   static std::string GetString(const Key key);

private:
    AppSettings() {
        m_app_settings = std::make_shared<QSettings>(QStringLiteral("Lkdw Card game"), QStringLiteral("3 Of Spades"));
    }

    // Note: This is used only for test cases
    AppSettings(std::shared_ptr<QSettings> settings) {
        m_app_settings.swap(settings);
    }

    static AppSettings* getInstance();

    static std::shared_ptr<QSettings>  m_app_settings;
    static std::map<Key, std::pair<std::string,QVariant>> m_keys;
    static AppSettings* singleton_instance;
};
