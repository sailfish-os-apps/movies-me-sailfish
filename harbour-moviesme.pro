# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-moviesme

QT += core gui network sql qml quick

MOC_DIR     = _moc
RCC_DIR     = _rcc
OBJECTS_DIR = _obj

CONFIG += sailfishapp

INCLUDEPATH += src/libQtQmlTricks

SOURCES += src/harbour-moviesme.cpp \
    src/MovieItem.cpp \
    src/MoviesEngine.cpp \
    src/MoviesWorker.cpp \
    src/MoviesImageCacher.cpp \
    src/libQtQmlTricks/qqmlhelpers.cpp \
    src/libQtQmlTricks/qqmlobjectlistmodel.cpp \
    src/libQtQmlTricks/qqmlvariantlistmodel.cpp

HEADERS += \
    src/MovieItem.h \
    src/MoviesEngine.h \
    src/MoviesWorker.h \
    src/MoviesImageCacher.h \
    src/libQtQmlTricks/qqmlhelpers.h \
    src/libQtQmlTricks/qqmlmodels.h \
    src/libQtQmlTricks/qqmlobjectlistmodel.h \
    src/libQtQmlTricks/qqmlobjectlistmodel_p.h \
    src/libQtQmlTricks/qqmlvariantlistmodel.h \
    src/libQtQmlTricks/qqmlvariantlistmodel_p.h \
    src/commondefs.h

RESOURCES += \
    data.qrc

OTHER_FILES += \
    rpm/harbour-moviesme.yaml \
    harbour-moviesme.desktop \
    harbour-moviesme.png \
    translations/harbour-moviesme.ts \
    qml/harbour-moviesme.qml \
    qml/cover/CoverPage.qml \
    qml/pages/MoviesListPage.qml \
    qml/pages/MovieDetailsPage.qml \
    qml/pages/MoviesSearchAndAddPage.qml \
    qml/components/CachedImage.qml

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n
#TRANSLATIONS +=



