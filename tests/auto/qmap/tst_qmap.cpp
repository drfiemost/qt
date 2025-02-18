/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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

#define QT_STRICT_ITERATORS

#include <qmap.h>
#include <QtTest/QtTest>
#include <QDebug>



//TESTED_CLASS=
//TESTED_FILES=

class tst_QMap : public QObject
{
    Q_OBJECT

public:
    tst_QMap();

protected:
    template <class KEY, class VALUE>
    void sanityCheckTree(const QMap<KEY, VALUE> &m, int calledFromLine);
public slots:
    void init();
private slots:
    void count();
    void clear();
    void beginEnd();
    void key();

    void swap();

    void operator_eq();

    void empty();
    void contains();
    void find();
    void constFind();
    void lowerUpperBound();
    void mergeCompare();
    void take();

    void iterators();
    void keys_values_uniqueKeys();
    void qmultimap_specific();

    void const_shared_null();

    void equal_range();
    void setSharable();

    void insert();
    void checkMostLeftNode();
    void eraseValidIteratorOnSharedMap();
};

tst_QMap::tst_QMap()
{
}

typedef QMap<QString, QString> StringMap;

class MyClass
{
public:
    MyClass() {
       ++count;
//     qDebug("creating MyClass count=%d", count);
    }
    MyClass( const QString& c) {
	count++; str = c;
// 	qDebug("creating MyClass '%s' count = %d", str.latin1(), count);
    }
    ~MyClass() {
	count--;
// 	qDebug("deleting MyClass '%s' count = %d", str.latin1(), count);
    }
    MyClass( const MyClass& c ) {
	count++; str = c.str;
// 	qDebug("creating MyClass '%s' count = %d", str.latin1(), count);
    }
    MyClass &operator =(const MyClass &o) {
// 	qDebug("copying MyClass '%s'", o.str.latin1());
	str = o.str; return *this;
    }

    QString str;
    static int count;
};

int MyClass::count = 0;

typedef QMap<QString, MyClass> MyMap;

QDebug operator << (QDebug d, const MyClass &c) {
    d << c.str;
    return d;
}

template <class KEY, class VALUE>
void tst_QMap::sanityCheckTree(const QMap<KEY, VALUE> &m, int calledFromLine)
{
    QString possibleFrom;
    possibleFrom.setNum(calledFromLine);
    possibleFrom = "Called from line: " + possibleFrom;
    int count = 0;
    typename QMap<KEY, VALUE>::const_iterator oldite = m.constBegin();
    for (typename QMap<KEY, VALUE>::const_iterator i = m.constBegin(); i != m.constEnd(); ++i) {
        count++;
        bool oldIteratorIsLarger = i.key() < oldite.key();
        QVERIFY2(!oldIteratorIsLarger, possibleFrom.toUtf8());
        oldite = i;
    }
    if (m.size() != count) { // Fail
        qDebug() << possibleFrom;
        QCOMPARE(m.size(), count);
    }
    if (m.size() == 0)
        QVERIFY(m.constBegin() == m.constEnd());
}

void tst_QMap::init()
{
    MyClass::count = 0;
}

void tst_QMap::count()
{
    {
	MyMap map;
	MyMap map2( map );
	QCOMPARE( map.count(), 0 );
	QCOMPARE( map2.count(), 0 );
	QCOMPARE( MyClass::count, int(0) );
	// detach
	map2["Hallo"] = MyClass( "Fritz" );
	QCOMPARE( map.count(), 0 );
        QCOMPARE( map2.count(), 1 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 1 );
#endif
    }
    QCOMPARE( MyClass::count, int(0) );

    {
	typedef QMap<QString, MyClass> Map;
	Map map;
	QCOMPARE( map.count(), 0);
	map.insert( "Torben", MyClass("Weis") );
	QCOMPARE( map.count(), 1 );
	map.insert( "Claudia", MyClass("Sorg") );
	QCOMPARE( map.count(), 2 );
	map.insert( "Lars", MyClass("Linzbach") );
	map.insert( "Matthias", MyClass("Ettrich") );
	map.insert( "Sue", MyClass("Paludo") );
	map.insert( "Eirik", MyClass("Eng") );
	map.insert( "Haavard", MyClass("Nord") );
	map.insert( "Arnt", MyClass("Gulbrandsen") );
	map.insert( "Paul", MyClass("Tvete") );
	QCOMPARE( map.count(), 9 );
	map.insert( "Paul", MyClass("Tvete 1") );
	map.insert( "Paul", MyClass("Tvete 2") );
	map.insert( "Paul", MyClass("Tvete 3") );
	map.insert( "Paul", MyClass("Tvete 4") );
	map.insert( "Paul", MyClass("Tvete 5") );
	map.insert( "Paul", MyClass("Tvete 6") );

	QCOMPARE( map.count(), 9 );
    QCOMPARE( map.count("Paul"), 1 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	Map map2( map );
	QVERIFY( map2.count() == 9 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	map2.insert( "Kay", MyClass("Roemer") );
	QVERIFY( map2.count() == 10 );
	QVERIFY( map.count() == 9 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 19 );
#endif

	map2 = map;
	QVERIFY( map.count() == 9 );
	QVERIFY( map2.count() == 9 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	map2.insert( "Kay", MyClass("Roemer") );
	QVERIFY( map2.count() == 10 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 19 );
#endif

	map2.clear();
	QVERIFY( map.count() == 9 );
	QVERIFY( map2.count() == 0 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	map2 = map;
	QVERIFY( map.count() == 9 );
	QVERIFY( map2.count() == 9 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	map2.clear();
	QVERIFY( map.count() == 9 );
	QVERIFY( map2.count() == 0 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 9 );
#endif

	map.remove( "Lars" );
	QVERIFY( map.count() == 8 );
	QVERIFY( map2.count() == 0 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 8 );
#endif

	map.remove( "Mist" );
	QVERIFY( map.count() == 8 );
	QVERIFY( map2.count() == 0 );
#ifndef Q_CC_SUN
	QCOMPARE( MyClass::count, 8 );
#endif
    }
    QVERIFY( MyClass::count == 0 );

    {
	typedef QMap<QString,MyClass> Map;
	Map map;
	map["Torben"] = MyClass("Weis");
#ifndef Q_CC_SUN
	QVERIFY( MyClass::count == 1 );
#endif
	QVERIFY( map.count() == 1 );

	(void)map["Torben"].str;
	(void)map["Lars"].str;
#ifndef Q_CC_SUN
	QVERIFY( MyClass::count == 2 );
#endif
	QVERIFY( map.count() == 2 );

	const Map& cmap = map;
	(void)cmap["Depp"].str;
#ifndef Q_CC_SUN
	QVERIFY( MyClass::count == 2 );
#endif
	QVERIFY( map.count() == 2 );
	QVERIFY( cmap.count() == 2 );
    }
    QCOMPARE( MyClass::count, 0 );
    {
	for ( int i = 0; i < 100; ++i )
	{
	    QMap<int, MyClass> map;
	    for (int j = 0; j < i; ++j)
		map.insert(j, MyClass(QString::number(j)));
	}
	QCOMPARE( MyClass::count, 0 );
    }
    QCOMPARE( MyClass::count, 0 );
}

void tst_QMap::clear()
{
    {
        MyMap map;
        map.clear();
        QVERIFY( map.isEmpty() );
        map.insert( "key", MyClass( "value" ) );
        map.clear();
        QVERIFY( map.isEmpty() );
        map.insert( "key0", MyClass( "value0" ) );
        map.insert( "key0", MyClass( "value1" ) );
        map.insert( "key1", MyClass( "value2" ) );
        map.clear();
        sanityCheckTree(map, __LINE__);
        QVERIFY( map.isEmpty() );
    }
    QCOMPARE( MyClass::count, int(0) );
}

void tst_QMap::beginEnd()
{
    StringMap m0;
    QVERIFY( m0.begin() == m0.end() );
    QVERIFY( m0.begin() == m0.begin() );

    // sample string->string map
    StringMap map;
    QVERIFY( map.constBegin() == map.constEnd() );
    map.insert( "0", "a" );
    map.insert( "1", "b" );

    // make a copy. const function shouldn't detach
    StringMap map2 = map;
    QVERIFY( map.constBegin() == map2.constBegin() );
    QVERIFY( map.constEnd() == map2.constEnd() );

    // test iteration
    QString result;
    for ( StringMap::ConstIterator it = map.constBegin();
	  it != map.constEnd(); ++it )
	result += *it;
    QCOMPARE( result, QString( "ab" ) );

    // maps should still be identical
    QVERIFY( map.constBegin() == map2.constBegin() );
    QVERIFY( map.constEnd() == map2.constEnd() );

    // detach
    map2.insert( "2", "c" );
    QVERIFY( map.constBegin() == map.constBegin() );
    QVERIFY( map.constBegin() != map2.constBegin() );
}

void tst_QMap::key()
{
    {
        QString def("default value");

        QMap<QString, int> map1;
        QCOMPARE(map1.key(1), QString());
        QCOMPARE(map1.key(1, def), def);

        map1.insert("one", 1);
        QCOMPARE(map1.key(1), QString("one"));
        QCOMPARE(map1.key(1, def), QString("one"));
        QCOMPARE(map1.key(2), QString());
        QCOMPARE(map1.key(2, def), def);

        map1.insert("two", 2);
        QCOMPARE(map1.key(1), QString("one"));
        QCOMPARE(map1.key(1, def), QString("one"));
        QCOMPARE(map1.key(2), QString("two"));
        QCOMPARE(map1.key(2, def), QString("two"));
        QCOMPARE(map1.key(3), QString());
        QCOMPARE(map1.key(3, def), def);

        map1.insert("deux", 2);
        QCOMPARE(map1.key(1), QString("one"));
        QCOMPARE(map1.key(1, def), QString("one"));
        QVERIFY(map1.key(2) == "deux" || map1.key(2) == "two");
        QVERIFY(map1.key(2, def) == "deux" || map1.key(2, def) == "two");
        QCOMPARE(map1.key(3), QString());
        QCOMPARE(map1.key(3, def), def);
    }

    {
        int def = 666;

        QMap<int, QString> map2;
        QCOMPARE(map2.key("one"), 0);
        QCOMPARE(map2.key("one", def), def);

        map2.insert(1, "one");
        QCOMPARE(map2.key("one"), 1);
        QCOMPARE(map2.key("one", def), 1);
        QCOMPARE(map2.key("two"), 0);
        QCOMPARE(map2.key("two", def), def);

        map2.insert(2, "two");
        QCOMPARE(map2.key("one"), 1);
        QCOMPARE(map2.key("one", def), 1);
        QCOMPARE(map2.key("two"), 2);
        QCOMPARE(map2.key("two", def), 2);
        QCOMPARE(map2.key("three"), 0);
        QCOMPARE(map2.key("three", def), def);

        map2.insert(3, "two");
        QCOMPARE(map2.key("one"), 1);
        QCOMPARE(map2.key("one", def), 1);
        QCOMPARE(map2.key("two"), 2);
        QCOMPARE(map2.key("two", def), 2);
        QCOMPARE(map2.key("three"), 0);
        QCOMPARE(map2.key("three", def), def);

        map2.insert(-1, "two");
        QCOMPARE(map2.key("two"), -1);
        QCOMPARE(map2.key("two", def), -1);

        map2.insert(0, "zero");
        QCOMPARE(map2.key("zero"), 0);
        QCOMPARE(map2.key("zero", def), 0);
    }
}

void tst_QMap::swap()
{
    QMap<int,QString> m1, m2;
    m1[0] = "m1[0]";
    m2[1] = "m2[1]";
    m1.swap(m2);
    QCOMPARE(m1.value(1),QLatin1String("m2[1]"));
    QCOMPARE(m2.value(0),QLatin1String("m1[0]"));
    sanityCheckTree(m1, __LINE__);
    sanityCheckTree(m2, __LINE__);
}

void tst_QMap::operator_eq()
{
    {
        // compare for equality:
        QMap<int, int> a;
        QMap<int, int> b;

        QVERIFY(a == b);
        QVERIFY(!(a != b));

        a.insert(1,1);
        b.insert(1,1);
        QVERIFY(a == b);
        QVERIFY(!(a != b));

        a.insert(0,1);
        b.insert(0,1);
        QVERIFY(a == b);
        QVERIFY(!(a != b));

        // compare for inequality:
        a.insert(42,0);
        QVERIFY(a != b);
        QVERIFY(!(a == b));

        a.insert(65, -1);
        QVERIFY(a != b);
        QVERIFY(!(a == b));

        b.insert(-1, -1);
        QVERIFY(a != b);
        QVERIFY(!(a == b));
    }

    {
        // a more complex map
        QMap<QString, QString> a;
        QMap<QString, QString> b;

        QVERIFY(a == b);
        QVERIFY(!(a != b));

        a.insert("Hello", "World");
        QVERIFY(a != b);
        QVERIFY(!(a == b));

        b.insert("Hello", "World");
        QVERIFY(a == b);
        QVERIFY(!(a != b));

        a.insert("Goodbye", "cruel world");
        QVERIFY(a != b);
        QVERIFY(!(a == b));

        b.insert("Goodbye", "cruel world");

        // what happens if we insert nulls?
        a.insert(QString(), QString());
        QVERIFY(a != b);
        QVERIFY(!(a == b));

        // empty keys and null keys match:
        b.insert(QString(""), QString());
        QVERIFY(a == b);
        QVERIFY(!(a != b));
    }

    {
        // task 102658

        QMap<QString, int> a;
        QMap<QString, int> b;

        a.insert("otto", 1);
        b.insert("willy", 1);
        QVERIFY(a != b);
        QVERIFY(!(a == b));
    }
}

void tst_QMap::empty()
{
    QMap<int, QString> map1;

    QVERIFY(map1.isEmpty());

    map1.insert(1, "one");
    QVERIFY(!map1.isEmpty());

    map1.clear();
    QVERIFY(map1.isEmpty());

}

void tst_QMap::contains()
{
    QMap<int, QString> map1;
    int i;

    map1.insert(1, "one");
    QVERIFY(map1.contains(1));

    for(i=2; i < 100; ++i)
        map1.insert(i, "teststring");
    for(i=99; i > 1; --i)
        QVERIFY(map1.contains(i));

    map1.remove(43);
    QVERIFY(!map1.contains(43));
}

void tst_QMap::find()
{
    QMap<int, QString> map1;
    QString testString="Teststring %0";
    QString compareString;
    int i,count=0;

    QVERIFY(map1.find(1) == map1.end());

    map1.insert(1,"Mensch");
    map1.insert(1,"Mayer");
    map1.insert(2,"Hej");

    QVERIFY(map1.find(1).value() == "Mayer");
    QVERIFY(map1.find(2).value() == "Hej");

    for(i = 3; i < 10; ++i) {
        compareString = testString.arg(i);
        map1.insertMulti(4, compareString);
        QCOMPARE(map1.count(4), i - 2);
    }

    QMap<int, QString>::const_iterator it=map1.constFind(4);

    for(i = 9; i > 2 && it != map1.constEnd() && it.key() == 4; --i) {
        compareString = testString.arg(i);
        QVERIFY(it.value() == compareString);
        ++it;
        ++count;
    }
    QCOMPARE(count, 7);
}

void tst_QMap::constFind()
{
    QMap<int, QString> map1;
    QString testString="Teststring %0";
    QString compareString;
    int i,count=0;

    QVERIFY(map1.constFind(1) == map1.constEnd());

    map1.insert(1,"Mensch");
    map1.insert(1,"Mayer");
    map1.insert(2,"Hej");

    QVERIFY(map1.constFind(4) == map1.constEnd());

    QVERIFY(map1.constFind(1).value() == "Mayer");
    QVERIFY(map1.constFind(2).value() == "Hej");

    for(i = 3; i < 10; ++i) {
        compareString = testString.arg(i);
        map1.insertMulti(4, compareString);
    }

    QMap<int, QString>::const_iterator it=map1.constFind(4);

    for(i = 9; i > 2 && it != map1.constEnd() && it.key() == 4; --i) {
        compareString = testString.arg(i);
        QVERIFY(it.value() == compareString);
        ++it;
        ++count;
    }
    QCOMPARE(count, 7);
}

void tst_QMap::lowerUpperBound()
{
    QMap<int, QString> map1;

    map1.insert(1, "one");
    map1.insert(5, "five");
    map1.insert(10, "ten");


    //Copied from documentation

    QCOMPARE(map1.upperBound(0).key(), 1);      // returns iterator to (1, "one")
    QCOMPARE(map1.upperBound(1).key(), 5);      // returns iterator to (5, "five")
    QCOMPARE(map1.upperBound(2).key(), 5);      // returns iterator to (5, "five")
    QVERIFY(map1.upperBound(10) == map1.end());     // returns end()
    QVERIFY(map1.upperBound(999) == map1.end());    // returns end()

    QCOMPARE(map1.lowerBound(0).key(), 1);      // returns iterator to (1, "one")
    QCOMPARE(map1.lowerBound(1).key(), 1);      // returns iterator to (1, "one")
    QCOMPARE(map1.lowerBound(2).key(), 5);      // returns iterator to (5, "five")
    QCOMPARE(map1.lowerBound(10).key(), 10);     // returns iterator to (10, "ten")
    QVERIFY(map1.lowerBound(999) == map1.end());    // returns end()

    map1.insert(3, "three");
    map1.insert(7, "seven");
    map1.insertMulti(7, "seven_2");

    QCOMPARE(map1.upperBound(0).key(), 1);
    QCOMPARE(map1.upperBound(1).key(), 3);
    QCOMPARE(map1.upperBound(2).key(), 3);
    QCOMPARE(map1.upperBound(3).key(), 5);
    QCOMPARE(map1.upperBound(7).key(), 10);
    QVERIFY(map1.upperBound(10) == map1.end());
    QVERIFY(map1.upperBound(999) == map1.end());

    QCOMPARE(map1.lowerBound(0).key(), 1);
    QCOMPARE(map1.lowerBound(1).key(), 1);
    QCOMPARE(map1.lowerBound(2).key(), 3);
    QCOMPARE(map1.lowerBound(3).key(), 3);
    QCOMPARE(map1.lowerBound(4).key(), 5);
    QCOMPARE(map1.lowerBound(5).key(), 5);
    QCOMPARE(map1.lowerBound(6).key(), 7);
    QCOMPARE(map1.lowerBound(7).key(), 7);
    QCOMPARE(map1.lowerBound(6).value(), QString("seven_2"));
    QCOMPARE(map1.lowerBound(7).value(), QString("seven_2"));
    QCOMPARE((++map1.lowerBound(6)).value(), QString("seven"));
    QCOMPARE((++map1.lowerBound(7)).value(), QString("seven"));
    QCOMPARE(map1.lowerBound(10).key(), 10);
    QVERIFY(map1.lowerBound(999) == map1.end());
}

void tst_QMap::mergeCompare()
{
    QMap<int, QString> map1, map2, map3, map1b, map2b;

    map1.insert(1,"ett");
    map1.insert(3,"tre");
    map1.insert(5,"fem");

    map2.insert(2,"tvo");
    map2.insert(4,"fyra");

    map1.unite(map2);
    sanityCheckTree(map1, __LINE__);

    map1b = map1;
    map2b = map2;
    map2b.insert(0, "nul");
    map1b.unite(map2b);
    sanityCheckTree(map1b, __LINE__);

    QVERIFY(map1.value(1) == "ett");
    QVERIFY(map1.value(2) == "tvo");
    QVERIFY(map1.value(3) == "tre");
    QVERIFY(map1.value(4) == "fyra");
    QVERIFY(map1.value(5) == "fem");

    map3.insert(1, "ett");
    map3.insert(2, "tvo");
    map3.insert(3, "tre");
    map3.insert(4, "fyra");
    map3.insert(5, "fem");

    QVERIFY(map1 == map3);
}

void tst_QMap::take()
{
    QMap<int, QString> map;

    map.insert(2, "zwei");
    map.insert(3, "drei");

    QVERIFY(map.take(3) == "drei");
    QVERIFY(!map.contains(3));
}

void tst_QMap::iterators()
{
    QMap<int, QString> map;
    QString testString="Teststring %1";
    int i;

    for(i = 1; i < 100; ++i)
        map.insert(i, testString.arg(i));

    //STL-Style iterators

    QMap<int, QString>::iterator stlIt = map.begin();
    QVERIFY(stlIt.value() == "Teststring 1");

    stlIt+=5;
    QVERIFY(stlIt.value() == "Teststring 6");

    stlIt++;
    QVERIFY(stlIt.value() == "Teststring 7");

    stlIt-=3;
    QVERIFY(stlIt.value() == "Teststring 4");

    stlIt--;
    QVERIFY(stlIt.value() == "Teststring 3");

    for(stlIt = map.begin(), i = 1; stlIt != map.end(), i < 100; ++stlIt, ++i)
            QVERIFY(stlIt.value() == testString.arg(i));

    //STL-Style const-iterators

    QMap<int, QString>::const_iterator cstlIt = map.constBegin();
    QVERIFY(cstlIt.value() == "Teststring 1");

    cstlIt+=5;
    QVERIFY(cstlIt.value() == "Teststring 6");

    cstlIt++;
    QVERIFY(cstlIt.value() == "Teststring 7");

    cstlIt-=3;
    QVERIFY(cstlIt.value() == "Teststring 4");

    cstlIt--;
    QVERIFY(cstlIt.value() == "Teststring 3");

    for(cstlIt = map.constBegin(), i = 1; cstlIt != map.constEnd(), i < 100; ++cstlIt, ++i)
            QVERIFY(cstlIt.value() == testString.arg(i));

    //Java-Style iterators

    QMapIterator<int, QString> javaIt(map);

    i = 0;
    while(javaIt.hasNext()) {
        ++i;
        javaIt.next();
        QVERIFY(javaIt.value() == testString.arg(i));
    }

    ++i;
    while(javaIt.hasPrevious()) {
        --i;
        javaIt.previous();
        QVERIFY(javaIt.value() == testString.arg(i));
    }

    /*
        I've removed findNextKey() and findPreviousKey() from the API
        for Qt 4.0 beta 1.
    */

#if 0
    QVERIFY(javaIt.findNextKey(50));
    QVERIFY(javaIt.value() == "Teststring 50");
#endif

    i = 51;
    while(javaIt.hasPrevious()) {
        --i;
        javaIt.previous();
        QVERIFY(javaIt.value() == testString.arg(i));
    }

#if 0
    QVERIFY(javaIt.findNextKey(50));
    QVERIFY(javaIt.value() == "Teststring 50");

    QVERIFY(javaIt.hasPrevious());
    QVERIFY(javaIt.findPreviousKey(20));
    QCOMPARE(javaIt.value(), QString("Teststring 20"));
#endif
}

void tst_QMap::keys_values_uniqueKeys()
{
    QMap<QString, int> map;
    QVERIFY(map.uniqueKeys().isEmpty());
    QVERIFY(map.keys().isEmpty());
    QVERIFY(map.values().isEmpty());

    map.insertMulti("alpha", 1);
    QVERIFY(map.keys() == (QList<QString>() << "alpha"));
    QVERIFY(map.uniqueKeys() == map.keys());
    QVERIFY(map.values() == (QList<int>() << 1));

    map.insertMulti("beta", -2);
    QVERIFY(map.keys() == (QList<QString>() << "alpha" << "beta"));
    QVERIFY(map.keys() == map.uniqueKeys());
    QVERIFY(map.values() == (QList<int>() << 1 << -2));

    map.insertMulti("alpha", 2);
    QVERIFY(map.uniqueKeys() == (QList<QString>() << "alpha" << "beta"));
    QVERIFY(map.keys() == (QList<QString>() << "alpha" << "alpha" << "beta"));
    QVERIFY(map.values() == (QList<int>() << 2 << 1 << -2));

    map.insertMulti("beta", 4);
    QVERIFY(map.uniqueKeys() == (QList<QString>() << "alpha" << "beta"));
    QVERIFY(map.keys() == (QList<QString>() << "alpha" << "alpha" << "beta" << "beta"));
    QVERIFY(map.values() == (QList<int>() << 2 << 1 << 4 << -2));
}

void tst_QMap::qmultimap_specific()
{
    QMultiMap<int, int> map1;
    for (int i = 1; i <= 9; ++i) {
        for (int j = 1; j <= i; ++j) {
            int k = i * 10 + j;
            QVERIFY(!map1.contains(i, k));
            map1.insert(i, k);
            QVERIFY(map1.contains(i, k));
        }
    }

    for (int i = 1; i <= 9; ++i) {
        for (int j = 1; j <= i; ++j) {
            int k = i * 10 + j;
            QVERIFY(map1.contains(i, k));
        }
    }

    QVERIFY(map1.contains(9, 99));
    QCOMPARE(map1.count(), 45);
    map1.remove(9, 99);
    QVERIFY(!map1.contains(9, 99));
    QCOMPARE(map1.count(), 44);

    map1.remove(9, 99);
    QVERIFY(!map1.contains(9, 99));
    QCOMPARE(map1.count(), 44);

    map1.remove(1, 99);
    QCOMPARE(map1.count(), 44);

    map1.insert(1, 99);
    map1.insert(1, 99);

    QCOMPARE(map1.count(), 46);
    map1.remove(1, 99);
    QCOMPARE(map1.count(), 44);
    map1.remove(1, 99);
    QCOMPARE(map1.count(), 44);

    {
    QMultiMap<int, int>::const_iterator i = map1.constFind(1, 11);
    QVERIFY(i.key() == 1);
    QVERIFY(i.value() == 11);

    i = map1.constFind(2, 22);
    QVERIFY(i.key() == 2);
    QVERIFY(i.value() == 22);

    i = map1.constFind(9, 98);
    QVERIFY(i.key() == 9);
    QVERIFY(i.value() == 98);
    }

    {
    const QMultiMap<int, int> map2(map1);
    QMultiMap<int, int>::const_iterator i = map2.find(1, 11);
    QVERIFY(i.key() == 1);
    QVERIFY(i.value() == 11);

    i = map2.find(2, 22);
    QVERIFY(i.key() == 2);
    QVERIFY(i.value() == 22);

    i = map2.find(9, 98);
    QVERIFY(i.key() == 9);
    QVERIFY(i.value() == 98);
    }

    {
    QMultiMap<int, int>::iterator i = map1.find(1, 11);
    QVERIFY(i.key() == 1);
    QVERIFY(i.value() == 11);

    i = map1.find(2, 22);
    QVERIFY(i.key() == 2);
    QVERIFY(i.value() == 22);

    i = map1.find(9, 98);
    QVERIFY(i.key() == 9);
    QVERIFY(i.value() == 98);
    }

    {
    QMultiMap<int, int> map1;
    map1.insert(42, 1);
    map1.insert(10, 2);
    map1.insert(48, 3);
    QMultiMap<int, int> map2;
    map2.insert(8, 4);
    map2.insert(42, 5);
    map2.insert(95, 12);

    map1+=map2;
    map2.insert(42, 1);
    map2.insert(10, 2);
    map2.insert(48, 3);
    QCOMPARE(map1.count(), map2.count());
    QVERIFY(map1.remove(42,5));
    QVERIFY(map2.remove(42,5));
    QVERIFY(map1 == map2);
    }
}

void tst_QMap::const_shared_null()
{
    QMap<int, QString> map1;
    map1.setSharable(false);
    QVERIFY(map1.isDetached());

    QMap<int, QString> map2;
    map2.setSharable(true);
    QVERIFY(!map2.isDetached());
}

void tst_QMap::equal_range()
{
    QMap<int, QString> map;

    QPair<QMap<int, QString>::iterator, QMap<int, QString>::iterator> result = map.equal_range(0);
    QCOMPARE(result.first, map.end());
    QCOMPARE(result.second, map.end());

    map.insert(1, "one");

    result = map.equal_range(0);
    QCOMPARE(result.first, map.find(1));
    QCOMPARE(result.second, map.find(1));

    result = map.equal_range(1);
    QCOMPARE(result.first, map.find(1));
    QCOMPARE(result.second, map.end());

    result = map.equal_range(2);
    QCOMPARE(result.first, map.end());
    QCOMPARE(result.second, map.end());

    for (int i = -10; i < 10; i += 2)
        map.insert(i, QString("%1").arg(i));

    result = map.equal_range(0);
    QCOMPARE(result.first, map.find(0));
    QCOMPARE(result.second, map.find(1));

    result = map.equal_range(1);
    QCOMPARE(result.first, map.find(1));
    QCOMPARE(result.second, map.find(2));

    result = map.equal_range(2);
    QCOMPARE(result.first, map.find(2));
    QCOMPARE(result.second, map.find(4));

    map.insertMulti(1, "another one");
    result = map.equal_range(1);
    QCOMPARE(result.first, map.find(1));
    QCOMPARE(result.second, map.find(2));

    QCOMPARE(map.count(1), 2);
}

template <class T>
const T &const_(const T &t)
{
    return t;
}

void tst_QMap::setSharable()
{
    QMap<int, QString> map;

    map.insert(1, "um");
    map.insert(2, "dois");
    map.insert(4, "quatro");
    map.insert(5, "cinco");

    map.setSharable(true);
    QCOMPARE(map.size(), 4);
    QCOMPARE(const_(map)[4], QString("quatro"));

    {
        QMap<int, QString> copy(map);

        QVERIFY(!map.isDetached());
        QVERIFY(copy.isSharedWith(map));
        sanityCheckTree(copy, __LINE__);
    }

    map.setSharable(false);
    sanityCheckTree(map, __LINE__);
    QVERIFY(map.isDetached());
    QCOMPARE(map.size(), 4);
    QCOMPARE(const_(map)[4], QString("quatro"));

    {
        QMap<int, QString> copy(map);

        QVERIFY(map.isDetached());
        QVERIFY(copy.isDetached());

        QCOMPARE(copy.size(), 4);
        QCOMPARE(const_(copy)[4], QString("quatro"));

        QCOMPARE(map, copy);
        sanityCheckTree(map, __LINE__);
        sanityCheckTree(copy, __LINE__);
    }

    map.setSharable(true);
    QCOMPARE(map.size(), 4);
    QCOMPARE(const_(map)[4], QString("quatro"));

    {
        QMap<int, QString> copy(map);

        QVERIFY(!map.isDetached());
        QVERIFY(copy.isSharedWith(map));
    }
}

void tst_QMap::insert()
{
    QMap<QString, float> map;
    map.insert("cs/key1", 1);
    map.insert("cs/key2", 2);
    map.insert("cs/key1", 3);
    QCOMPARE(map.count(), 2);

    QMap<int, int> intMap;
    for (int i = 0; i < 1000; ++i) {
        intMap.insert(i, i);
    }

    QCOMPARE(intMap.size(), 1000);

    for (int i = 0; i < 1000; ++i) {
        QCOMPARE(intMap.value(i), i);
        intMap.insert(i, -1);
        QCOMPARE(intMap.size(), 1000);
        QCOMPARE(intMap.value(i), -1);
    }
}

void tst_QMap::eraseValidIteratorOnSharedMap()
{
    QMap<int, int> a, b;
    a.insert(10, 10);
    a.insertMulti(10, 40);
    a.insertMulti(10, 25);
    a.insertMulti(10, 30);
    a.insert(20, 20);

    QMap<int, int>::iterator i = a.begin();
    while (i.value() != 25)
        ++i;

    b = a;
    a.erase(i);

    QCOMPARE(b.size(), 5);
    QCOMPARE(a.size(), 4);

    for (i = a.begin(); i != a.end(); ++i)
        QVERIFY(i.value() != 25);

    int itemsWith10 = 0;
    for (i = b.begin(); i != b.end(); ++i)
        itemsWith10 += (i.key() == 10);

    QCOMPARE(itemsWith10, 4);

    // Border cases
    QMap <QString, QString> ms1, ms2, ms3;
    ms1.insert("foo", "bar");
    ms1.insertMulti("foo", "quux");
    ms1.insertMulti("foo", "bar");

    QMap <QString, QString>::iterator si = ms1.begin();
    ms2 = ms1;
    ms1.erase(si);
    si = ms1.begin();
    QCOMPARE(si.value(), QString("quux"));
    ++si;
    QCOMPARE(si.value(), QString("bar"));

    si = ms2.begin();
    ++si;
    ++si;
    ms3 = ms2;
    ms2.erase(si);
    si = ms2.begin();
    QCOMPARE(si.value(), QString("bar"));
    ++si;
    QCOMPARE(si.value(), QString("quux"));

    QCOMPARE(ms1.size(), 2);
    QCOMPARE(ms2.size(), 2);
    QCOMPARE(ms3.size(), 3);
}


void tst_QMap::checkMostLeftNode()
{
    QMap<int, int> map;

    map.insert(100, 1);
    sanityCheckTree(map, __LINE__);

    // insert
    map.insert(99, 1);
    sanityCheckTree(map, __LINE__);
    map.insert(98, 1);
    sanityCheckTree(map, __LINE__);
    map.insert(97, 1);
    sanityCheckTree(map, __LINE__);
    map.insert(96, 1);
    sanityCheckTree(map, __LINE__);
    map.insert(95, 1);

    // remove
    sanityCheckTree(map, __LINE__);
    map.take(95);
    sanityCheckTree(map, __LINE__);
    map.remove(96);
    sanityCheckTree(map, __LINE__);
    map.erase(map.begin());
    sanityCheckTree(map, __LINE__);
    map.remove(97);
    sanityCheckTree(map, __LINE__);
    map.remove(98);
    sanityCheckTree(map, __LINE__);
    map.remove(99);
    sanityCheckTree(map, __LINE__);
    map.remove(100);
    sanityCheckTree(map, __LINE__);
    map.insert(200, 1);
    QCOMPARE(map.constBegin().key(), 200);
    sanityCheckTree(map, __LINE__);
    // remove the non left most node
    map.insert(202, 2);
    map.insert(203, 3);
    map.insert(204, 4);
    map.remove(202);
    sanityCheckTree(map, __LINE__);
    map.remove(203);
    sanityCheckTree(map, __LINE__);
    map.remove(204);
    sanityCheckTree(map, __LINE__);
    // erase last item
    map.erase(map.begin());
    sanityCheckTree(map, __LINE__);
}

QTEST_APPLESS_MAIN(tst_QMap)
#include "tst_qmap.moc"
