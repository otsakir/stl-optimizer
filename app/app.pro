HEADERS       = glwidget.h \
                app.h \
                appwindow.h \
                loader.h \
                mesh.h \
                rendering.h \
                stl_reader.h \
                window.h
SOURCES       = glwidget.cpp \
                app.cpp \
                appwindow.cpp \
                loader.cpp \
                main.cpp \
                mesh.cpp \
                rendering.cpp \
                window.cpp

QT           += widgets

# install
INSTALLS += target

FORMS += \
    appwindow.ui
