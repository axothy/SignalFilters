QT += charts

SOURCES += \
    main.cpp \
    randomsignalgenerator.cpp \
    diagnostic_filter.cpp

RESOURCES += \
    signaldata.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/charts/datetimeaxis
INSTALLS += target

HEADERS += \
    randomsignalgenerator.h \
    diagnostic_filter.h
