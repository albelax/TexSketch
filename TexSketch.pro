TARGET = TexSketch

QT += opengl\
      core \
      gui

CONFIG += console \
          c++11

CONFIG -= app_bundle

INCLUDEPATH += include \
               $$PWD/cl/include \
               ui \
               glm \
               shaders \
               images

HEADERS += include/MainWindow.h \
           include/GLWindow.h \
           include/Mesh.h \
           include/Shader.h \
           include/Camera.h \
           include/TrackballCamera.h \
           include/Image.h \
    include/PBRViewport.h

SOURCES += src/main.cpp \
           src/MainWindow.cpp \
           src/GLWindow.cpp \
           src/Mesh.cpp \
           src/Shader.cpp \
           src/Camera.cpp \
           src/TrackballCamera.cpp \
           src/Image.cpp \
    src/PBRViewport.cpp

FORMS += ui/MainWindow.ui

OTHER_FILES += shaders/* \
               images/* \
               cl/include/* \
               cl/src/*

UI_HEADERS_DIR = ui
OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui

linux:LIBS +=  -lGLEW

# CL
macx: LIBS += -framework OpenCL
linux: LIBS += -L/usr/lib64/nvidia -lOpenCL
INCLUDEPATH += /usr/include/CL cl/include/
