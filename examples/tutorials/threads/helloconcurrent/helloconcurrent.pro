QT -= gui

CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += helloconcurrent.cpp 
   
# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/threads/helloconcurrent
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS helloconcurrent.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/threads/helloconcurrent
INSTALLS += target sources



