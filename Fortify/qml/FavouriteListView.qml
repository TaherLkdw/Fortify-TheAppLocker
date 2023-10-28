import QtQuick 2.12
import "common"

/*!
    \qmltype InstalledAppsListView
    \inqmlmodule Qml-FortiyMainApplication
    \inherits Rectangle
    \brief This class provides the ListView of the name of Favourites created by user.

    This class can be instantiated as shown below:
    \qml
        FavouriteListView {
            id: favouriteListView
            anchors.top: top_bar.bottom
            anchors.bottom: mainActionBar.top
            visible: !app.apps_locked && !installedAppsListView.visible
        }
    \endqml
*/

Rectangle {
    id: root
    width: parent.width
    color: app.background_color

    /*!
        \qmlsignal FavouriteListView::editFavourite(var favourite_name)
        \brief The editFavourite(\a favourite_name) signal is emitted when user selects any favourite to edit.
    */
    signal editFavourite(var favourite_name)

    TopBar {
        id: favourite_top_bar
        header_text: qsTr("Favourites")
        back_button_visible: false
        font_size: sp(20)
        onBackButtonClicked: root.visible = false
    }

    Rectangle {
        id: noFavouriteBackground
        width: parent.width
        anchors.top: favourite_top_bar.bottom
        anchors.bottom: parent.bottom
        visible: favouriteListView.count === 0
        color: app.background_color

        Text {
            id: noFavouriteText
            text: qsTr("No favourites added, please create favourites")
            width: parent.width - dp(20)
            height: Math.max(implicitHeight, noFavouriteText.paintedHeight)
            anchors.centerIn: parent
            color: app.title_bar_color
            font.bold: true
            font.italic: true
            font.pointSize: sp(18)
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
        }
    }

    ListView {
        id: favouriteListView
        visible: favouriteListView.count > 0
        width: parent.width - dp(20)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: favourite_top_bar.bottom
        anchors.topMargin: app.dp(10)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: app.dp(10)
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        pressDelay: interactive ? 20 : 0 // don't highlight a row when flicking
        interactive: contentHeight > height
        maximumFlickVelocity: 100000
        model: app_locker_ui_manager.favouriteListModel
        spacing: dp(10)

        delegate: Item {
            id: favouriteDelegateContainer
            width: favouriteListView.width
            height: (favouriteListView.height / 6) - (favouriteListView.spacing * 5)

            Rectangle {
                id: favouriteDelegate
                anchors.fill: parent
                border.color: app.title_bar_color
                border.width: dp(1)
                radius: width/30
                color: "white"

                Item {
                    id: editButtonContainer
                    width: editButton.width * 3
                    height: parent.height
                    anchors.right: deleteButton.left

                    Image {
                        id: editButton
                        width: height
                        height: parent.height / 3
                        anchors.centerIn: parent
                        clip: true
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/qml/assets/edit_icon.png"

                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            app_locker_ui_manager.updateFavouriteAppsView(index)
                            editFavourite(model.nameRole)
                        }
                    }
                }

                Item {
                    id: deleteButton
                    width: deleteIcon.width * 2
                    height: parent.height
                    anchors.right: parent.right

                    Image {
                        id: deleteIcon
                        width: height
                        height: parent.height / 2
                        anchors.centerIn: parent
                        clip: true
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        source: "qrc:/qml/assets/delete_icon.png"

                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            app_locker_ui_manager.removeFavourite(index)
                        }
                    }
                }


                Text {
                    id: favouriteName
                    text: model.nameRole
                    height: Math.max(implicitHeight, favouriteName.paintedHeight)
                    anchors.left: parent.left
                    anchors.leftMargin: app.dp(10)
                    anchors.right: editButtonContainer.left
                    anchors.rightMargin: app.dp(10)
                    anchors.verticalCenter: parent.verticalCenter
                    color: app.text_color
                    font.bold: true
                    font.pointSize: sp(20)
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                    wrapMode: Text.WordWrap

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            app_locker_ui_manager.lockFavourite(index)
                            app.apps_locked = true
                        }
                    }
                }
            }
        }
    }
}
