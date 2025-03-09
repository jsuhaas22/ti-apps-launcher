import QtQuick
import QtQuick.Controls

Item {
    width: 300
    height: 300

    property real minimumValue
    property real maximumValue
    property real value
    property real labelStepSize

    property int labelCount: ((maximumValue - minimumValue) / labelStepSize) + 1

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"  // Light grey background
        radius: width / 2
        border.color: "black"
        border.width: 3
    }

    // Speedometer Ticks & Labels
    Canvas {
        id: speedometerCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
/*
            var centerX = width / 2
            var centerY = height / 2
            var radius = width * 0.4*/

            // ctx.font = "14px Arial"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"

            for (var i = 0; i < labelCount; i++) {
                var rad_interval = (320 * Math.PI / 180) / labelCount
                // var angle = ((i / labelCount) * 275 - 227) * Math.PI / 180
                // var angle = ((i / labelCount) * rad_interval - (145 * Math.PI / 180))
                var angle = ((i % labelCount) * rad_interval + (125 * Math.PI / 180))
                var x1 = centerX + Math.cos(angle) * radius
                var y1 = centerY + Math.sin(angle) * radius
                var x2 = centerX + Math.cos(angle) * (radius - 15)
                var y2 = centerY + Math.sin(angle) * (radius - 15)
                var textX = centerX + Math.cos(angle) * (radius - 30)
                var textY = centerY + Math.sin(angle) * (radius - 30)

                // Draw tick marks
                ctx.beginPath()
                ctx.moveTo(x1, y1)
                ctx.lineTo(x2, y2)
                ctx.strokeStyle = "black"
                ctx.lineWidth = 2
                ctx.stroke()

                // Draw labels
                ctx.fillStyle = "red"
                ctx.fillText(i * 10, textX, textY)
            }
        }
    }

    // Speedometer Needle
    Rectangle {
        width: 4
        height: parent.width * 0.35
        radius: 2
        color: "red"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.verticalCenter // Ensures it extends only upwards
        transformOrigin: Item.Bottom
        rotation: ((value - minimumValue) / (maximumValue - minimumValue)) * 180 - 90 // Mapping to 180° arc
    }

    // Needle Center (for aesthetics)
    Rectangle {
        width: 12
        height: 12
        color: "black"
        radius: 6
        anchors.centerIn: parent
    }
}
