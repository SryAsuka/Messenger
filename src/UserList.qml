import QtQuick 2.15
import QtQuick.Controls 2.15



Item{
    width: parent.width
    height: parent.height
    id : userlist

//数据结构
    ListModel {
        id: userListModel
    }

    // 定义一个信号，当用户被点击时触发
    signal userClicked(var id,var ip)
    //连接到 C++ 信号
    Connections {
        target: mess // 这里的 mess 是你的 C++ 类实例
        //​onUserInfoChanged​ 是一个槽函数，当 ​mess​ 发出 ​userInfoChanged​ 信号时会被调用
        function onUserInfoChanged() {
            userListModel.clear(); // 清空现有数据
            //遍历 ​mess.userInfo​ 数组，将每个用户的信息添加到 ​userListModel​ 中。每个用户信息包含 ​id​ 和 ​ip​ 两个字段。
            for (var i = 0; i < mess.userInfo.length; ++i) {
                var user = mess.userInfo[i];
                userListModel.append({
                    id: user[0],
                    ip: user[1]
                });
            }
        }
    }


//列表视图
    ListView {
        width: userlist.width
        height: parent.height

        model: userListModel
        //delegate:是渲染
        delegate: Item {
            width: userlist.width
            height: 50
            Rectangle {
                width: parent.width
                height: 50
                color: index % 2 === 0 ? "grey" : "black"  //奇数项为黑色，偶数项为透明。

                //​​Item​ 是一个容器，内部有两个 ​Label​:id,ip
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

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            userClicked(model.id,model.ip)
                            var clickedIndex = index
                            console.log(clickedIndex)
                            mess.startChat(clickedIndex)
                        }
                    }
                }
            }

        }
    }


}
