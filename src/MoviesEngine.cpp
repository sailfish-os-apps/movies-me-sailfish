
#include "MoviesEngine.h"
#include "MoviesWorker.h"
#include "MovieItem.h"

#include <QDebug>

MoviesEngine::MoviesEngine (QObject * parent) : QObject (parent) {
    m_searchModel = QQmlObjectListModel::create<MovieItem> (this);
    m_moviesModel = QQmlObjectListModel::create<MovieItem> (this);

    m_moviesModel->setRoleNameForUid (QByteArrayLiteral ("tmdbId"));

    m_releaseDatesList = new QQmlVariantListModel (this); // { "certification" : "R", "iso_3166_1" : "US", "release_date" : "2002-01-10" }
    m_castRolesList    = new QQmlVariantListModel (this); // { "character" : "Mr Bean", "name" : "The Actor", "profile_path" : "img/url" }
    m_crewTeamList     = new QQmlVariantListModel (this); // { "job" : "Job name", "name" : "The Person", "profile_path" : "img/url" }
    m_altTitlesList    = new QQmlVariantListModel (this); // { "iso_3166_1" : "US", "title" : "The US title" }

    // separated thread
    m_thread = new QThread (this);

    // worker object
    m_worker = new MoviesWorker;
    m_worker->moveToThread (m_thread);

    connect (m_thread,  &QThread::started,                          m_worker, &MoviesWorker::initialize);

    // requests
    connect (this,      &MoviesEngine::searchRequested,             m_worker, &MoviesWorker::searchForMovie);
    connect (this,      &MoviesEngine::fullMovieInfoRequested,      m_worker, &MoviesWorker::getFullMovieInfo);
    connect (this,      &MoviesEngine::removeMovieRequested,        m_worker, &MoviesWorker::removeMovieInfo);
    connect (this,      &MoviesEngine::updateMovieVoteRequested,    m_worker, &MoviesWorker::updateMovieVote);

    // replies
    connect (m_worker,  &MoviesWorker::movieSearchResults,          this,     &MoviesEngine::onMovieSearchResults);
    connect (m_worker,  &MoviesWorker::movieItemAdded,              this,     &MoviesEngine::onMovieItemAdded);
    connect (m_worker,  &MoviesWorker::movieItemRemoved,            this,     &MoviesEngine::onMovieItemRemoved);
    connect (m_worker,  &MoviesWorker::movieItemUpdated,            this,     &MoviesEngine::onMovieItemUpdated);
    connect (m_worker,  &MoviesWorker::altTitlesUpdated,            this,     &MoviesEngine::onAltTitlesUpdated);
    connect (m_worker,  &MoviesWorker::crewTeamUpdated,             this,     &MoviesEngine::onCrewTeamUpdated);
    connect (m_worker,  &MoviesWorker::castRolesUpdated,            this,     &MoviesEngine::onCastRolesUpdated);
    connect (m_worker,  &MoviesWorker::releaseDatesUpdated,         this,     &MoviesEngine::onReleaseDatesUpdated);

    // handlers
    connect (this,      &MoviesEngine::currentMovieIdChanged,       this,     &MoviesEngine::onCurrentMovieIdChanged);

    m_thread->start (QThread::HighestPriority);
}

void MoviesEngine::requestSearch (QString name) {
    qDebug () << "MoviesEngine::requestSearch" << name;
    emit searchRequested (name);
}

void MoviesEngine::requestFullMovieInfo (int tmdbId) {
    qDebug () << "MoviesEngine::requestFullMovieInfo" << tmdbId;
    emit fullMovieInfoRequested (tmdbId);
}

void MoviesEngine::requestUpdateVote (int tmdbId, int vote) {
    qDebug () << "MoviesEngine::requestUpdateVote" << tmdbId << vote;
    emit updateMovieVoteRequested (tmdbId, vote);
}

void MoviesEngine::onMovieItemAdded (int tmdbId) {
    qDebug () << "MoviesEngine::onMovieItemAdded" << tmdbId;
    if (!m_moviesModel->getByUid (QString::number (tmdbId))) {
        MovieItem * item = new MovieItem;
        item->update_tmdbId (tmdbId);
        m_moviesModel->append (item);
    }
}

void MoviesEngine::onMovieItemRemoved (int tmdbId) {
    qDebug () << "MoviesEngine::onMovieItemRemoved" << tmdbId;
    QObject * item = m_moviesModel->getByUid (QString::number (tmdbId));
    if (item) {
        m_moviesModel->remove (item);
    }
}

void MoviesEngine::onCurrentMovieIdChanged (int tmdbId) {
    qDebug () << "MoviesEngine::onCurrentMovieIdChanged" << tmdbId;

}

void MoviesEngine::onMovieItemUpdated (int tmdbId, QVariantMap values) {
    qDebug () << "MoviesEngine::onMovieItemUpdated" << tmdbId << values;
    onMovieItemAdded (tmdbId);
    MovieItem * item = qobject_cast<MovieItem *> (m_moviesModel->getByUid (QString::number (tmdbId)));
    if (item) {
        item->updateWithQtVariant (values);
    }
}

void MoviesEngine::onMovieSearchResults (QVariantList list) {
    qDebug () << "MoviesEngine::onMovieSearchResults" << list;
    m_searchModel->clear ();
    foreach (QVariant variant, list) {
        QVariantMap values = variant.toMap ();
        MovieItem * item = MovieItem::fromQtVariant (values);
        m_searchModel->append (item);
    }
}

void MoviesEngine::onReleaseDatesUpdated (QVariantList list) {
    qDebug () << "MoviesEngine::onReleaseDatesUpdated" << list;

}

void MoviesEngine::onCastRolesUpdated (QVariantList list) {
    qDebug () << "MoviesEngine::onCastRolesUpdated" << list;

}

void MoviesEngine::onCrewTeamUpdated (QVariantList list) {
    qDebug () << "MoviesEngine::onCrewTeamUpdated" << list;

}

void MoviesEngine::onAltTitlesUpdated (QVariantList list) {
    qDebug () << "MoviesEngine::onAltTitlesUpdated" << list;

}
