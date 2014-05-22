
#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include <QAbstractListModel>
#include <QNetworkProxy>
#include <QQuickView>
#include <QStringList>
#include <sailfishapp.h>
#include <QSettings>
#include <QDebug>
#include <qqml.h>

#include "MoviesEngine.h"
#include "MovieItem.h"
#include "MoviesImageCacher.h"

#define QML_MODULE "harbour.moviesme.myPrivateImports", 1, 0

int main (int argc, char * argv []) {
    QString env = QString::fromLocal8Bit (!qgetenv ("http_proxy").isEmpty ()
                                          ? qgetenv ("http_proxy")
                                          : (!qgetenv ("HTTP_PROXY").isEmpty ()
                                             ? qgetenv ("HTTP_PROXY")
                                             : QByteArrayLiteral ("")));
    if (!env.isEmpty ()) {
        QStringList tmp = env.toLower ().remove (QStringLiteral ("http://")).split (":");
        QString host = tmp.first ();
        quint16 port = tmp.last  ().toInt ();
        qDebug () << "HTTP proxy :" << "host=" << host << "port=" << port;
        QNetworkProxy::setApplicationProxy (QNetworkProxy (QNetworkProxy::HttpProxy, host, port));
    }
    QSettings::setDefaultFormat (QSettings::IniFormat);
    QGuiApplication::setApplicationName ("harbour-moviesme");
    qmlRegisterUncreatableType<QAbstractItemModel>    (QML_MODULE, "AbstractItemModel",  "!!!");
    qmlRegisterUncreatableType<QAbstractListModel>    (QML_MODULE, "AbstractListModel",  "!!!");
    qmlRegisterUncreatableType<QQmlObjectListModel>   (QML_MODULE, "ObjectListModel",    "!!!");
    qmlRegisterType<MoviesImageCacher>                (QML_MODULE, "MoviesImageCacher");
    qmlRegisterType<MovieItem>                        (QML_MODULE, "MovieItem");
    qmlRegisterType<MoviesEngine>                     (QML_MODULE, "MoviesEngine");
    QGuiApplication * app = SailfishApp::application (argc, argv);
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl (QStringLiteral ("qrc:/qml/harbour-moviesme.qml")));
    view->show ();
    return app->exec ();
}
