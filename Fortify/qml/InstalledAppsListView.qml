import QtQuick 2.12
import "common"

/*!
    \qmltype InstalledAppsListView
    \inqmlmodule Qml-FortiyMainApplication
    \inherits Rectangle
    \brief This class provides the GridView of the installed apps on device.

    The usage of this class is as shown below::
    \qml
        InstalledAppsListView {
            id: installedAppsListView
            anchors.top: top_bar.top
            anchors.bottom: mainActionBar.bottom
            visible: false
        }
    \endqml
*/

Rectangle {
    id: root
    width: parent.width
    color: app.background_color

    /*!
        \qmlproperty bool InstalledAppsListView::addingNewFavourite
        \brief This property is used to inform InstalledAppsListView that user is adding a new favourite.
    */
    property bool addingNewFavourite: false

    /*!
        \qmlproperty bool InstalledAppsListView::updatingFavourite
        \brief This property is used to inform InstalledAppsListView that user is updating an existing favourite.
    */
    property bool updatingFavourite: false

    /*!
        \qmlproperty string InstalledAppsListView::favouriteName
        \brief This property is used to save the favourite name set by user.
    */
    property string favouriteName: ""

    TopBar {
        id: top_bar
        header_text: addingNewFavourite ? qsTr("Add New Favourite") : (updatingFavourite ? qsTr("Updating Favourite") : qsTr("Quick Lock"))
        font_size: sp(28)
        onBackButtonClicked: {
            root.favouriteName = ""
            addingNewFavourite = false
            updatingFavourite = false
            root.visible = false
        }
    }

    InputField {
        id: favouriteNameInputField
        width: parent.width
        height: visible ? (parent.height/6)/2 - dp(10) : 0
        anchors.top: top_bar.bottom
        visible: root.addingNewFavourite || root.updatingFavourite
        placeHolderText: qsTr("Favourite Title")
        inputText: root.updatingFavourite ? root.favouriteName : ""
        onTextAccepted: (text) => {
            root.favouriteName = text
        }
    }

    GridView {
        id: appsListView
        anchors.top: favouriteNameInputField.bottom
        anchors.topMargin: dp(10)
        anchors.bottom: quickLockButton.top
        anchors.bottomMargin: dp(5)
        width: parent.width
        model: app_locker_ui_manager.appsListModel
        boundsBehavior: Flickable.StopAtBounds
        cellWidth: appsListView.width / 4
        cellHeight: (appsListView.height / 6)
        clip: true
        pressDelay: interactive ? 20 : 0 // don't highlight a row when flicking
        interactive: contentHeight > height
        maximumFlickVelocity: 100000

        delegate: Item {
            id: appDelegateContainer
            width: appsListView.cellWidth
            height: appsListView.cellHeight

            Rectangle {
                id: appDelegate
                width: appsListView.cellWidth - dp(15)
                height: appsListView.cellHeight - dp(15)
                anchors.centerIn: parent
                border.color: app.title_bar_color
                border.width: model.lockedRole ? dp(1) : 0
                radius: width/12
                color: "white"

                Item {
                    id: checkBox
                    width: parent.width/4
                    height: width
                    anchors.horizontalCenter: parent.right
                    anchors.verticalCenter: parent.top
                    visible: model.lockedRole
                    Image {
                        id: lockIcon
                        source: "qrc:/qml/assets/lock_icon.png"
                        anchors.fill: parent
                        clip: true
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                    }
                }

                Image {
                    id: icon
                    source: "file://" + app_locker_ui_manager.installedAppIconsDirPath + model.labelRole + ".png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: appName.top
                    anchors.bottomMargin: dp(5)
                    height: Math.max(parent.height/1.5, parent.width/1.5)
                    width: height
                    clip: true
                    mipmap: true
                    fillMode: Image.PreserveAspectFit
                }

                Text {
                    id: appName
                    text: model.labelRole
                    width: parent.width - dp(5)
                    height: Math.max(implicitHeight, appName.paintedHeight)
                    anchors.horizontalCenter: icon.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: dp(5)
                    color: app.text_color
                    font.bold: true
                    font.pointSize: sp(8)
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        app_locker_ui_manager.toggleAppLock(index)
                    }
                }
            }
        }
    }

    Rectangle {
        id: quickLockButton
        width: parent.width / 2
        height: (app.height/6) / 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: dp(5)
        anchors.horizontalCenter: parent.horizontalCenter
        color: app.button_color
        radius: width/24

        Text {
            id: saveFavouriteText
            text: addingNewFavourite ? qsTr("Add New Favourite") : (updatingFavourite ? qsTr("Update Favourite") : qsTr("Lock Apps"))
            anchors.centerIn: parent
            font.bold: true
            font.pointSize: sp(18)
            color: app.text_color
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (addingNewFavourite) {
                    addingNewFavourite = false
                    app_locker_ui_manager.addFavourite(favouriteName)
                } else if (updatingFavourite) {
                    updatingFavourite = false
                    app_locker_ui_manager.updateFavourite()
                } else {
                    app_locker_ui_manager.quickLockApps()
                    app.apps_locked = true
                }
                root.favouriteName = ""
                root.visible = false
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            favouriteNameInputField.focus = false
            mouse.accepted = false
        }
    }
}
