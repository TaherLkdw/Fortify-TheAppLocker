import QtQuick 2.12

/*!
    \qmltype Topbar
    \inqmlmodule Qml-FortiyMainApplication
    \inherits Rectangle
    \brief This class provides the top bar to show headers and back button.

    The usage of this class is as shown below::
    \qml
        TopBar {
            id: favourite_top_bar
            header_text: qsTr("Fortify-TheAppLocker")
            back_button_visible: false
            font_size: sp(20)
        }
    \endqml
*/

Rectangle {
    id: header

    /*!
        \qmlproperty alias Topbar::header_text
        \brief This property is used to provides reference property for header text component's text property.
    */
    property alias header_text: headerText.text

    /*!
        \qmlproperty alias Topbar::back_button_visible
        \brief This property is used to show or hide back button.
    */
    property alias back_button_visible: backButtonIcon.visible

    /*!
        \qmlproperty int Topbar::font_size
        \brief This property is used to modify font size of the header text.
    */
    property int font_size: sp(32)

    /*!
        \qmlproperty bool Topbar::bold
        \brief This property is used to modify font.bold proprty of the header text.
    */
    property bool bold: true

    /*!
        \qmlsignal Topbar::backButtonClicked()
        \brief This signal is emitted when back button is clicked.
    */
    signal backButtonClicked()

    width: parent.width
    height: (parent.height/6)/2
    anchors.top: parent.top
    color: app.title_bar_color

    Image {
        id: backButtonIcon
        source: "qrc:/qml/assets/back_icon.png"
        width: height
        height: parent.height / 3
        anchors.verticalCenter: headerText.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: dp(10)
        clip: true
        mipmap: true
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onClicked: {
                backButtonClicked()
                root.visible = false
            }
        }
    }

    Text {
        id: headerText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: backButtonIcon.visible ? backButtonIcon.right : parent.left
        anchors.leftMargin: dp(20)
        color: app.title_text_color
        font.bold: bold
        font.pointSize: font_size
    }
}
