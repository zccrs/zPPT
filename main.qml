import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    id: root

    property int currentWebPage: 1
    property int easingType: Easing.InBack
    property variant bulletTextList
    property int bulletListCurrentIndex: 0

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
                        from: enterItem.rotation = currentWebPage == 1 ? 90 : 0
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
            stackView.push({item: Qt.resolvedUrl("qrc:///HomePage.qml"), replace: true})

            getBulletTextByFile("data/0.txt")
        }
    }

    Component.onCompleted: {
        bulletTextList = []
    }

    function getBulletTextByFile(fileName) {
        var contents = utility.readFile(utility.toAbsoluteFileUrl(fileName)).split("\n")

        for(var line in contents) {
            root.bulletTextList.push(contents[line])
        }
    }

    BulletScreen {
        id: bulletScreen

        anchors.fill: parent
    }

    Connections {
        target: speechRecognition

        onTextAppend: {
            bulletScreen.emit(text)
        }
    }

    Keys.onReturnPressed: {
        stackView.currentItem.play()
    }

    Keys.onEscapePressed: {
        speechRecognition.stop()

        Qt.quit()
    }

    Keys.onEnterPressed: {
        if(speechRecognition.isRecording())
            speechRecognition.stop()
        else
            speechRecognition.start()
    }

    Keys.onRightPressed: {
        var fileUrl = utility.toAbsoluteFileUrl("data/" + currentWebPage + ".html")

        getBulletTextByFile("data/" + currentWebPage + ".txt")

        ++currentWebPage

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

        if(currentWebPage > 1)
            --currentWebPage

        stackView.pop()
    }

    Keys.onDownPressed: {
        var tmp = bulletListCurrentIndex

        if(bulletListCurrentIndex >= bulletTextList.length)
            return

        bulletScreen.emit(root.bulletTextList[bulletListCurrentIndex++])
    }

    Keys.onUpPressed: {
        var tmp = bulletListCurrentIndex

        if(bulletListCurrentIndex <= 0)
            return

        bulletScreen.emit(root.bulletTextList[bulletListCurrentIndex - 1])

        --bulletListCurrentIndex
    }
}
