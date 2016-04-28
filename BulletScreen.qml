import QtQuick 2.0
import QtQuick.Particles 2.0

Item {
    function emit(text) {
        var component = Qt.createComponent("BulletScreenText.qml")

        if (component.status === Component.Ready) {
            var bullet = component.createObject(this);

            bullet.text = text;
        }
    }
}
