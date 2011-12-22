unix:TARGET = anomade
win32|macx:TARGET = Anomade

TEMPLATE = app
QT      += webkit network
CONFIG   = qt resources

HEADERS  = MainWindow.hpp  \
           Highlighter.hpp 

SOURCES  = MainWindow.cpp  \ 
           Highlighter.cpp \
           main.cpp      

LIBS       += -L../libs -lmarkdown
MOC_DIR     = .obj
OBJECTS_DIR = .obj
RESOURCES   = AnomadeApp.qrc

# install
target.path = /usr/local/bin/
INSTALLS += target
