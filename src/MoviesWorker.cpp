
#include "MoviesWorker.h"

#include "commondefs.h"

#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkConfiguration>
#include <QLocale>

#define GET_SENDER(TYPE,NAME) \
    TYPE * NAME = qobject_cast<TYPE *> (sender ()); \
    Q_ASSERT (NAME)




MoviesWorker::MoviesWorker (QObject * parent) : QObject (parent) {
    m_db  = QSqlDatabase::addDatabase (QStringLiteral ("QSQLITE"));
    m_nam = new QNetworkAccessManager (this);

    for (int countryEnum = 0; countryEnum < QLocale::LastCountry; countryEnum++) {
        m_hashCountries.insert (ISO::countryCodeFromISO3166 (QLocale::Country (countryEnum)),
                                ISO::countryNameFromISO3166 (QLocale::Country (countryEnum)));
    }
    qDebug () << "m_hashCountries:" << m_hashCountries.count () << m_hashCountries;

    for (int langEnum = 0; langEnum < QLocale::LastLanguage; langEnum++) {
        m_hashLanguages.insert (ISO::languageCodeFromISO639 (QLocale::Language (langEnum)),
                                ISO::languageNameFromISO639 (QLocale::Language (langEnum)));
    }
    qDebug () << "m_hashLanguages:" << m_hashLanguages.count () << m_hashLanguages;
}

void MoviesWorker::initialize () {
    QString path (QStandardPaths::writableLocation (QStandardPaths::DataLocation));
    QDir dir (QDir::homePath ());
    dir.mkpath (path);
    dir.mkpath (QString ("%1/images").arg (path));
    m_db.setHostName (QStringLiteral ("localhost"));
    m_db.setDatabaseName (QString ("%1/movies_storage.db").arg (path));
    if (m_db.open ()) {
        m_db.transaction ();
        m_db.exec (QStringLiteral ("CREATE TABLE IF NOT EXISTS movies ( "
                                   "    tmdbId  INTEGER NOT NULL, "
                                   "    title   TEXT    NOT NULL DEFAULT (''), "
                                   "    cover   TEXT    NOT NULL DEFAULT (''), "
                                   "    summary TEXT    NOT NULL DEFAULT (''), "
                                   "    vote    INTEGER NOT NULL DEFAULT (-1), "
                                   "    PRIMARY KEY (tmdbId) "
                                   ");"));
        m_db.exec (QStringLiteral ("CREATE TABLE IF NOT EXISTS sublists ( "
                                   "    tmdbId  INTEGER NOT NULL, "
                                   "    type    TEXT    NOT NULL DEFAULT (''), "
                                   "    json    TEXT    NOT NULL DEFAULT (''), "
                                   "    PRIMARY KEY (tmdbId, type) "
                                   ");"));
        m_db.commit ();
        loadMoviesFromDb ();
    }
    else {
        qWarning () << "Offline storage database couldn't be loaded nor created !"
                    << m_db.lastError ().text ();
    }
}

void MoviesWorker::loadMoviesFromDb () {
    QSqlQuery queryList (m_db);
    queryList.prepare (QStringLiteral ("SELECT tmdbId, title, cover, summary, vote FROM movies ORDER BY title"));
    if (queryList.exec ()) {
        QSqlRecord record      = queryList.record ();
        int fieldMovieTmdbId   = record.indexOf (QStringLiteral ("tmdbId"));
        int fieldMovieVote     = record.indexOf (QStringLiteral ("vote"));
        int fieldMovieTitle    = record.indexOf (QStringLiteral ("title"));
        int fieldMovieCover    = record.indexOf (QStringLiteral ("cover"));
        int fieldMovieSummary  = record.indexOf (QStringLiteral ("summary"));
        while (queryList.next ()) {
            int tmdbId = queryList.value (fieldMovieTmdbId).toInt ();
            QVariantMap values;
            values.insert (QStringLiteral ("tmdbId"),  tmdbId);
            values.insert (QStringLiteral ("vote"),    queryList.value (fieldMovieVote).toInt ());
            values.insert (QStringLiteral ("title"),   queryList.value (fieldMovieTitle).toString ());
            values.insert (QStringLiteral ("cover"),   queryList.value (fieldMovieCover).toString ());
            values.insert (QStringLiteral ("summary"), queryList.value (fieldMovieSummary).toString ());
            emit movieItemAdded   (tmdbId);
            emit movieItemUpdated (tmdbId, values);
        }
    }
}

void MoviesWorker::searchForMovie (QString name) {
    qDebug () << "MoviesWorker::searchForMovie" << name;
    QNetworkRequest request (movieSearchRequest.arg (tmdbApiUrl).arg (tmdbApiKey).arg (name.replace (" ", "+")));
    //request.setRawHeader ("Proxy-Connection", "keep-alive");
    //request.setRawHeader ("Accept-Encoding", "gzip,deflate,sdch");
    qDebug () << request.url ().toString ();
    QNetworkReply * reply = m_nam->get (request);
    connect (reply, &QNetworkReply::finished,         this, &MoviesWorker::onSearchReply);
    connect (reply, &QNetworkReply::uploadProgress,   this, &MoviesWorker::onReplyProgress);
    connect (reply, &QNetworkReply::downloadProgress, this, &MoviesWorker::onReplyProgress);
    connect (reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this,  SLOT(onReplyError(QNetworkReply::NetworkError)));
}

void MoviesWorker::removeMovieInfo (int tmdbId) {
    qDebug () << "MoviesWorker::removeMovieInfo" << tmdbId;
    m_db.transaction ();
    QSqlQuery queryRemove (m_db);
    queryRemove.prepare   (QStringLiteral ("DELETE FROM movies WHERE tmdbId=:tmdbId"));
    queryRemove.bindValue (QStringLiteral (":tmdbId"), tmdbId);
    queryRemove.exec      ();
    m_db.commit ();
    emit movieItemRemoved (tmdbId);
}

void MoviesWorker::getFullMovieInfo (int tmdbId) {
    qDebug () << "MoviesWorker::getFullMovieInfo" << tmdbId;
    QNetworkRequest request (movieInfoRequest.arg (tmdbApiUrl).arg (tmdbId).arg (tmdbApiKey));
    qDebug () << request.url ().toString ();
    QNetworkReply * reply = m_nam->get (request);
    connect (reply, &QNetworkReply::finished, this, &MoviesWorker::onMovieReply);
    connect (reply, &QNetworkReply::uploadProgress,   this, &MoviesWorker::onReplyProgress);
    connect (reply, &QNetworkReply::downloadProgress, this, &MoviesWorker::onReplyProgress);
    connect (reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this,  SLOT(onReplyError(QNetworkReply::NetworkError)));
    emit movieItemAdded (tmdbId);
}

void MoviesWorker::updateMovieVote (int tmdbId, int vote) {
    qDebug () << "MoviesWorker::updateMovieVote" << tmdbId << vote;
    m_db.transaction ();
    QSqlQuery queryVote (m_db);
    queryVote.prepare   (QStringLiteral ("UPDATE movies SET vote=:vote WHERE tmdbId=:tmdbId"));
    queryVote.bindValue (QStringLiteral (":vote"),   vote);
    queryVote.bindValue (QStringLiteral (":tmdbId"), tmdbId);
    queryVote.exec      ();
    m_db.commit ();
    QVariantMap movie;
    movie.insert (QStringLiteral ("vote"), vote);
    emit movieItemUpdated (tmdbId, movie);
}

void MoviesWorker::onSearchReply () {
    qDebug () << "MoviesWorker::onSearchReply";
    QVariantList ret;
    GET_SENDER (QNetworkReply, reply);
    if (reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        qDebug () << "data=" << data;
        QVariant object = JSON::parse (QString::fromUtf8 (data));
        if (object.isValid ()) {
            QVariantList list = getSubValue (object, QStringLiteral ("results")).value<QVariantList> ();
            foreach (QVariant value, list) {
                QString poster = getSubValue (value, QStringLiteral ("poster_path")).value<QString> ();
                QVariantMap item;
                item.insert (QStringLiteral ("tmdbId"), getSubValue (value, QStringLiteral ("id")).value<int> ());
                item.insert (QStringLiteral ("title"),  getSubValue (value, QStringLiteral ("original_title")).value<QString> ());
                item.insert (QStringLiteral ("cover"),  (!poster.isEmpty () ? tmdbImgUrl + poster : imgNoPoster));
                ret.append (item);
            }
        }
        else {
            qWarning () << "Search movie : result is not an array !";
        }
    }
    else {
        qWarning () << "Network error on search request :" << reply->errorString ();
    }
    emit movieSearchResults (ret);
}

void MoviesWorker::onMovieReply () {
    qDebug () << "MoviesWorker::onMovieReply";
    GET_SENDER (QNetworkReply, reply);
    if (reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        qDebug () << "data=" << data;
        QVariant object = JSON::parse (QString::fromUtf8 (data));
        if (object.isValid ()) {
            int     tmdbId = getSubValue (object, QStringLiteral ("id")).value<int> ();
            QString poster = getSubValue (object, QStringLiteral ("poster_path")).value<QString> ();

            QVariantMap movieItem;
            movieItem.insert (QStringLiteral ("tmdbId"),    tmdbId);
            movieItem.insert (QStringLiteral ("title"),     getSubValue (object, QStringLiteral ("original_title")).value<QString> ());
            movieItem.insert (QStringLiteral ("overview"),  getSubValue (object, QStringLiteral ("overview")).value<QString> ());
            movieItem.insert (QStringLiteral ("cover"),     (!poster.isEmpty () ? tmdbImgUrl + poster : imgNoPoster));
            emit movieItemUpdated (tmdbId, movieItem);

            QVariantList releaseDatesList = getSubValue (object, QStringLiteral ("releases/countries")).value<QVariantList> ();
            emit releaseDatesUpdated (releaseDatesList);

            QVariantList altTitlesList = getSubValue (object, QStringLiteral ("alternative_titles/titles")).value<QVariantList> ();
            emit altTitlesUpdated (altTitlesList);

            QVariantList crewTeamList = getSubValue (object, QStringLiteral ("credits/crew")).value<QVariantList> ();
            emit crewTeamUpdated (crewTeamList);

            QVariantList castRolesList = getSubValue (object, QStringLiteral ("credits/cast")).value<QVariantList> ();
            emit castRolesUpdated (castRolesList);
        }
        else {
            qWarning () << "Movie info : result is not an object !";
        }
    }
    else {
        qWarning () << "Network error on movie info request :" << reply->errorString ();
    }
}

void MoviesWorker::onReplyProgress (qint64 currBytes, qint64 totalBytes) {
    GET_SENDER (QNetworkReply, reply);
    qWarning () << "reply=" << reply << "progress=" << currBytes << "/" << totalBytes << "bytes";
}

void MoviesWorker::onReplyError (QNetworkReply::NetworkError code) {
    GET_SENDER (QNetworkReply, reply);
    qWarning () << "reply=" << reply << "error=" << code << reply->errorString ();
}
