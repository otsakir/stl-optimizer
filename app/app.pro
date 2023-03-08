HEADERS       = glwidget.h \
                app.h \
                loader.h \
                mesh.h \
                rendering.h \
                stl_reader.h \
                window.h \
                mainwindow.h
SOURCES       = glwidget.cpp \
                app.cpp \
                loader.cpp \
                main.cpp \
                mesh.cpp \
                rendering.cpp \
                window.cpp \
                mainwindow.cpp

QT           += widgets

# install
INSTALLS += target
