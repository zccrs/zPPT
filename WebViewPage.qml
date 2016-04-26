import QtWebKit 3.0
import QtQuick 2.0

AbstractPage {
    property alias url: webview.url

    WebView {
        id: webview

        anchors.fill: parent
    }
}

