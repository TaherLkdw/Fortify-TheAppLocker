import QtQuick 2.12
import QtQuick.Controls 2.2
import Felgo 4.0

import "../"

FocusScope {
    id: root
    signal textAccepted(var text)
    property alias placeHolderText: textInput.placeholderText
    focus: false

    Rectangle {
        id: text_background
        anchors.fill: parent
        color: app.title_bar_color

        AppTextInput {
          id: textInput
          focus: root.focus
          width: dp(200)
          placeholderText: qsTr("Title")
          placeholderColor: app.light_grey
          cursorColor: app.title_text_color
          color: app.title_text_color
          anchors.fill: parent
          anchors.leftMargin: dp(10)
          font.pixelSize: sp(23)
          font.bold: true
          font.family: "Tahoma"
          enabled: true
          activeFocusOnTab: true
          activeFocusOnPress: true
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignLeft

          onAccepted: {
              root.focus = false
              textAccepted(textInput.text)
          }
        }
    }
}
