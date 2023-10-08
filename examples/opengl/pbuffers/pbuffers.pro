HEADERS += glwidget.h \
    cube.h
SOURCES += glwidget.cpp \
    main.cpp \
    cube.cpp
RESOURCES += pbuffers.qrc
QT += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/pbuffers
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    pbuffers.pro \
    *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/pbuffers
INSTALLS += target \
    sources

simulator: warning(This example might not fully work on Simulator platform)
