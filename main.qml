import QtQuick 2.15
import QtQuick.Controls 2.15

import NetScanner 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Network Scanner")


    ListView {
        width: 200
        height: parent.height
        clip: true
        model: _scaner.connectedModel
        delegate: ItemDelegate {
            width: ListView.view.width
            height: 50
            Label {
                x: 7; y: 7
                text: "ip:" + model.address
            }
            Label {
                x: 7; y: 25
                text: "ports: " + model.ports.join(",")
            }
        }
    }

    Button {
        x: 250; y: 50
        width: 120
        height: 40
        text: "Scan"
        onClicked: _scaner.start()
    }


    NetScanner {
        id: _scaner
        async: true
        ports: [80, 443]
        onStarted: console.log("started!")
        onFinished: console.log("finished!")
    }
}
