#ifndef MOVIESENGINE_H
#define MOVIESENGINE_H

#include <QObject>
#include <QThread>
#include <QVariantList>
#include <QVariantMap>
#include <QQmlHelpers>
#include <QQmlObjectListModel>
#include <QQmlVariantListModel>

class MoviesWorker;
class MoviesEngine : public QObject {
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, currentMovieId)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, searchModel)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, moviesModel)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, releaseDatesList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, castRolesList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, crewTeamList)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, altTitlesList)

public:
    explicit MoviesEngine         (QObject * parent = NULL);

public slots:
    void requestSearch            (QString name);
    void requestFullMovieInfo     (int tmdbId);
    void requestUpdateVote        (int tmdbId, int vote);

signals:
    void searchRequested          (QString name);
    void fullMovieInfoRequested   (int tmdbId);
    void removeMovieRequested     (int tmdbId);
    void updateMovieVoteRequested (int tmdbId, int vote);

private slots:
    void onMovieItemAdded         (int tmdbId);
    void onMovieItemRemoved       (int tmdbId);
    void onCurrentMovieIdChanged  (int tmdbId);
    void onMovieItemUpdated       (int tmdbId, QVariantMap values);
    void onMovieSearchResults     (QVariantList list);
    void onReleaseDatesUpdated    (QVariantList list);
    void onCastRolesUpdated       (QVariantList list);
    void onCrewTeamUpdated        (QVariantList list);
    void onAltTitlesUpdated       (QVariantList list);

private:
    MoviesWorker * m_worker;
    QThread      * m_thread;
};

#endif // MOVIESENGINE_H
