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

    ColumnLayout {
        anchors.centerIn: parent

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

        RowLayout{
            Button {
                objectName: "myBtn"
                text: qsTr("登录")
                Material.accent: Material.Pink
                Layout.fillWidth: true
                onClicked: {
                    loginManager.handleLogin(usernameField.text, passwordField.text);
                }
    
                function recvMsg(msg) {
                    console.log("xxx msg:" + msg);
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
        Button {
            text: qsTr("退出")
            Material.accent: Material.Pink
            Layout.fillWidth: true
            onClicked: {
                loginManager.handleQuit(usernameField.text, passwordField.text);
            }
        }
    }
}

