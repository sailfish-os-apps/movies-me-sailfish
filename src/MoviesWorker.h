#ifndef MOVIESWORKER_H
#define MOVIESWORKER_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#define tmdbImgUrl         QStringLiteral ("http://image.tmdb.org/t/p/w342")
#define tmdbApiUrl         QStringLiteral ("http://api.themoviedb.org/3")
#define tmdbApiKey         QStringLiteral ("6afcfd358f445edf5deed91b672792be")

#define imgNoPoster        QStringLiteral ("qrc:/qml/images/no-poster-w342.jpg")

#define movieInfoRequest   QStringLiteral ("%1/movie/%2?api_key=%3&append_to_response=releases,alternative_titles,credits")
#define movieSearchRequest QStringLiteral ("%1/search/movie?api_key=%2&query=%3")

class MoviesWorker : public QObject {
    Q_OBJECT

public:
    explicit MoviesWorker    (QObject * parent = NULL);

public slots:
    void initialize          ();
    void loadMoviesFromDb    ();
    void searchForMovie      (QString name);
    void removeMovieInfo     (int     tmdbId);
    void getFullMovieInfo    (int     tmdbId);
    void updateMovieVote     (int     tmdbId, int vote);

signals:
    void movieItemAdded      (int tmdbId);
    void movieItemRemoved    (int tmdbId);
    void movieItemUpdated    (int tmdbId, QVariantMap values);
    void movieSearchResults  (QVariantList list);
    void releaseDatesUpdated (QVariantList list);
    void castRolesUpdated    (QVariantList list);
    void crewTeamUpdated     (QVariantList list);
    void altTitlesUpdated    (QVariantList list);

private slots:
    void onSearchReply       ();
    void onMovieReply        ();
    void onReplyProgress     (qint64 currBytes, qint64 totalBytes);
    void onReplyError        (QNetworkReply::NetworkError code);

private:
    QSqlDatabase            m_db;
    QNetworkAccessManager * m_nam;
    QHash<QString, QString> m_hashCountries;
    QHash<QString, QString> m_hashLanguages;
};

#endif // MOVIESWORKER_H
