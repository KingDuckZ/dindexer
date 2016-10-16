import QtQuick 2.1

Item {
	Component {
		id: listDelegate
		Rectangle {
			width: parent.width
			//height: imageSize.height

			Image {
				id: itemIcon
				width: 64
				height: parent.height
				anchors.left: parent.left
				source: model.decoration
			}

			Text {
				id: itemDesc
				anchors.left: itemIcon.right
				anchors.leftMargin: 20
				anchors.verticalCenter: parent.verticalCenter
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
		orientation: ListView.Horizontal
	}
}
