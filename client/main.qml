//import QtQuick 2.15
//import QtQuick.Controls 2.15
//import QtQuick.Controls.Material 2.15
//import QtQuick.Layouts 1.15

//ApplicationWindow {
//    visible: true
//    width: 760
//    height: 620
//    minimumWidth: 760
//    maximumWidth: 760
//    minimumHeight: 620
//    maximumHeight: 620
//    title: qsTr("登录")
    
//    Material.theme: Material.Dark
//    Material.primary: Material.Blue
//    Material.accent: Material.Pink

//    ColumnLayout {
//        anchors.centerIn: parent
//        Text {
//            id: loginStatusText
//            text: ""
//            Layout.fillWidth: true
//            Connections {
//                target: loginManager
//                onRegisterCallback: {
//                    if (success) {
//                        loginStatusText.text = "注册成功";
//                        success=false;
//                    } else {
//                        loginStatusText.text = "注册失败";
//                    }
//                }
//                onLoginCallback: {
//                    if (success) {
//                        loginStatusText.text = "登录成功";
//                    } else {
//                        loginStatusText.text = "登录失败";
//                    }
//                }
//                onQuitCallback: {
//                    if (success) {
//                        loginStatusText.text = "退出成功";
//                        success=false;
//                    } else {
//                        loginStatusText.text = "退出失败";
//                    }
//                }

//            }
//        }
//        TextField {
//            id: usernameField
//            placeholderText: qsTr("用户名")
//            Material.accent: Material.Blue
//            Layout.fillWidth: true
//        }

//        TextField {
//            id: passwordField
//            placeholderText: qsTr("密码")
//            echoMode: TextInput.Password
//            Material.accent: Material.Blue
//            Layout.fillWidth: true
//        }

//        RowLayout{
//            Button {
//                objectName: "myBtn"
//                text: qsTr("登录")
//                Material.accent: Material.Pink
//                Layout.fillWidth: true
//                onClicked: {
//                    loginManager.handleLogin(usernameField.text, passwordField.text);
//                }
    
//                function recvMsg(msg) {
//                    console.log("xxx msg:" + msg);
//                }
//            }
//            Button {
//                text: qsTr("注册")
//                Material.accent: Material.Pink
//                Layout.fillWidth: true
//                onClicked: {
//                    loginManager.handleRegister(usernameField.text, passwordField.text);
//                }
//            }
//        }
//        Button {
//            text: qsTr("退出")
//            Material.accent: Material.Pink
//            Layout.fillWidth: true
//            onClicked: {
//                loginManager.handleQuit(usernameField.text, passwordField.text);
//            }
//        }
    
//    }
//}


import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 760
    height: 620
    minimumWidth: 760
    maximumWidth: 760
    minimumHeight: 620
    maximumHeight: 620
    title: qsTr("登录")

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Pink

    StackView {
        id: stackView
        anchors.fill: parent

        Text {
            id: loginStatusText
            text: ""
            Layout.fillWidth: true
        }

//        Connections {
//            target: loginManager
            
//            onQuitCallback: {
//                if (success) {
//                    loginStatusText.text = "退出成功";
//                } else {
//                    loginStatusText.text = "退出失败";
//                }
//            }
//        }

        // 登录页面
        Component {
            id: loginPage
            Rectangle {
                color: "black"
                ColumnLayout {
                    anchors.centerIn: parent
                    Connections {
                        target: loginManager
                        
                        onLoginCallback: {
                            if (success) {
                                loginStatusText.text = "登录成功";
                                stackView.push(lobbyPage);
                            } else {
                                loginStatusText.text = "登录失败";
                            }
                        }
                    }
                    
                    Text {
                        color: "white"
                        text:"登录页面"
                        font.pixelSize: 30
                        Layout.alignment: Qt.AlignCenter
                    }
                    Text {
                        color: "white"
                        id: loginStatusText
                        text: "登录状态：None"
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: usernameField
                        placeholderText: qsTr("用户名")
                        Material.accent: Material.Blue
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: passwordField
                        placeholderText: qsTr("密码")
                        echoMode: TextInput.Password
                        Material.accent: Material.Blue
                        Layout.fillWidth: true
                    }
                    RowLayout {
                        Button {
                            text: qsTr("登录")
                            Material.accent: Material.Pink
                            Layout.fillWidth: true
                            onClicked: {
                                loginManager.handleLogin(usernameField.text, passwordField.text);
                            }
                        }
                        Button {
                            text: qsTr("注册")
                            Material.accent: Material.Pink
                            Layout.fillWidth: true
                            onClicked: {
                                // 切换到注册页面
                                stackView.push(registerPage);
                            }
                        }
                    }
                }
            }
        }

        // 注册页面
        Component {
            id: registerPage
            Rectangle {
                color: "black"
                ColumnLayout {
                    anchors.centerIn: parent
                    Connections {
                        target: loginManager
                        
                        onRegisterCallback: {
                            if (success) {
                                registerStatusText.text = "注册成功：注册成功";
                            } else {
                                registerStatusText.text = "注册成功：注册失败";
                            }
                        }
                    }
                    Text {
                        color: "white"
                        text:"注册页面"
                        font.pixelSize: 30
                        Layout.alignment: Qt.AlignCenter
                    }
                    Text {
                        color: "white"
                        id: registerStatusText
                        text: "注册状态：None"
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: usernameField
                        placeholderText: qsTr("用户名")
                        Material.accent: Material.Blue
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: passwordField
                        placeholderText: qsTr("密码")
                        echoMode: TextInput.Password
                        Material.accent: Material.Blue
                        Layout.fillWidth: true
                    }
        
                    RowLayout {
                        spacing: 10 // 设置按钮之间的间距
        
                        Button {
                            text: qsTr("返回登录页面")
                            Material.accent: Material.Pink
                            Layout.fillWidth: true
                            onClicked: {
                                // 切换回登录页面
                                stackView.pop();
                            }
                        }
                        Button {
                            text: qsTr("注册")
                            Material.accent: Material.Pink
                            Layout.fillWidth: true
                            onClicked: {
                                loginManager.handleRegister(usernameField.text, passwordField.text);
                            }
                        }
                    }
                }
            }
        }

        //大厅界面
        Component {
            id: lobbyPage
            Rectangle {
                color: "black"
                ColumnLayout {
                    anchors.centerIn: parent
                    Connections {
                        target: loginManager
                        
                        onQuitCallback: {
                            if (success) {
                                loginStatusText.text = "退出成功: 退出成功";
                            } else {
                                loginStatusText.text = "退出失败: 退出成功";
                            }
                        }
                    }
                    Text {
                        color: "white"
                        text:"大厅界面"
                        font.pixelSize: 30
                        Layout.alignment: Qt.AlignCenter
                    }
                    Text {
                        color: "white"
                        id: registerStatusText
                        text: "退出状态：None"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: qsTr("退出")
                        Material.accent: Material.Pink
                        Layout.fillWidth: true
                        onClicked: {
                            // 切换回登录页面
                            stackView.pop();
                        }
                    }
                }
            }
        }
        initialItem: loginPage // 初始页面为登录页面
    }
}
