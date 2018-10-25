TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= c++11


gcc:QMAKE_CXXFLAGS += -std=c++1z

SOURCES += \
    main.cc


gcc:LIBS += -lboost_system
gcc:LIBS += -lboost_filesystem
gcc:LIBS += -lboost_thread

HEADERS += \
    cc/Slice.h \
    cc/range/collect.hpp \
    cc/thread/Mutex.hpp \
    cc/thread/Thread.hpp \
    cc/scope.hpp \
    cc/File.hpp \
    cc/NotNull.hpp \
    cc/range/fold.hpp \
    cc/move.hpp
