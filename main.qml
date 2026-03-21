import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
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

    FileDialog {
        id: saveDialog
        title: "Save CSV File"
        nameFilters: ["CSV files (*.csv)", "All files (*)"]
        selectExisting: false
        onAccepted: {
            System.saveCsv(resultArea.text, fileUrl)
        }
    }

    Connections{
        target: System
        function onImageChanged(){
            console.log("clip board image updated.")
        }
    }

    // Toolbar with buttons at the top
    RowLayout {
        id: toolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 15
        
        Button {
            id: catchClipboardBt
            text: "Import Image"
            font.family: fontFamily
            font.pointSize: 12
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            onClicked: {
                System.getImageFromClipboard()
            }
        }
        Button {
            id: runTesseractBt
            text: "Run OCR"
            font.family: fontFamily
            font.pointSize: 12
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            onClicked: {
                var str = System.runTesseract()
                ocrResult += str
            }
        }
        Button {
            id: convertExcel
            text: "Convert Excel"
            font.family: fontFamily
            font.pointSize: 12
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            onClicked: {
                saveDialog.open()
            }
        }
    }

    // Main content area in a top-bottom structure
    ColumnLayout {
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        anchors.topMargin: 15
        spacing: 15

        // --- Top Half: Image ---
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredHeight: mainRoot.height * 0.5
            spacing: 5
            
            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: qsTr("Import Image:")
                    font.family: fontFamily
                    font.pointSize: contentTitleSize
                    font.bold: true
                }
                Item { Layout.fillWidth: true } // spacer
                Text {
                    text: qsTr("Zoom:")
                    font.family: fontFamily
                    font.pointSize: 12
                }
                Slider {
                    id: zoomSlider
                    from: 0.1
                    to: 2.0
                    value: 0.5
                    stepSize: 0.1
                    Layout.preferredWidth: 150
                }
                Text {
                    text: Math.round(zoomSlider.value * 100) + "%"
                    font.family: fontFamily
                    font.pointSize: 12
                    Layout.preferredWidth: 40
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#f9f9f9"
                border.width: 1
                border.color: "#cccccc"
                radius: 4
                
                ScrollView {
                    id: imageScrollView
                    anchors.fill: parent
                    anchors.margins: 4
                    clip: true
                    
                    // The ScrollView needs to know the scaled size of its content to show scrollbars
                    contentWidth: clipboardImage.width
                    contentHeight: clipboardImage.height

                    Image {
                        id: clipboardImage
                        // Compute dynamic size based on the actual image dimension * slider value
                        width: sourceSize.width * zoomSlider.value
                        height: sourceSize.height * zoomSlider.value
                        
                        // We use center inside the content bounds when small, or top-left when large enough to scroll
                        // But an easier way is to just let it start at 0,0 and expand.
                        
                        smooth: true
                        source: System.imageUrl
                        
                        // When filling dynamically, we turn off PreserveAspectFit and strictly use our calculated width/height 
                        // so it actually physically stretches the dimensions for the scrollview.
                        fillMode: Image.Stretch
//                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
        }

        // --- Bottom Half: Result ---
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredHeight: mainRoot.height * 0.35
            spacing: 5
            
            Text {
                text: qsTr("OCR Result:")
                font.family: fontFamily
                font.pointSize: contentTitleSize
                font.bold: true
                Layout.alignment: Qt.AlignLeft
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "white"
                border.width: 1
                border.color: "#cccccc"
                radius: 4
                
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 8
                    clip: true

                    TextArea {
                        id: resultArea
                        text: ocrResult
                        font.family: fontFamily
                        font.pointSize: contentTitleSize
                        selectByMouse: true
                        background: Item {} // Remove default background
                        // wrapMode: TextArea.Wrap // Enable if you want strict wrapping
                    }
                }
            }
        }
    }
}
