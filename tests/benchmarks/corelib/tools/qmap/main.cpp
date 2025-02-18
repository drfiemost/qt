/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QFile>
#include <QMap>
#include <QString>
#include <QTest>
#include <qdebug.h>


class tst_QMap : public QObject
{
    Q_OBJECT

private slots:
    void insertion_int_int();
    void insertion_int_string();
    void insertion_string_int();

    void lookup_int_int();
    void lookup_int_string();
    void lookup_string_int();

    void iteration();
    void toStdMap();
    void iterator_begin();
};


void tst_QMap::insertion_int_int()
{
    QMap<int, int> map;
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            map.insert(i, i);
    }
}

void tst_QMap::insertion_int_string()
{
    QMap<int, QString> map;
    QString str("Hello World");
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            map.insert(i, str);
    }
}

void tst_QMap::insertion_string_int()
{
    QMap<QString, int> map;
    QString str("Hello World");
    QBENCHMARK {
        for (int i = 1; i < 100000; ++i) {
            str[0] = QChar(i);
            map.insert(str, i);
        }
    }
}


void tst_QMap::lookup_int_int()
{
    QMap<int, int> map;
    for (int i = 0; i < 100000; ++i)
        map.insert(i, i);

    int sum = 0;
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
             sum += map.value(i);
    }
}

void tst_QMap::lookup_int_string()
{
    QMap<int, QString> map;
    QString str("Hello World");
    for (int i = 0; i < 100000; ++i)
        map.insert(i, str);

    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
             str += map.value(i);
    }
}

void tst_QMap::lookup_string_int()
{
    QMap<QString, int> map;
    QString str("Hello World");
    for (int i = 1; i < 100000; ++i) {
        str[0] = QChar(i);
        map.insert(str, i);
    }

    int sum = 0;
    QBENCHMARK {
        for (int i = 1; i < 100000; ++i) {
            str[0] = QChar(i);
            sum += map.value(str);
        }
    }
}

// iteration speed doesn't depend on the type of the map.
void tst_QMap::iteration()
{
    QMap<int, int> map;
    for (int i = 0; i < 100000; ++i)
        map.insert(i, i);

    int j = 0;
    QBENCHMARK {
        for (int i = 0; i < 100; ++i) {
            QMap<int, int>::const_iterator it = map.constBegin();
            QMap<int, int>::const_iterator end = map.constEnd();
            while (it != end) {
                j += *it;
                ++it;
            }
        }
    }
}

void tst_QMap::toStdMap()
{
    QMap<int, int> map;
    for (int i = 0; i < 100000; ++i)
        map.insert(i, i);
    QBENCHMARK {
        std::map<int, int> n = map.toStdMap();
        n.begin();
    }
}

void tst_QMap::iterator_begin()
{
    QMap<int, int> map;
    for (int i = 0; i < 100000; ++i)
        map.insert(i, i);

    QBENCHMARK {
        for (int i = 0; i < 100000; ++i) {
            QMap<int, int>::const_iterator it = map.constBegin();
            QMap<int, int>::const_iterator end = map.constEnd();
            if (it == end) // same as if (false)
                ++it;
        }
    }
}


QTEST_MAIN(tst_QMap)

#include "main.moc"
