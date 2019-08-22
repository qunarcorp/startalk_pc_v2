import QtQuick 2.5
import QtQuick.Window 2.0

Window {
    id: wnd
    visible: true
    flags:Qt.FramelessWindowHint
    color:Qt.rgba(0,0,0,0)

    function showMinSize() {
        flags = Qt.Window | Qt.WindowFullscreenButtonHint | Qt.CustomizeWindowHint | Qt.WindowMinimizeButtonHint
        wnd.visibility = Window.Minimized
    }

    function resizeContent(x, y, w, h) {
        wnd.x = x
        wnd.y = y
        wnd.width = w
        wnd.height = h
    }
}
