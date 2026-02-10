import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import System 1.0

ApplicationWindow {
    id: mainRoot
    visible: true
    width: 960
    height: 720
    title: qsTr("OCR Detect")

    property int contentTitleSize: 18
    property string fontFamily: "Calibri"
    property color transparentColor: "transparent"
    property string ocrResult: ""

    Connections{
        target: System
        function onImageChanged(){
            console.log("clip board image updated.")
        }
    }

    ColumnLayout{
        anchors.top: parent.top
        anchors.right: parent.right
        Button{
            id: catchClipboardBt
            text: "Import Image"
            Layout.preferredWidth: mainRoot.width *0.15
            Layout.preferredHeight: Layout.preferredWidth *2/3
            onClicked: {
                System.getImageFromClipboard()
            }
        }
        Button{
            id: runTesseractBt
            text: "Run OCR"
            Layout.preferredWidth: mainRoot.width *0.15
            Layout.preferredHeight: Layout.preferredWidth *2/3
            onClicked: {
                ocrResult = System.runTesseract()
            }
        }
        Button{
            id: convertExcel
            text: "Convert Excel"
            Layout.preferredWidth: mainRoot.width *0.15
            Layout.preferredHeight: Layout.preferredWidth *2/3
            onClicked: {
//                console.log("resultArea:")
//                console.log(resultArea.text)
                System.saveCsv(resultArea.text)
            }
        }
    }


    ColumnLayout{
        id: clipboardColumn
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 5
        spacing: 5
        Text {
            text: qsTr("Import Image:")
            font.family: fontFamily
            font.pointSize: contentTitleSize
        }
        Rectangle{
            id: clipboardRect
            Layout.preferredWidth: mainRoot.width *0.8
            Layout.preferredHeight: mainRoot.height *0.4
            color: transparentColor
            border.width: 1
            border.color: "red"
            Image {
                id: clipboardImage
                width: mainRoot.width *0.8
                height: mainRoot.height *0.8
                anchors.centerIn: parent
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: System.imageUrl
            }
        }
        Text {
            text: qsTr("Result:")
            font.family: fontFamily
            font.pointSize: contentTitleSize
        }
        Rectangle{
            id: resultRect
            Layout.preferredWidth: mainRoot.width *0.8
            Layout.preferredHeight: mainRoot.height *0.4
            color: transparentColor
            border.width: 1
            border.color: "red"
            Flickable {
                anchors.fill: parent
                contentWidth: resultArea.paintedWidth
                contentHeight: resultArea.paintedHeight
                clip: true
                TextArea{
                    id: resultArea
                    anchors.fill: parent
                    text: ocrResult
                    font.family: fontFamily
                    font.pointSize: contentTitleSize
    //                wrapMode: TextArea.WrapAnywhere
                }
                ScrollBar.vertical: ScrollBar {}
                ScrollBar.horizontal: ScrollBar {}
            }
        }
    }
}
