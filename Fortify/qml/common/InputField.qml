import QtQuick 2.12
import QtQuick.Controls 2.2
import Felgo 4.0

import "../"

/*!
    \qmltype InputField
    \inqmlmodule Qml-FortiyMainApplication
    \inherits FocusScope
    \brief This class provides the text input field to take inputs from user.

    The usage of this class is as shown below::
    \qml
        InputField {
            id: favouriteNameInputField
            width: parent.width
            height: visible ? (parent.height/6)/2 - dp(10) : 0
            anchors.top: top_bar.bottom
            visible: root.addingNewFavourite
            placeHolderText: qsTr("Favourite Title")
            onTextAccepted: (text) => {
                root.favouriteName = text
            }
        }
    \endqml
*/

FocusScope {
    id: root

    /*!
        \qmlsignal InputField::textAccepted(var text)
        \brief The textAccepted(\a text) signal is emitted when the Return or Enter key is pressed.

        Note that if there is a validator or inputMask set on the text input, the signal will only be emitted if the input is in an acceptable state.
    */
    signal textAccepted(var text)

    /*!
        \qmlproperty alias InputField::placeHolderText
        \brief This property is used to provides reference property for textInput component's text property.
    */
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
