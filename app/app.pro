HEADERS       = glwidget.h \
                app.h \
                appwindow.h \
                dockwidget.h \
                loader.h \
                mesh.h \
                rendering.h \
                stl_reader.h
SOURCES       = glwidget.cpp \
                app.cpp \
                appwindow.cpp \
                dockwidget.cpp \
                loader.cpp \
                main.cpp \
                mesh.cpp \
                rendering.cpp

QT           += widgets

# install
INSTALLS += target

FORMS += \
    appwindow.ui \
    dockwidget.ui

RESOURCES += \
    icons.qrc
