HEADERS   = block.h \
            renderthread.h \
            window.h
SOURCES   = main.cpp \
            block.cpp \
            renderthread.cpp \
            window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS mandelbrot.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
INSTALLS += target sources


