import Felgo
import QtQuick

/*/////////////////////////////////////
  NOTE:
  Additional integration steps are needed to use the plugin, for example to add and link required libraries for Android and iOS.
  Please follow the integration steps described in the plugin documentation: https://felgo.com/doc/plugin-admob/


  EXAMPLE USAGE:
  Add the following piece of code inside your main QML { } to display this example page.

  AdMobItem {

  }

/////////////////////////////////////*/

Rectangle {
    anchors.fill: parent

    // Plugin Item
    AdMobBanner {
        id: adMobBanner
        adUnitId: "ca-app-pub-7369013909167745/1490414716" // replace with your AdMob adUnitId
        banner: AdMobBanner.Smart

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }

    // Plugin Item
    AdMobInterstitial {
        id: interstitial
        adUnitId: "ca-app-pub-9155324456588158/9075427220" // replace with your AdMob adUnitId
    }

    // AppListView for Plugin Overview and Example Usage
    AppListView {
        id: listView
        anchors.fill: parent
        backgroundColor: parent.color

        // Plugin Overview
        header: Column {
            id: contentCol
            width: parent.width
            spacing: dp(16)

            // Description Text
            Rectangle {
                width: parent.width
                height: descriptionText.height + 2 * contentCol.spacing
                color: "lightgrey"
                AppText {
                    id: descriptionText
                    width: parent.width - 2 * contentCol.spacing
                    text: "Integrate with AdMob to monetize and promote your apps & games with ads on Android and iOS."
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                }
            }

            // Image of Code Sample
            Image {
                width: sourceSize.width * dp(1) * 0.75
                height: width / sourceSize.width * sourceSize.height
                source: Qt.resolvedUrl("code-admob.png")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Image of Plugin Logo
            Image {
                width: sourceSize.width * dp(1) * 0.5
                height: width / sourceSize.width * sourceSize.height
                source: Qt.resolvedUrl("logo-admob.png")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Example Headline
            Rectangle {
                width: parent.width
                height: exampleHeadline.height + 2 * contentCol.spacing
                color: "lightgrey"
                AppText {
                    id: exampleHeadline
                    width: parent.width - 2 * contentCol.spacing
                    text: "Example"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                }
            }
        }

        // Example Actions (Content of List)
        model: ListModel {
            id: listModel

            ListElement { section: "Visibility"; current: false; name: "Display banner" }
            ListElement { section: "Visibility"; current: false; name: "Hide banner" }

            ListElement { section: "Banner Type"; current: false; name: "Standard (320 x 50)" }
            ListElement { section: "Banner Type"; current: false; name: "Medium Rectangle (300x250)" }
            ListElement { section: "Banner Type"; current: false; name: "Large (320x100)" }
            ListElement { section: "Banner Type"; current: false; name: "Leaderboard (728x90) - Tablet Only" }
            ListElement { section: "Banner Type"; current: true; name: "Smart (auto-size)" }

            ListElement { section: "Interstitial"; active: false; name: "Load and show" }
        }

        // Styling of ListView Delegate (List Items)
        delegate: SimpleRow {
            id: row

            text: name
            property bool isSelected: current || (index === 0 && adMobBanner.visible || index === 1 && !adMobBanner.visible)

            AppIcon {
                anchors.right: parent.right
                anchors.rightMargin: dp(10)
                anchors.verticalCenter: parent.verticalCenter
                iconType:  IconType.check
                size: dp(14)
                color: row.style.textColor
                visible: isSelected
            }

            style.showDisclosure: false

            // Execute selected Action for plugin when list item is clicked
            onSelected: {
                // Set selection
                if (index >= 2 && index <= 6) {
                    for(var i = 0; i < listModel.count; i++) {
                        listModel.setProperty(i, "current", i === index ? true : false)
                    }
                }

                // Visibility
                if (index === 0) {
                    adMobBanner.visible = true
                }
                else if (index === 1) {
                    adMobBanner.visible = false
                }
                // Banner types
                else if (index === 2) {
                    adMobBanner.banner = AdMobBanner.Standard
                }
                else if (index === 3) {
                    adMobBanner.banner = AdMobBanner.Medium
                }
                else if (index === 4) {
                    adMobBanner.banner = AdMobBanner.Large
                }
                else if (index === 5) {
                    adMobBanner.banner = AdMobBanner.LeaderBoard
                }
                else if (index === 6) {
                    adMobBanner.banner = AdMobBanner.Smart
                }
                // Interstitial
                else if (index === 7) {
                    interstitial.showInterstitialIfLoaded()
                }
            }
        }

        // Styling of List Sections
        section.property: "section"
        section.delegate: SimpleSection { }
    }

    Component.onCompleted: {
        // Cache interstitial
        interstitial.loadInterstitial()
    }
}
