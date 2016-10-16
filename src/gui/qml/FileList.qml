import QtQuick 2.1

Item {
	id: rootItem

	Component {
		id: listDelegate

		Rectangle {
			width: rootItem.width
			height: 64
			//color: "transparent"
			border.color: "red"
			border.width: 2
			radius: 5

			Image {
				id: itemIcon
				fillMode: Image.PreserveAspectFit
				source: model.decoration
			}

			Text {
				id: itemDesc
				anchors.leftMargin: 20
				anchors.left: itemIcon.right
				font.pixelSize: 40
				text: model.display
			}
		}
	}

	ListView {
		id: fileListView
		anchors.fill: parent

		model: resultListModel
		delegate: listDelegate
		spacing: 4
	}
}
