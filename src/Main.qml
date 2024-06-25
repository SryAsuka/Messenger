import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Messenger")

    header: ToolBar {
        RowLayout{
            ToolButton{ action: open }
            ToolButton{ action: quit }
        }
    }

    Action {
        id: open
        text: qsTr("刷新")
        icon.name: "document-open"
        shortcut: "StandardKey.Open"
        onTriggered: mess.fresh()
    }

    Action {
        id: quit
        text: qsTr("&Quit")
        icon.name: "application-exit"
        onTriggered: Qt.quit();
    }

    Row {
        anchors.fill: parent

        Rectangle {
            width: parent.width / 4
            height: parent.height
            color: "transparent"

            UserList {
                anchors.fill: parent
            }
        }

        Rectangle {
            width: parent.width / 4 *3
            height: parent.height
            color: "transparent"

            ChatForm {
                anchors.fill: parent
            }
        }
    }
}
