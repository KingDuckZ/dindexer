import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0

ApplicationWindow {
	id: mainWin
	visible: true
	title: "dindexer gui"
	width: 640
	height: 480

	menuBar: MenuBar {
		id: menu
		Menu {
			id: menuFile
			title: qsTr("File")

			//MenuSeparator {
			//}

			MenuItem {
				text: qsTr("Exit")
				shortcut: "Ctrl+q"
				onTriggered: Qt.quit();
			}
		}
	}

	Rectangle {
		id: rectagleMain
		TextField {
			id: searchBox
			maximumLength: 256
			placeholderText: qsTr("Locate glob");
			text: "*.ogg"
			onEditingFinished: if (text.length >= 1) { locate(text); }
		}
	}

	statusBar: StatusBar {
		RowLayout {
			Label {
				id: statusLabel
				text: qsTr("dindexer idle")
			}
		}
	}

    signal locate(string glob)
}
