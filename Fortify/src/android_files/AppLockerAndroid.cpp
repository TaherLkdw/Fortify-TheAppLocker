#include "AppLockerAndroid.hpp"

//#include <QtAndroid>
//#include <QtAndroidExtras>
#include <QDebug>
#include <QJniObject>
#include <QJniEnvironment>
#include <QtCore/private/qandroidextras_p.h>

AppLockerAndroid::AppLockerAndroid() {
}

//To-Do: Need to optimise this function.
std::map<std::string, std::string> AppLockerAndroid::GetInstalledApps() {
    std::map<std::string, std::string> installed_apps;

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    auto installed_apps_obj = activity.callObjectMethod("GetInstalledApps", "()Ljava/util/Map;");
    auto installed_apps_jni_obj = installed_apps_obj.object<jobject>();
    if (installed_apps_jni_obj == nullptr) {
        qDebug() << TAG << __func__ << " Returned installed_apps_jni_obj is null.";
        return installed_apps;
    }

    QJniEnvironment env;
    jclass map_class = env->FindClass("java/util/Map");
    jclass set_class = env->FindClass("java/util/Set");
    jclass iterator_class = env->FindClass("java/util/Iterator");
    jclass map_entry_class = env->FindClass("java/util/Map$Entry");

    jmethodID map_method_id = env->GetMethodID(map_class, "size", "()I");
    int map_size = env->CallIntMethod(installed_apps_jni_obj, map_method_id);

    qDebug() << TAG << __func__ << " Number of installed apps found are: " << map_size;
    if (map_size > 0) {
        jmethodID entry_method_id = env->GetMethodID(map_class, "entrySet", "()Ljava/util/Set;");

        jobject entry_set = env->CallObjectMethod(installed_apps_jni_obj, entry_method_id);
        if (entry_set == nullptr) {
            qDebug() << TAG << __func__ << " Failed to get entry_set from Map.";
            return installed_apps;
        }

        jmethodID iterator_method_id = env->GetMethodID(set_class, "iterator", "()Ljava/util/Iterator;");
        jobject iterator_obj = env->CallObjectMethod(entry_set, iterator_method_id);
        if (iterator_obj == nullptr) {
            qDebug() << TAG << __func__ << " Failed to get iterator_obj from Set.";
            return installed_apps;
        }

        jmethodID hasNext_method_id = env->GetMethodID(iterator_class, "hasNext", "()Z");

        for (int i = 0; i < map_size; ++i) {
            if (env->CallBooleanMethod(iterator_obj, hasNext_method_id)) {
                jmethodID next_method_id = env->GetMethodID(iterator_class, "next", "()Ljava/lang/Object;");
                jobject entry = env->CallObjectMethod(iterator_obj, next_method_id);
                if (entry == nullptr) {
                    qDebug() << TAG << __func__ << " Failed to get object of entry at index: " << i;
                    continue;
                }

                jmethodID get_key_method_id = env->GetMethodID(map_entry_class, "getKey", "()Ljava/lang/Object;");
                jstring key = static_cast<jstring>(env->CallObjectMethod(entry, get_key_method_id));
                if (key == nullptr) {
                    qDebug() << TAG << __func__ << " Failed to get key of entry at index: " << i;
                    continue;
                }
                char const* native_key = env->GetStringUTFChars(key, nullptr);
                if (native_key == nullptr) {
                    qDebug() << TAG << __func__ << " Failed to get string of key of entry at index: " << i;
                    continue;
                }

                jmethodID get_value_method_id = env->GetMethodID(map_entry_class, "getValue", "()Ljava/lang/Object;");
                jstring value = static_cast<jstring>(env->CallObjectMethod(entry, get_value_method_id));
                if (value == nullptr) {
                    qDebug() << TAG << __func__ << " Failed to get value at index: " << i;
                    installed_apps[native_key] = "";
                    continue;
                }
                char const* native_value = env->GetStringUTFChars(value, nullptr);
                if (native_value == nullptr) {
                    qDebug() << TAG << __func__ << " Failed to get string of value of entry at index: " << i;
                    continue;
                }
                installed_apps[native_key] = native_value;
//                env->ReleaseStringUTFChars(key, native_key);
                env->DeleteLocalRef(key);
//                env->ReleaseStringUTFChars(value, native_key);
                env->DeleteLocalRef(value);
            }
        }
    }
    qDebug() << TAG << __func__ << " after extract installed_apps: " << installed_apps.size();
    return installed_apps;
}

void AppLockerAndroid::LockApps(const std::vector<std::string>& apps_list) {
    jobjectArray object_array = 0;
    QJniEnvironment env;

    object_array = env->NewObjectArray(apps_list.size(), env->FindClass("java/lang/String"), 0);

    for(unsigned int i = 0; i < apps_list.size(); ++i) {
        jstring str = env->NewStringUTF(apps_list[i].c_str());
        env->SetObjectArrayElement(object_array, i, str);
    }
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    activity.callMethod<void>("LockApps", "([Ljava/lang/String;)V", object_array);
}

void AppLockerAndroid::UnlockApps() {
     QJniObject activity = QNativeInterface::QAndroidApplication::context();
     activity.callMethod<void>("UnlockApps", "()V");
}

void AppLockerAndroid::SetInstalledAppIconsDirectoryPath(const std::string& path) {
    QJniEnvironment env;
    jstring str = env->NewStringUTF(path.c_str());
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    activity.callMethod<void>("SetInstalledAppIconsDirectoryPath", "(Ljava/lang/String;)V", str);
}
