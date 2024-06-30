import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Messenger")


//头部工具栏，绑定刷新和推出
    header: ToolBar {
        RowLayout{
            ToolButton{ action: open }
            ToolButton{ action: quit }
        }
    }


//动作，刷新和退出
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
//水平布局，两个长方形，用于放置userlist和chatform
    Row {
        anchors.fill: parent

        Rectangle {
            width: parent.width / 4
            height: parent.height
            color: "transparent"

            UserList {
                id: userList
                anchors.fill: parent
            }
        }

        Rectangle {
            width: parent.width / 4 *3
            height: parent.height
            color: "transparent"

            ChatForm {
                id: chatForm
                anchors.fill: parent
                Component.onCompleted: {
                            // 建立`UserList`和`ChatForm`之间的连接
                            userList.userClicked.connect(chatForm.handleData);
                        }
            }
        }
    }

}
