import Felgo 4.0
import QtQuick 2.15
import "common"

App {
    // You get free licenseKeys from https://felgo.com/licenseKey
    // With a licenseKey you can:
    //  * Publish your games & apps for the app stores
    //  * Remove the Felgo Splash Screen or set a custom one (available with the Pro Licenses)
    //  * Add plugins to monetize, analyze & improve your apps (available with the Pro Licenses)
    licenseKey: "2D22691797E3DF842A438B1D1BD57B29AC9BC318300A6886F5685DFB344F40BF67B9CD07A513B12C79D905142ABE0CAD64A964C05CB1C697D7D77AFECE8E3A097F3E1E4884FD67530FAA0CA1E78ED544415CD6101619117C4B313B277F21BCE4387A85B4B67512EF604F15ABE1EF5E8362AD640F3AE2D41F1BAB74F6C42EDE3B9F01BE451207A3B8D0E4061C34764395435A16E869B71DEF3DC255D498C67486BB2F6301D5AC78465C419CE1D27009B7BCFEA6E8F39DECDFB2557A6696FAF34927F471AA7E238DD5201E3B9EB6AD64F57EEE3673DE0E7AC5DBA5BBB2298EF6A2DDD6C8BC1F79AB83603680194288187F3635FF6AC9F7831E8EDD22ABE0D7B5BA48A6B115BAF6A1C0A2C1CE9016AEDAB9039BE27F4A872C54DB0AF9A3FF045F3430031C11B8D7C009155E63D0013BAA4B8B0F19EC01F03ACA491DF09A440F5769"

    id: app
    visible: true
    width: 380
    height: 640

    property string button_color: "#47D7D7"
    property string title_bar_color: "#10272E"
    property string text_color: "#3C4047"
    property string title_text_color: "#FAFDFD"
    property string grey: "#dddddd"
    property string light_grey: "#999999"
    property string shadow_grey_color: "#B3B3B3"
    property bool apps_locked: false

    Connections {
        target: app_locker_ui_manager
        onFavouriteViewUpdated: {
            installedAppsListView.updatingFavourite = true
            installedAppsListView.visible = true
        }
    }

    Rectangle {
        id: root
        anchors.fill: parent
        color: title_bar_color

        TopBar {
            id: top_bar
            header_text: qsTr("AppLocker")
            back_button_visible: false
            anchors.topMargin: dp(28)
        }

        Rectangle {
            id: stopAppLockBackground
            color: 'white'
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
            color: 'white'

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
}
