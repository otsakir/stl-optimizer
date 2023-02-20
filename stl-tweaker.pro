HEADERS       = glwidget.h \
                loader.h \
                mesh.h \
                stl_reader.h \
                window.h \
                mainwindow.h
SOURCES       = glwidget.cpp \
                loader.cpp \
                main.cpp \
                mesh.cpp \
                window.cpp \
                mainwindow.cpp

QT           += widgets

# install
INSTALLS += target
