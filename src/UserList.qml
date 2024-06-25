import QtQuick 2.15
import QtQuick.Controls 2.15

Item{
    width: parent.width
    height: parent.height
    id : userlist

    ListModel {
        id: userListModel
    }

    Connections {
        target: mess // 这里的 mess 是你的 C++ 类实例
        function onUserInfoChanged() {
            userListModel.clear(); // 清空现有数据
            for (var i = 0; i < mess.userInfo.length; ++i) {
                var user = mess.userInfo[i];
                userListModel.append({
                    id: user[0],
                    ip: user[1]
                });
            }
        }
    }

    ListView {
        width: userlist.width
        height: parent.height

        model: userListModel

        delegate: Item {
            width: userlist.width
            height: 50
            Rectangle {
                width: parent.width
                height: 50
                color: index % 2 === 0 ? "black" : "transparent"

                Item {
                    anchors.fill: parent

                    Label {
                        id :users
                        text: model.id
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        // color: undefined // 使用默认字体颜色
                    }

                    Label {
                        id :ips
                        anchors.top: users.bottom
                        anchors.topMargin: 3
                        text: model.ip
                        // color: undefined // 使用默认字体颜色
                    }
                }
            }

        }
    }
}
