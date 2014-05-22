
#include "MoviesWorker.h"

#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLocale>

MoviesWorker::MoviesWorker (QObject * parent) : QObject (parent) {
    m_db  = QSqlDatabase::addDatabase (QStringLiteral ("QSQLITE"));
    m_nam = new QNetworkAccessManager (this);

    for (int countryEnum = 0; countryEnum < QLocale::LastCountry; countryEnum++) {
        QString countryName = QLocale::countryToString (QLocale::Country (countryEnum));
        QString countryCode = countryCodes [countryEnum];
        //qDebug () << "countryEnum=" << countryEnum
        //          << "countryName=" << countryName
        //          << "countryCode=" << countryCode;
        m_hashCountries.insert (countryCode, countryName);
    }
    //qDebug () << "m_hashCountries:" << m_hashCountries.count () << m_hashCountries;

    for (int langEnum = 0; langEnum < QLocale::LastLanguage; langEnum++) {
        QString langName = QLocale::languageToString (QLocale::Language (langEnum));
        QString langCode = languageCodes [langEnum];
        //qDebug () << "langEnum=" << langEnum
        //          << "langName=" << langName
        //          << "langCode=" << langCode;
        m_hashLanguages.insert (langCode, langName);
    }
    //qDebug () << "m_hashLanguages:" << m_hashLanguages.count () << m_hashLanguages;
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
                                   "    cover   TEXT             DEFAULT (''), "
                                   "    summary TEXT             DEFAULT (''), "
                                   "    vote    INTEGER NOT NULL DEFAULT (-1), "
                                   "    PRIMARY KEY (tmdbId) "
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
    connect (reply, &QNetworkReply::finished, this, &MoviesWorker::onSearchReply);
    connect (reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this,  SLOT(onReplyError(QNetworkReply::NetworkError)));
}

void MoviesWorker::removeMovieInfo (int tmdbId) {
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
    connect (reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this,  SLOT(onReplyError(QNetworkReply::NetworkError)));
    emit movieItemAdded (tmdbId);
}

void MoviesWorker::updateMovieVote (int tmdbId, int vote) {
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
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    if (reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        QJsonParseError error;
        qDebug () << "data=" << data;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isObject ()) {
            QVariantList list = json.object ().value ("results").toArray ().toVariantList ();
            foreach (QVariant value, list) {
                QVariantMap movie = value.toMap ();
                QString poster = movie.value (QStringLiteral ("poster_path")).toString ();
                QVariantMap item;
                item.insert (QStringLiteral ("tmdbId"), movie.value (QStringLiteral ("id")).toInt ());
                item.insert (QStringLiteral ("title"),  movie.value (QStringLiteral ("original_title")).toString ());
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
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    if (reply->error () == QNetworkReply::NoError) {
        QByteArray data = reply->readAll ();
        QJsonParseError error;
        qDebug () << "data=" << data;
        QJsonDocument json = QJsonDocument::fromJson (data, &error);
        if (!json.isNull () && json.isObject ()) {
            QVariantMap movie = json.object ().toVariantMap ();

            int     tmdbId = movie.value (QStringLiteral ("id")).toInt ();
            QString poster = movie.value (QStringLiteral ("poster_path")).toString ();

            QVariantMap movieItem;
            movieItem.insert (QStringLiteral ("tmdbId"),    tmdbId);
            movieItem.insert (QStringLiteral ("title"),     movie.value (QStringLiteral ("original_title")).toString ());
            movieItem.insert (QStringLiteral ("overview"),  movie.value (QStringLiteral ("overview")).toString ());
            movieItem.insert (QStringLiteral ("cover"),     (!poster.isEmpty () ? tmdbImgUrl + poster : imgNoPoster));
            emit movieItemUpdated (tmdbId, movieItem);

            QVariantList releaseList = movie.value ("releases").toMap ().value ("countries").toList ();
            emit releaseDatesUpdated (releaseList);

            QVariantList altTitlesList = movie.value ("alternative_titles").toMap ().value ("titles").toList ();
            emit altTitlesUpdated (altTitlesList);

            QVariantList crewTeamList = movie.value ("credits").toMap ().value ("crew").toList ();
            emit crewTeamUpdated (crewTeamList);

            QVariantList castRoleList = movie.value ("credits").toMap ().value ("cast").toList ();
            emit castRolesUpdated (castRoleList);
        }
        else {
            qWarning () << "Movie info : result is not an object !";
        }
    }
    else {
        qWarning () << "Network error on movie info request :" << reply->errorString ();
    }
}

void MoviesWorker::onReplyError (QNetworkReply::NetworkError code) {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    Q_ASSERT (reply);
    qWarning () << "reply error=" << code << reply->errorString ();
}
