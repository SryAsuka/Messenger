import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.0


Item {
    width: parent.width
    height: parent.height
    id :chatform
    property string user_ip;
    property string user_id;

    function handleData(id,ip) {
        user_id=id
        user_ip=ip

      }

    function showWarningDialog() {
        messageDialog.open()
    }



    Rectangle {
        width: parent.width
        height: parent.height

        Rectangle {
                id: titleBar
                width: parent.width
                height: 30
                color: "lightgray"
                Text {
                    text: "开始"+user_id+"聊天"
                    anchors.centerIn: parent
                    font.pixelSize: 18
                }
            }


        ListView {
            id: chatListView
            width: parent.width
            // height: parent.height / 2
            height:parent.height - titleBar.height - inputField.height
            anchors.top: titleBar.bottom
            //listview数据模型
            model: ListModel {
                ListElement { text: "你好，这是一条聊天消息。" }

            }
            //每个项的外观
            delegate: Item {
                // width: parent.width
                height: 20
                Text {
                    text: model.text
                    id: textEditHistory
                    wrapMode: Text.WordWrap
                    font.pixelSize: 16
                    // color: "white"
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                }

            }


        }

        //输入框
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
                // color: "white"
                // 在TextArea中显示文件名
                function displaySelectedFile(fileNames) {
                    var message = "";
                    for (var i = 0; i < fileNames.length; i++) {
                        message +=fileNames[i].fileInfo+ "\n";
                    }
                    messageInput.text += message;
                }
            }
            //发送按钮
            Button {
                id: sendButton
                text: "发送"
                anchors.right: parent.right
                anchors.rightMargin: 150
                anchors.bottom: parent.bottom

                // 定义按钮点击时的行为，这里是在文本不为空时将文本添加到ListView的模型中，并清空文本输入区域。
                onClicked: {
                    if (messageInput.text.trim() !== "") {
                        // chatListView.model.append({ "text": messageInput.text.trim() });
                        if (user_ip === undefined || user_ip === "") { // 检查user_id是否为空
                            console.log("请先选择用户")
                              showWarningDialog()
                              return; //
                          }
                        var des=user_ip;

                        formChat.chatSend( messageInput.text.trim()+" ",des)

                        messageInput.text = "";
                    }

                }

            }


            Button {
                id: chooseFileButton
                text: "选择文件"
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom

                onClicked: {
                    fileDialog.open()
                }

            }





        }

    }


    FileDialog {
        id: fileDialog
        title: "选择要发送的文件"
        onAccepted: {

            messageInput.displaySelectedFile(fileDialog.files)
           // formChat.addShareFile(fileDialog.files);
           // mess.addFiles(fileDialog.files)
            formChat.addShareFile(fileDialog.files,user_ip)


        }
        onRejected: {
                   console.log("Canceled")
               }
    }
    MessageDialog {
        id: messageDialog
        title: "警告"
        text: "请先选择用户！"

    }



    Connections {
        target: mess
        function onChatmessChanged() {
            if (typeof mess.chatmess !== "undefined" && mess.chatmess !== "") {
                chatListView.model.append({ "text": mess.chatmess });
            } else {
                console.warn("chatmess is undefined or empty");
            }
        }
    }

    Connections {
         target: formChat // 假设formChat是从C++传递到QML的FormChat实例
         function onChatHistoryUpdated(data) {
             // 将接收到的数据添加到ListView的模型中
             chatListView.model.clear()
             chatListView.model.append({"text": data});
             // console.log(data)
             // 确保ListView滚动到最新消息
             // chatListView.positionViewAtEnd()
             messageInput.text = "";
         }

    }
}
