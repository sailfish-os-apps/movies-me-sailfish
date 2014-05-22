#ifndef MOVIESIMAGECACHER_H
#define MOVIESIMAGECACHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQmlHelpers>

class MoviesCommon {
public:
    static   MoviesCommon * getInstance ();
    QNetworkAccessManager * getNAM      () const;

    QString localFileFromRemoteUrl      (QString remoteUrl);

private:
    QString                 m_path;
    QNetworkAccessManager * m_nam;

    static   MoviesCommon * s_instance;
    explicit MoviesCommon   ();
};

class MoviesImageCacher : public QObject {
    Q_OBJECT
    QML_WRITABLE_PROPERTY (QString, remoteSource)
    QML_READONLY_PROPERTY (QString, localSource)

public:
    explicit MoviesImageCacher (QObject * parent = NULL);

private slots:
    void onRequestFinished     ();
    void onRemoteSourceChanged ();
};

#endif // MOVIESIMAGECACHER_H
