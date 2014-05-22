#ifndef MOVIEITEM_H
#define MOVIEITEM_H

#include <QObject>
#include <QQmlHelpers>

class MovieItem : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (int,     tmdbId)  // tmdb id
    QML_READONLY_PROPERTY (int,     vote)    // vote between -1 (unseen), 0 (hated) and 10 (loved)
    QML_READONLY_PROPERTY (QString, title)   // original title
    QML_READONLY_PROPERTY (QString, cover)   // poster url
    QML_READONLY_PROPERTY (QString, summary) // summary

public:
    explicit MovieItem (QObject * parent = NULL);
    static   MovieItem * fromQtVariant (const QVariantMap & values);
    virtual ~MovieItem ();

    void updateWithQtVariant (const QVariantMap & values);
};

#endif // MOVIEITEM_H
