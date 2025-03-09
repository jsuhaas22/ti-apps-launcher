import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 6.0

Item {
    id: circularGauge
    width: 200
    height: 200

    property real value: 50
    property real minimumValue: 0
    property real maximumValue: 100
    property color backgroundColor: "lightgray"
    property color foregroundColor: "blue"
    property color needleColor: "red"

    // Background Arc (Full Circle)
    Shape {
        anchors.fill: parent

        ShapePath {
            strokeWidth: 10
            strokeColor: backgroundColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: width / 2
                centerY: height / 2
                radiusX: width / 2 - 10
                radiusY: height / 2 - 10
                startAngle: 135
                sweepAngle: 270
            }
        }
    }

    // Foreground Arc (Gauge Progress)
    Shape {
        anchors.fill: parent

        ShapePath {
            strokeWidth: 10
            strokeColor: foregroundColor
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap

            PathAngleArc {
                centerX: width / 2
                centerY: height / 2
                radiusX: width / 2 - 10
                radiusY: height / 2 - 10
                startAngle: 135
                sweepAngle: 270 * ((value - minValue) / (maxValue - minValue))
            }
        }
    }

    // Needle (Pointer)
    Rectangle {
        width: 4
        height: parent.height / 2 - 20
        color: needleColor
        radius: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        transform: Rotation {
            origin.x: 2
            origin.y: parent.height / 2 - 20
            angle: 135 + 270 * ((value - minimumValue) / (maximumValue - minimumValue))
        }
    }

    // Value Display
    Text {
        text: Math.round(value)
        anchors.centerIn: parent
        font.pixelSize: 18
        font.bold: true
    }

    // Animation for Smooth Value Change
    Behavior on value {
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutCubic
        }
    }
}
