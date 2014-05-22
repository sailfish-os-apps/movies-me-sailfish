
#include "MovieItem.h"

#include <QVariant>
#include <QVariantMap>

MovieItem::MovieItem (QObject * parent) : QObject (parent) {
    m_tmdbId  = 0;
    m_vote    = -1;
    m_title   = QStringLiteral ("");
    m_cover   = QStringLiteral ("");
    m_summary = QStringLiteral ("");
}

MovieItem * MovieItem::fromQtVariant (const QVariantMap & values) {
    MovieItem * ret = new MovieItem;
    ret->updateWithQtVariant (values);
    return ret;
}

void MovieItem::updateWithQtVariant (const QVariantMap & values) {
    if (values.contains (QStringLiteral ("tmdbId"))) {
        update_tmdbId (values.value (QStringLiteral ("tmdbId")).value<int> ());
    }
    if (values.contains (QStringLiteral ("vote"))) {
        update_vote (values.value (QStringLiteral ("vote")).value<int> ());
    }
    if (values.contains (QStringLiteral ("title"))) {
        update_title (values.value (QStringLiteral ("title")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("cover"))) {
        update_cover (values.value (QStringLiteral ("cover")).value<QString> ());
    }
    if (values.contains (QStringLiteral ("summary"))) {
        update_summary (values.value (QStringLiteral ("summary")).value<QString> ());
    }
}

MovieItem::~MovieItem () { }
