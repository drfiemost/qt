load(qttest_p4)
SOURCES  += ../tst_qudpsocket.cpp
QT = core network

MOC_DIR=tmp

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
  }
} else {
    DESTDIR = ../
}

TARGET = tst_qudpsocket

CONFIG+=insignificant_test # QTQAINFRA-428
