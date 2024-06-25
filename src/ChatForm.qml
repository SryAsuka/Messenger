import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: parent.width
    height: parent.height
    id :chatform

    Rectangle {
        width: parent.width
        height: parent.height
        color: "transparent"

        ListView {
            id: chatListView
            width: parent.width
            height: parent.height / 2
            model: ListModel {
                ListElement { text: "你好，这是一条聊天消息。" }
            }

            delegate: Item {
                width: parent.width
                height: 20
                Text {
                    text: model.text
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                    color: "white"
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                }
            }
        }

        Rectangle {
            id: inputField
            width: parent.width
            height: parent.height / 2
            color: "transparent"
            anchors.bottom: parent.bottom

            TextArea {
                id: messageInput
                width: parent.width - sendButton.width - 20
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 10
                placeholderText: "输入消息..."
                wrapMode: Text.WordWrap
                color: "white"
            }

            Button {
                id: sendButton
                text: "发送"
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom
                onClicked: {
                    if (messageInput.text.trim() !== "") {
                        chatListView.model.append({ "text": messageInput.text.trim() });
                        messageInput.text = "";
                    }
                }
            }
        }
    }
}
