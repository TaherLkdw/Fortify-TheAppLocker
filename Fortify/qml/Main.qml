import Felgo 4.0
import QtQuick 2.15
import "common"

App {
    // You get free licenseKeys from https://felgo.com/licenseKey
    // With a licenseKey you can:
    //  * Publish your games & apps for the app stores
    //  * Remove the Felgo Splash Screen or set a custom one (available with the Pro Licenses)
    //  * Add plugins to monetize, analyze & improve your apps (available with the Pro Licenses)
    licenseKey: "23AC3D3A12E92677CB47D120F31C41987DF9FEF9F197A30F0CB297D702A0E84BB0D79A9A375BE61309B09CFDFBA88E4C85776305C020207F1B47A459816A917335511278B17813D521F510BDAEB456BAAACCBFEC10F397AE5ACAD6A77A2D45AD7F7FA94B0537FCFAB432D18E0CC115E07FCC2EE40C8AD4F59186E655C060EE0C37221654152FC172037AE3E81A9A10DF8C3EAB8BF4668C1572A8AA07891B51ADB448E16A7B013B076E67D893E2C7BEDD2136627638D043D0BED9AEF4D9BD5CC287F2BF4B44A41F018305E7A584D7D9FB09B577CAFE0FCFC6A81464BD371A1E0DFADC4C446682A7DB24E944A20456BB9658F953EA4853D207BE25C7D00BD1DF6C25846E9270CB55504AF8246886A548AC045A89D6109F87D268F01EFD9583B21B9AB7A0056F794BE3740813CBEE68D0AB67DF429EF75279A818D692C9645BEF39EEF426B08EFC75F885683B3BBDC37F61"

    id: app
    visible: true
    width: 380
    height: 640

    property string background_color: "#bdcdd1"
    property string button_color: "#47D7D7"
    property string title_bar_color: "#10272E"
    property string text_color: "#3C4047"
    property string title_text_color: "#FAFDFD"
    property string grey: "#dddddd"
    property string light_grey: "#999999"
    property string shadow_grey_color: "#B3B3B3"
    property bool apps_locked: app_locker_ui_manager.isAppLockerRunning()

    Connections {
        target: app_locker_ui_manager
        onFavouriteViewUpdated: {
            interstitial.showInterstitialIfLoaded()
            installedAppsListView.updatingFavourite = true
            installedAppsListView.visible = true
        }
    }

    Rectangle {
        id: root
        anchors.fill: parent
        anchors.bottom: adMobBanner.top
        color: title_bar_color

        TopBar {
            id: top_bar
            header_text: qsTr("Fortify-TheAppLocker")
            back_button_visible: false
            anchors.topMargin: dp(28)
        }

        Rectangle {
            id: stopAppLockBackground
            color: app.background_color
            anchors.top: top_bar.bottom
            anchors.bottom: mainActionBar.bottom
            width: parent.width
            visible: app.apps_locked

            Rectangle {
                id: stopAppLockFavouriteButton
                width: parent.width / 2
                height: (app.height/6) / 2
                anchors.centerIn: parent
                color: button_color
                radius: width/24

                Text {
                    id: stopAppLockFavouriteText
                    text: qsTr("Stop app lock")
                    anchors.centerIn: parent
                    font.bold: true
                    font.pointSize: sp(16)
                    color: app.text_color
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        app.apps_locked = false
                        app_locker_ui_manager.unlockApps()
                    }
                }
            }
        }

        FavouriteListView {
            id: favouriteListView
            anchors.top: top_bar.bottom
            anchors.bottom: mainActionBar.top
            visible: !app.apps_locked && !installedAppsListView.visible
            onEditFavourite: (favourite_name, selected_index) => {
                installedAppsListView.favouriteName = favourite_name //First set the name then update page
                app_locker_ui_manager.updateFavouriteAppsView(selected_index)
            }
        }

        InstalledAppsListView {
            id: installedAppsListView
            anchors.top: top_bar.top
            anchors.bottom: mainActionBar.bottom
            visible: false
        }

        Rectangle {
            id: mainActionBar
            width: parent.width
            height: (app.height/6)/2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: dp(28)
            visible: !app.apps_locked && !installedAppsListView.visible
            color: app.background_color

            Rectangle {
                id: addFavouriteButton
                width: (parent.width / 2) - dp(25)
                height: parent.height - dp(5)
                anchors.left: parent.left
                anchors.leftMargin: dp(20)
                anchors.top: parent.top
                color: app.button_color
                radius: width/24

                Text {
                    id: addFavouriteText
                    text: qsTr("Add New Favourite")
                    anchors.centerIn: parent
                    font.bold: true
                    font.pointSize: sp(18)
                    color: app.text_color
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        app_locker_ui_manager.updateQuickLockView()
                        interstitial.showInterstitialIfLoaded()
                        installedAppsListView.addingNewFavourite = true
                        installedAppsListView.visible = true
                    }
                }
            }

            Rectangle {
                id: quickLockButton
                width: (parent.width / 2) - dp(25)
                height: parent.height - dp(5)
                anchors.right: parent.right
                anchors.rightMargin: dp(20)
                anchors.top: parent.top
                color: app.button_color
                radius: width/24

                Text {
                    id: quickLockText
                    text: qsTr("Quick lock")
                    anchors.centerIn: parent
                    font.bold: true
                    font.pointSize: sp(18)
                    color: app.text_color
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        app_locker_ui_manager.updateQuickLockView()
                        installedAppsListView.addingNewFavourite = false
                        installedAppsListView.visible = true
                    }
                }
            }
        }
    }

    // Plugin Item
    AdMobBanner {
        id: adMobBanner
        adUnitId: "ca-app-pub-7369013909167745/1490414716"
        banner: AdMobBanner.Smart

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        testDeviceIds: ["46AB44CA8FD7D0E913476B95FBDF029D"]

    }

    // Plugin Item
    AdMobInterstitial {
        id: interstitial
        adUnitId: "ca-app-pub-9155324456588158/9075427220" //To-Do: replace with your AdMob adUnitId
        onPluginLoaded: loadInterstitial()
    }
}
