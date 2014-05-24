#ifndef MOVIESENGINE_H
#define MOVIESENGINE_H

#include <QObject>
#include <QThread>
#include <QVariantList>
#include <QVariantMap>
#include <QQmlHelpers>
#include <QSortFilterProxyModel>
#include <QQmlObjectListModel>
#include <QQmlVariantListModel>

class MovieItem;
class MoviesWorker;

class MoviesSortFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    QML_WRITABLE_PROPERTY (bool, toggleFilterSeen)

public:
    explicit MoviesSortFilterProxy (QQmlObjectListModel * model, QObject * parent = NULL);

protected:
    bool filterAcceptsRow (int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan (const QModelIndex & left, const QModelIndex & right) const;

private slots:
    void onToggleFilterSeenChanged ();

private:
    int                   m_roleTmdbId;
    int                   m_roleVote;
    QQmlObjectListModel * m_moviesModel;
};

class MoviesEngine : public QObject {
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int,                     currentMovieId)
    QML_READONLY_PROPERTY (MovieItem             *, currentMovieObject)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel   *, searchModel)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel   *, moviesModel)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel  *, releaseDatesList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel  *, castRolesList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel  *, crewTeamList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel  *, altTitlesList)
    QML_CONSTANT_PROPERTY (QSortFilterProxyModel *, sortedFilteredMoviesModel)

public:
    explicit MoviesEngine         (QObject * parent = NULL);

    Q_INVOKABLE QString getFullCountryName  (QString countryCode);
    Q_INVOKABLE QString getFullLanguageName (QString languageCode);

public slots:
    void requestSearch             (QString name);
    void requestFullMovieInfo      (int tmdbId);
    void requestRemoveMovie        (int tmdbId);
    void requestUpdateVote         (int tmdbId, int vote);

signals:
    void searchRequested           (QString name);
    void fullMovieInfoRequested    (int tmdbId);
    void loadMovieDetailsRequested (int tmdbId);
    void removeMovieRequested      (int tmdbId);
    void updateMovieVoteRequested  (int tmdbId, int vote);

private slots:
    void onMovieItemAdded          (int tmdbId);
    void onMovieItemRemoved        (int tmdbId);
    void onCurrentMovieIdChanged   (int tmdbId);
    void onMovieItemUpdated        (int tmdbId, QVariantMap values);
    void onMovieSearchResults      (QVariantList list);
    void onReleaseDatesUpdated     (QVariantList list);
    void onCastRolesUpdated        (QVariantList list);
    void onCrewTeamUpdated         (QVariantList list);
    void onAltTitlesUpdated        (QVariantList list);

private:
    QThread                 * m_thread;
    MoviesWorker            * m_worker;
    QHash<QString, QString>   m_hashCountries;
    QHash<QString, QString>   m_hashLanguages;
};

#endif // MOVIESENGINE_H
