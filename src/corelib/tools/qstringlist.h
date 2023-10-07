/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSTRINGLIST_H
#define QSTRINGLIST_H

#include <QtCore/qalgorithms.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qlist.h>
#include <QtCore/qregexp.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringmatcher.h>
#ifdef QT_INCLUDE_COMPAT
#include <Qt3Support/q3valuelist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QRegExp;

typedef QListIterator<QString> QStringListIterator;
typedef QMutableListIterator<QString> QMutableStringListIterator;

class QStringList : public QList<QString>
{
public:
    inline QStringList() {}
    inline explicit QStringList(const QString &i) { append(i); }
    inline QStringList(const QStringList &l) : QList<QString>(l) { }
    inline QStringList& operator=(const QStringList &l) = default;
    inline QStringList(const QList<QString> &l) : QList<QString>(l) { }
#ifdef Q_COMPILER_INITIALIZER_LISTS
    inline QStringList(std::initializer_list<QString> args) : QList<QString>(args) { }
#endif

    inline void sort();
    inline int removeDuplicates();

    inline QString join(const QString &sep) const;
    inline QString join(QChar sep) const;

    inline QStringList filter(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline QStringList &replaceInStrings(const QString &before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    inline QStringList operator+(const QStringList &other) const
    { QStringList n = *this; n += other; return n; }
    inline QStringList &operator<<(const QString &str)
    { append(str); return *this; }
    inline QStringList &operator<<(const QStringList &l)
    { *this += l; return *this; }

#ifndef QT_NO_REGEXP
    inline QStringList filter(const QRegExp &rx) const;
    inline QStringList &replaceInStrings(const QRegExp &rx, const QString &after);
    inline int indexOf(const QRegExp &rx, int from = 0) const;
    inline int lastIndexOf(const QRegExp &rx, int from = -1) const;
    inline int indexOf(QRegExp &rx, int from = 0) const;
    inline int lastIndexOf(QRegExp &rx, int from = -1) const;
#endif
    using QList<QString>::indexOf;
    using QList<QString>::lastIndexOf;
};

namespace QtPrivate {
    void Q_CORE_EXPORT QStringList_sort(QStringList *that);
    int Q_CORE_EXPORT QStringList_removeDuplicates(QStringList *that);
    QString Q_CORE_EXPORT QStringList_join(const QStringList *that, const QChar *sep, int seplen);
    QStringList Q_CORE_EXPORT QStringList_filter(const QStringList *that, const QString &str,
                                               Qt::CaseSensitivity cs);

    bool Q_CORE_EXPORT QStringList_contains(const QStringList *that, const QString &str, Qt::CaseSensitivity cs);
    void Q_CORE_EXPORT QStringList_replaceInStrings(QStringList *that, const QString &before, const QString &after, Qt::CaseSensitivity cs);

#ifndef QT_NO_REGEXP
    void Q_CORE_EXPORT QStringList_replaceInStrings(QStringList *that, const QRegExp &rx, const QString &after);
    QStringList Q_CORE_EXPORT QStringList_filter(const QStringList *that, const QRegExp &re);
    int Q_CORE_EXPORT QStringList_indexOf(const QStringList *that, const QRegExp &rx, int from);
    int Q_CORE_EXPORT QStringList_lastIndexOf(const QStringList *that, const QRegExp &rx, int from);
    int Q_CORE_EXPORT QStringList_indexOf(const QStringList *that, QRegExp &rx, int from);
    int Q_CORE_EXPORT QStringList_lastIndexOf(const QStringList *that, QRegExp &rx, int from);
#endif
}

inline void QStringList::sort()
{
    QtPrivate::QStringList_sort(this);
}

inline int QStringList::removeDuplicates()
{
    return QtPrivate::QStringList_removeDuplicates(this);
}

inline QString QStringList::join(const QString &sep) const
{

    return QtPrivate::QStringList_join(this, sep.constData(), sep.length());
}

inline QString QStringList::join(QChar sep) const
{
    return QtPrivate::QStringList_join(this, &sep, 1);
}

inline QStringList QStringList::filter(const QString &str, Qt::CaseSensitivity cs) const
{
    return QtPrivate::QStringList_filter(this, str, cs);
}

inline bool QStringList::contains(const QString &str, Qt::CaseSensitivity cs) const
{
    return QtPrivate::QStringList_contains(this, str, cs);
}

inline QStringList &QStringList::replaceInStrings(const QString &before, const QString &after, Qt::CaseSensitivity cs)
{
    QtPrivate::QStringList_replaceInStrings(this, before, after, cs);
    return *this;
}

#ifndef QT_NO_REGEXP
inline QStringList &QStringList::replaceInStrings(const QRegExp &rx, const QString &after)
{
    QtPrivate::QStringList_replaceInStrings(this, rx, after);
    return *this;
}

inline QStringList QStringList::filter(const QRegExp &rx) const
{
    return QtPrivate::QStringList_filter(this, rx);
}

inline int QStringList::indexOf(const QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_indexOf(this, rx, from);
}

inline int QStringList::lastIndexOf(const QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_lastIndexOf(this, rx, from);
}

inline int QStringList::indexOf(QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_indexOf(this, rx, from);
}

inline int QStringList::lastIndexOf(QRegExp &rx, int from) const
{
    return QtPrivate::QStringList_lastIndexOf(this, rx, from);
}
#endif


#ifndef QT_NO_DATASTREAM
inline QDataStream &operator>>(QDataStream &in, QStringList &list)
{
    return operator>>(in, static_cast<QList<QString> &>(list));
}
inline QDataStream &operator<<(QDataStream &out, const QStringList &list)
{
    return operator<<(out, static_cast<const QList<QString> &>(list));
}
#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTRINGLIST_H
