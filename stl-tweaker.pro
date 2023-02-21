HEADERS       = glwidget.h \
                loader.h \
                mesh.h \
                renderstate.h \
                stl_reader.h \
                window.h \
                mainwindow.h
SOURCES       = glwidget.cpp \
                loader.cpp \
                main.cpp \
                mesh.cpp \
                renderstate.cpp \
                window.cpp \
                mainwindow.cpp

QT           += widgets

# install
INSTALLS += target
