TEMPLATE      = subdirs
SUBDIRS       = \
                network \
                statemachine \
                threads \
                xml

!contains(QT_CONFIG, no-gui) {
    SUBDIRS  += \
                animation \
                desktop \
                dialogs \
                draganddrop \
                effects \
                graphicsview \
                ipc \
                itemviews \
                layouts \
                linguist \
                mainwindows \
                painting \
                richtext \
                sql \
                tools \
                tutorials \
                widgets \
                uitools \
                touch \
                gestures
}

contains(DEFINES, QT_NO_GESTURES): SUBDIRS -= gestures

contains(QT_CONFIG, script): SUBDIRS += script

embedded:SUBDIRS += qws
{
    !contains(QT_EDITION, Console):!contains(QT_CONFIG, no-gui):contains(QT_BUILD_PARTS, tools):SUBDIRS += designer
    contains(QT_BUILD_PARTS, tools):!contains(QT_CONFIG, no-gui):SUBDIRS += qtestlib help
}
contains(QT_CONFIG, opengl): SUBDIRS += opengl
contains(QT_CONFIG, openvg): SUBDIRS += openvg
contains(QT_CONFIG, dbus): SUBDIRS += dbus
contains(QT_CONFIG, declarative): SUBDIRS += declarative helper
contains(QT_CONFIG, xmlpatterns):!contains(QT_CONFIG, no-gui): SUBDIRS += xmlpatterns
contains(DEFINES, QT_NO_CURSOR): SUBDIRS -= mainwindows
contains(QT_CONFIG, concurrent): SUBDIRS += qtconcurrent

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]
INSTALLS += sources

