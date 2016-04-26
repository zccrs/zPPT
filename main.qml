import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: root

    property int currentWebPage: 0
    property int easingType: Easing.InBack

    anchors.fill: parent

    StackView {
        id: stackView

        anchors.fill: parent
        focus: true

        initialItem: Image {
            source: utility.screenShots()
        }

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.x = 0
                properties.exitItem.rotation = 0
            }

            pushTransition: StackViewTransition {
                SequentialAnimation {
                    ScriptAction {
                        script: enterItem.rotation = stackView.depth == 2 ? 270 : 0
                    }
                    PropertyAnimation {
                        target: enterItem
                        property: "x"
                        from: enterItem.width
                        to: 0
                        duration: 1000
                        easing.type: easingType
                    }
                    PropertyAnimation {
                        target: enterItem
                        property: "rotation"
                        from: enterItem.rotation = stackView.depth == 2 ? 90 : 0
                        to: 0
                    }
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: -exitItem.width
                    duration: 1000
                    easing.type: easingType
                }
            }
        }
    }

    Timer {
        id: initTimer

        running: true
        interval: 100

        onTriggered: {
            stackView.push({"item": Qt.resolvedUrl("qrc:///HomePage.qml")})
        }
    }

    Keys.onReturnPressed: {
        stackView.currentItem.play()
    }

    Keys.onEscapePressed: Qt.quit()

    Keys.onRightPressed: {
        var fileUrl = utility.toAbsoluteFileUrl(currentWebPage + ".html")

        if(utility.fileExists(fileUrl)) {
            easingType = Easing.InBack

            stackView.push({"item": Qt.resolvedUrl("qrc:///WebViewPage.qml"),
                            "properties": {
                               "url": fileUrl
                               }})
        }
    }

    Keys.onLeftPressed: {
        easingType = Easing.OutBack

        stackView.pop()
    }
}
