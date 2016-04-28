import QtQuick 2.0

Text {
    id: text

    y: utility.random(10, parent.height / 3.0)
    font.pointSize: utility.random(19, 25)

    PropertyAnimation {
        id: animation

        target: text
        duration: utility.random(2000, 5000)
        property: "x"
        to: -width
        easing.type: Easing.InOutSine

        onStopped: {
            text.destroy()
        }
    }

    Component.onCompleted: {
        switch(utility.random(0, 2)) {
        case 0:
            color = Qt.rgba(utility.random(0, 255) / 255.0, 0, 0, 1)
            break;
        case 1:
            color = Qt.rgba(0, utility.random(0, 255) / 255.0, 0, 1)
            break;
        case 2:
            color = Qt.rgba(0, 0, utility.random(0, 255) / 255.0, 1)
            break;
        }
    }

    onTextChanged: {
        animation.from = parent.width - width
        animation.start()
    }
}
