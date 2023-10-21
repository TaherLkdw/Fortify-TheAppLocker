import QtQuick 2.12

Rectangle {
    id: header
    property alias header_text: headerText.text
    property alias back_button_visible: backButtonIcon.visible
    property int font_size: sp(32)
    property bool bold: true

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
