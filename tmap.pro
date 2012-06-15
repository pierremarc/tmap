TEMPLATE = app
CONFIG += console
CONFIG -= qt
DEFINES = LINUX BOOST_SPIRIT_THREADSAFE MAPNIK_THREADSAFE DEBUG MAPNIK_DEBUG MAPNIK_LOG HAVE_CAIRO HAVE_LIBXML2
SOURCES += main.cpp \
    Options.cpp

LIBS += -L/usr/lib -lmapnik
LIBS += -lfreetype -lltdl -lpng -ltiff -lz -ljpeg -lproj -licuuc -lboost_filesystem -lboost_regex -lxml2 -lboost_thread -lboost_system -lcairomm-1.0 -lcairo -lsigc-2.0
INCLUDEPATH += /home/pierre/System/include /usr/include /usr/include/freetype2 /usr/include/libxml2 /usr/include/gdal /usr/include/postgresql /usr/include/cairo /usr/include/glib-2.0 /usr/lib/i386-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/libpng12 /usr/include/cairomm-1.0 /usr/lib/cairomm-1.0/include /usr/include/sigc++-2.0 /usr/lib/sigc++-2.0/include

LIBS += -lGeographic

HEADERS += \
    Options.h \
    Utils.h
