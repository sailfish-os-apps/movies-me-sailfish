
#include "MoviesEngine.h"
#include "MoviesWorker.h"
#include "MovieItem.h"

#include "commondefs.h"

#include <QDebug>

MoviesEngine::MoviesEngine (QObject * parent) : QObject (parent) {
    m_currentMovieId     = -1;
    m_currentMovieObject = NULL;

    m_searchModel = QQmlObjectListModel::create<MovieItem> (this);
    m_moviesModel = QQmlObjectListModel::create<MovieItem> (this);

    m_moviesModel->setRoleNameForUid (QByteArrayLiteral ("tmdbId"));

    m_sortedFilteredMoviesModel = new MoviesSortFilterProxy (m_moviesModel, this);

    m_releaseDatesList = new QQmlVariantListModel (this); // { "certification" : "R", "iso_3166_1" : "US", "release_date" : "2002-01-10" }
    m_castRolesList    = new QQmlVariantListModel (this); // { "character" : "Mr Bean", "name" : "The Actor", "profile_path" : "img/url" }
    m_crewTeamList     = new QQmlVariantListModel (this); // { "job" : "Job name", "name" : "The Person", "profile_path" : "img/url" }
    m_altTitlesList    = new QQmlVariantListModel (this); // { "iso_3166_1" : "US", "title" : "The US title" }

    for (int countryEnum = 0; countryEnum < QLocale::LastCountry; countryEnum++) {
        m_hashCountries.insert (ISO::countryCodeFromISO3166 (QLocale::Country (countryEnum)),
                                ISO::countryNameFromISO3166 (QLocale::Country (countryEnum)));
    }
    //qDebug () << "m_hashCountries:" << m_hashCountries.count () << m_hashCountries;

    for (int langEnum = 0; langEnum < QLocale::LastLanguage; langEnum++) {
        m_hashLanguages.insert (ISO::languageCodeFromISO639 (QLocale::Language (langEnum)),
                                ISO::languageNameFromISO639 (QLocale::Language (langEnum)));
    }
    //qDebug () << "m_hashLanguages:" << m_hashLanguages.count () << m_hashLanguages;

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
    connect (this,      &MoviesEngine::loadMovieDetailsRequested,   m_worker, &MoviesWorker::loadMovieDetailsFromDb);

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

QString MoviesEngine::getFullCountryName (QString countryCode) {
    return m_hashCountries.value (countryCode, countryCode);
}

QString MoviesEngine::getFullLanguageName (QString languageCode) {
    return m_hashLanguages.value (languageCode, languageCode);
}

void MoviesEngine::requestSearch (QString name) {
    //qDebug () << "MoviesEngine::requestSearch" << name;
    emit searchRequested (name);
}

void MoviesEngine::requestFullMovieInfo (int tmdbId) {
    //qDebug () << "MoviesEngine::requestFullMovieInfo" << tmdbId;
    emit fullMovieInfoRequested (tmdbId);
}

void MoviesEngine::requestRemoveMovie (int tmdbId) {
    //qDebug () << "MoviesEngine::requestRemoveMovie" << tmdbId;
    if (tmdbId == m_currentMovieId) {
        set_currentMovieId (-1);
    }
    emit removeMovieRequested (tmdbId);
}

void MoviesEngine::requestUpdateVote (int tmdbId, int vote) {
    //qDebug () << "MoviesEngine::requestUpdateVote" << tmdbId << vote;
    emit updateMovieVoteRequested (tmdbId, vote);
}

void MoviesEngine::onMovieItemAdded (int tmdbId) {
    //qDebug () << "MoviesEngine::onMovieItemAdded" << tmdbId;
    if (!m_moviesModel->getByUid (QString::number (tmdbId))) {
        MovieItem * item = new MovieItem;
        item->update_tmdbId (tmdbId);
        m_moviesModel->append (item);
    }
}

void MoviesEngine::onMovieItemRemoved (int tmdbId) {
    //qDebug () << "MoviesEngine::onMovieItemRemoved" << tmdbId;
    QObject * item = m_moviesModel->getByUid (QString::number (tmdbId));
    if (item) {
        m_moviesModel->remove (item);
    }
}

void MoviesEngine::onCurrentMovieIdChanged (int tmdbId) {
    //qDebug () << "MoviesEngine::onCurrentMovieIdChanged" << tmdbId;
    m_crewTeamList->clear     ();
    m_castRolesList->clear    ();
    m_altTitlesList->clear    ();
    m_releaseDatesList->clear ();
    if (tmdbId > -1) {
        update_currentMovieObject (qobject_cast<MovieItem *> (m_moviesModel->getByUid (QString::number (tmdbId))));
        emit loadMovieDetailsRequested (tmdbId);
    }
    else {
        update_currentMovieObject (NULL);
    }
}

void MoviesEngine::onMovieItemUpdated (int tmdbId, QVariantMap values) {
    //qDebug () << "MoviesEngine::onMovieItemUpdated" << tmdbId << values;
    onMovieItemAdded (tmdbId);
    MovieItem * item = qobject_cast<MovieItem *> (m_moviesModel->getByUid (QString::number (tmdbId)));
    if (item) {
        item->updateWithQtVariant (values);
    }
}

void MoviesEngine::onMovieSearchResults (QVariantList list) {
    //qDebug () << "MoviesEngine::onMovieSearchResults" << list;
    m_searchModel->clear ();
    foreach (QVariant variant, list) {
        QVariantMap values = variant.toMap ();
        MovieItem * item = MovieItem::fromQtVariant (values);
        m_searchModel->append (item);
    }
}

void MoviesEngine::onReleaseDatesUpdated (QVariantList list) {
    //qDebug () << "MoviesEngine::onReleaseDatesUpdated" << list;
    m_releaseDatesList->clear ();
    m_releaseDatesList->appendList (list);
}

void MoviesEngine::onCastRolesUpdated (QVariantList list) {
    //qDebug () << "MoviesEngine::onCastRolesUpdated" << list;
    m_castRolesList->clear ();
    m_castRolesList->appendList (list);
}

void MoviesEngine::onCrewTeamUpdated (QVariantList list) {
    //qDebug () << "MoviesEngine::onCrewTeamUpdated" << list;
    m_crewTeamList->clear ();
    m_crewTeamList->appendList (list);
}

void MoviesEngine::onAltTitlesUpdated (QVariantList list) {
    //qDebug () << "MoviesEngine::onAltTitlesUpdated" << list;
    m_altTitlesList->clear ();
    m_altTitlesList->appendList (list);
}


MoviesSortFilterProxy::MoviesSortFilterProxy (QQmlObjectListModel * model, QObject * parent) : QSortFilterProxyModel (parent) {
    m_toggleFilterSeen = true;
    m_moviesModel = model;
    if (m_moviesModel) {
        m_roleTmdbId  = m_moviesModel->roleForName ("tmdbId");
        m_roleVote    = m_moviesModel->roleForName ("vote");
        setSourceModel (m_moviesModel);
        setSortRole (m_roleTmdbId);
        setDynamicSortFilter (true);
    }
    connect (this, &MoviesSortFilterProxy::toggleFilterSeenChanged,
             this, &MoviesSortFilterProxy::onToggleFilterSeenChanged);
}

bool MoviesSortFilterProxy::filterAcceptsRow (int sourceRow, const QModelIndex & sourceParent) const {
    QModelIndex modelIndex = sourceModel ()->index (sourceRow, 0, sourceParent);
    int vote = sourceModel ()->data (modelIndex, m_roleVote).toInt ();
    return (m_toggleFilterSeen ? vote >= 0 : vote < 0);
}

bool MoviesSortFilterProxy::lessThan (const QModelIndex & left, const QModelIndex & right) const {
    int leftTmdbId  = sourceModel()->data (left,  m_roleTmdbId).toInt ();
    int rightTmdbId = sourceModel()->data (right, m_roleTmdbId).toInt ();
    return (leftTmdbId < rightTmdbId);
}

void MoviesSortFilterProxy::onToggleFilterSeenChanged () {
    invalidateFilter ();
}
