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


#include <QtCore>
#include <QtTest/QtTest>

#ifdef Q_OS_LINUX
# include <pthread.h>
#endif

Q_DECLARE_METATYPE(QMetaType::Type)

class tst_QMetaType: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> prop READ prop WRITE setProp)

public:
    tst_QMetaType() { propList << 42 << "Hello"; }

    QList<QVariant> prop() const { return propList; }
    void setProp(const QList<QVariant> &list) { propList = list; }

private:
    QList<QVariant> propList;

private slots:
    void defined();
    void threadSafety();
    void namespaces();
    void qMetaTypeId();
    void properties();
    void normalizedTypes();
    void typeName_data();
    void typeName();
    void construct();
    void typedefs();
    void isRegistered_data();
    void isRegistered();
    void unregisterType();
    void registerStreamBuiltin();
    void automaticTemplateRegistration();

};

struct Foo { int i; };

void tst_QMetaType::defined()
{
    QCOMPARE(int(QMetaTypeId2<QString>::Defined), 1);
    QCOMPARE(int(QMetaTypeId2<Foo>::Defined), 0);
    QCOMPARE(int(QMetaTypeId2<void*>::Defined), 1);
    QCOMPARE(int(QMetaTypeId2<int*>::Defined), 0);
}

struct Bar
{
    Bar()
    {
        // check re-entrancy
        if (!QMetaType::isRegistered(qRegisterMetaType<Foo>("Foo"))) {
            qWarning("%s: re-entrancy test failed", Q_FUNC_INFO);
            ++failureCount;
        }
    }
    ~Bar() {}

public:
    static int failureCount;
};

int Bar::failureCount = 0;

class MetaTypeTorturer: public QThread
{
    Q_OBJECT
protected:
    void run()
    {
        for (int i = 0; i < 1000; ++i) {
            const QByteArray name = QString("Bar%1_%2").arg(i).arg((size_t)QThread::currentThreadId()).toLatin1();
            const char *nm = name.constData();
            int tp = qRegisterMetaType<Bar>(nm);
#ifdef Q_OS_LINUX
            pthread_yield();
#endif
            if (!QMetaType::isRegistered(tp)) {
                ++failureCount;
                qWarning() << name << "is not a registered metatype";
            }
            if (QMetaType::type(nm) != tp) {
                ++failureCount;
                qWarning() << "Wrong metatype returned for" << name;
            }
            if (QMetaType::typeName(tp) != name) {
                ++failureCount;
                qWarning() << "Wrong typeName returned for" << tp;
            }
            void *buf = QMetaType::construct(tp, 0);
            void *buf2 = QMetaType::construct(tp, buf);
            if (!buf) {
                ++failureCount;
                qWarning() << "Null buffer returned by QMetaType::construct(tp, 0)";
            }
            if (!buf2) {
                ++failureCount;
                qWarning() << "Null buffer returned by QMetaType::construct(tp, buf)";
            }
            QMetaType::destroy(tp, buf);
            QMetaType::destroy(tp, buf2);
        }
    }
public:
    MetaTypeTorturer() : failureCount(0) { }
    int failureCount;
};

void tst_QMetaType::threadSafety()
{
    MetaTypeTorturer t1;
    MetaTypeTorturer t2;
    MetaTypeTorturer t3;

    t1.start();
    t2.start();
    t3.start();

    QVERIFY(t1.wait());
    QVERIFY(t2.wait());
    QVERIFY(t3.wait());

    QCOMPARE(t1.failureCount, 0);
    QCOMPARE(t2.failureCount, 0);
    QCOMPARE(t3.failureCount, 0);
    QCOMPARE(Bar::failureCount, 0);
}

namespace TestSpace
{
    struct Foo { double d; public: ~Foo() {} };

}
Q_DECLARE_METATYPE(TestSpace::Foo)

void tst_QMetaType::namespaces()
{
    TestSpace::Foo nf = { 11.12 };
    QVariant v = QVariant::fromValue(nf);
    QCOMPARE(qvariant_cast<TestSpace::Foo>(v).d, 11.12);
}

void tst_QMetaType::qMetaTypeId()
{
    QCOMPARE(::qMetaTypeId<QString>(), int(QMetaType::QString));
    QCOMPARE(::qMetaTypeId<int>(), int(QMetaType::Int));
    QCOMPARE(::qMetaTypeId<TestSpace::Foo>(), QMetaType::type("TestSpace::Foo"));

    QCOMPARE(::qMetaTypeId<char>(), QMetaType::type("char"));
    QCOMPARE(::qMetaTypeId<uchar>(), QMetaType::type("unsigned char"));
    QCOMPARE(::qMetaTypeId<signed char>(), QMetaType::type("signed char"));
    QCOMPARE(::qMetaTypeId<qint8>(), QMetaType::type("qint8"));
}

void tst_QMetaType::properties()
{
    qRegisterMetaType<QList<QVariant> >("QList<QVariant>");

    QVariant v = property("prop");

    QCOMPARE(v.typeName(), "QVariantList");

    QList<QVariant> values = v.toList();
    QCOMPARE(values.count(), 2);
    QCOMPARE(values.at(0).toInt(), 42);

    values << 43 << "world";

    QVERIFY(setProperty("prop", values));
    v = property("prop");
    QCOMPARE(v.toList().count(), 4);
}

template <typename T>
struct Whity { T t; Whity() {} };

Q_DECLARE_METATYPE( Whity < int > )
Q_DECLARE_METATYPE(Whity<double>)

#if !defined(Q_CC_CLANG) && defined(Q_CC_GNU) && Q_CC_GNU < 501
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(Whity<double>, Q_MOVABLE_TYPE);
QT_END_NAMESPACE
#endif

void tst_QMetaType::normalizedTypes()
{
    int WhityIntId = ::qMetaTypeId<Whity<int> >();
    int WhityDoubleId = ::qMetaTypeId<Whity<double> >();

    QCOMPARE(QMetaType::type("Whity<int>"), WhityIntId);
    QCOMPARE(QMetaType::type(" Whity < int > "), WhityIntId);
    QCOMPARE(QMetaType::type("Whity<int >"), WhityIntId);

    QCOMPARE(QMetaType::type("Whity<double>"), WhityDoubleId);
    QCOMPARE(QMetaType::type(" Whity< double > "), WhityDoubleId);
    QCOMPARE(QMetaType::type("Whity<double >"), WhityDoubleId);

    QCOMPARE(qRegisterMetaType<Whity<int> >(" Whity < int > "), WhityIntId);
    QCOMPARE(qRegisterMetaType<Whity<int> >("Whity<int>"), WhityIntId);
    QCOMPARE(qRegisterMetaType<Whity<int> >("Whity<int > "), WhityIntId);

    QCOMPARE(qRegisterMetaType<Whity<double> >(" Whity < double > "), WhityDoubleId);
    QCOMPARE(qRegisterMetaType<Whity<double> >("Whity<double>"), WhityDoubleId);
    QCOMPARE(qRegisterMetaType<Whity<double> >("Whity<double > "), WhityDoubleId);
}

#define TYPENAME_DATA(MetaTypeName, MetaTypeId, RealType)\
    QTest::newRow(#RealType) << QMetaType::MetaTypeName << #RealType;

#define TYPENAME_DATA_ALIAS(MetaTypeName, MetaTypeId, AliasType, RealTypeString)\
    QTest::newRow(RealTypeString) << QMetaType::MetaTypeName << #AliasType;


void tst_QMetaType::typeName_data()
{
    QTest::addColumn<QMetaType::Type>("aType");
    QTest::addColumn<QString>("aTypeName");

    QT_FOR_EACH_STATIC_TYPE(TYPENAME_DATA)
    QT_FOR_EACH_STATIC_ALIAS_TYPE(TYPENAME_DATA_ALIAS)
}

void tst_QMetaType::typeName()
{
    QFETCH(QMetaType::Type, aType);
    QFETCH(QString, aTypeName);

    QCOMPARE(QString::fromLatin1(QMetaType::typeName(aType)), aTypeName);
}

#define FOR_EACH_PRIMITIVE_METATYPE(F) \
    QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(F) \
    QT_FOR_EACH_STATIC_CORE_POINTER(F) \

#define FOR_EACH_COMPLEX_CORE_METATYPE(F) \
    QT_FOR_EACH_STATIC_CORE_CLASS(F)

#define FOR_EACH_CORE_METATYPE(F) \
    FOR_EACH_PRIMITIVE_METATYPE(F) \
    FOR_EACH_COMPLEX_CORE_METATYPE(F) \

class C { char _[4]; };
class M { char _[4]; };
class P { char _[4]; };

QT_BEGIN_NAMESPACE
#if defined(Q_CC_GNU) && Q_CC_GNU < 501
Q_DECLARE_TYPEINFO(M, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(P, Q_PRIMITIVE_TYPE);
#endif
QT_END_NAMESPACE

// avoid the comma:
typedef QPair<C,C> QPairCC;
typedef QPair<C,M> QPairCM;
typedef QPair<C,P> QPairCP;
typedef QPair<M,C> QPairMC;
typedef QPair<M,M> QPairMM;
typedef QPair<M,P> QPairMP;
typedef QPair<P,C> QPairPC;
typedef QPair<P,M> QPairPM;
typedef QPair<P,P> QPairPP;

Q_DECLARE_METATYPE(QPairCC)
Q_DECLARE_METATYPE(QPairCM)
Q_DECLARE_METATYPE(QPairCP)
Q_DECLARE_METATYPE(QPairMC)
Q_DECLARE_METATYPE(QPairMM)
Q_DECLARE_METATYPE(QPairMP)
Q_DECLARE_METATYPE(QPairPC)
Q_DECLARE_METATYPE(QPairPM)
Q_DECLARE_METATYPE(QPairPP)

#if 0
void tst_QMetaType::flags_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<bool>("isMovable");
    QTest::addColumn<bool>("isComplex");
    QTest::addColumn<bool>("isPointerToQObject");

#define ADD_METATYPE_TEST_ROW(MetaTypeName, MetaTypeId, RealType) \
    QTest::newRow(QMetaType::typeName(QMetaType::MetaTypeName)) << QMetaType::MetaTypeName;
QT_FOR_EACH_STATIC_CORE_CLASS(ADD_METATYPE_TEST_ROW)
QT_FOR_EACH_STATIC_PRIMITIVE_POINTER(ADD_METATYPE_TEST_ROW)
QT_FOR_EACH_STATIC_CORE_POINTER(ADD_METATYPE_TEST_ROW)
#undef ADD_METATYPE_TEST_ROW
    QTest::newRow("TestSpace::Foo") << ::qMetaTypeId<TestSpace::Foo>() << false << true << false;
    QTest::newRow("Whity<double>") << ::qMetaTypeId<Whity<double> >() << true << true << false;
    //QTest::newRow("CustomMovable") << ::qMetaTypeId<CustomMovable>() << true << true << false;
    //QTest::newRow("CustomObject*") << ::qMetaTypeId<CustomObject*>() << true << false << true;
    //QTest::newRow("CustomMultiInheritanceObject*") << ::qMetaTypeId<CustomMultiInheritanceObject*>() << true << false << true;
    QTest::newRow("QPair<C,C>") << ::qMetaTypeId<QPair<C,C> >() << false << true  << false;
    QTest::newRow("QPair<C,M>") << ::qMetaTypeId<QPair<C,M> >() << false << true  << false;
    QTest::newRow("QPair<C,P>") << ::qMetaTypeId<QPair<C,P> >() << false << true  << false;
    QTest::newRow("QPair<M,C>") << ::qMetaTypeId<QPair<M,C> >() << false << true  << false;
    QTest::newRow("QPair<M,M>") << ::qMetaTypeId<QPair<M,M> >() << true  << true  << false;
    QTest::newRow("QPair<M,P>") << ::qMetaTypeId<QPair<M,P> >() << true  << true  << false;
    QTest::newRow("QPair<P,C>") << ::qMetaTypeId<QPair<P,C> >() << false << true  << false;
    QTest::newRow("QPair<P,M>") << ::qMetaTypeId<QPair<P,M> >() << true  << true  << false;
    QTest::newRow("QPair<P,P>") << ::qMetaTypeId<QPair<P,P> >() << true  << false << false;
    QTest::newRow("FlagsDataEnum") << ::qMetaTypeId<FlagsDataEnum>() << true << false << false << true;
}

void tst_QMetaType::flags()
{
    QFETCH(int, type);
    QFETCH(bool, isMovable);
    QFETCH(bool, isComplex);
    QFETCH(bool, isPointerToQObject);

    QCOMPARE(bool(QMetaType::typeFlags(type) & QMetaType::NeedsConstruction), isComplex);
    QCOMPARE(bool(QMetaType::typeFlags(type) & QMetaType::NeedsDestruction), isComplex);
    QCOMPARE(bool(QMetaType::typeFlags(type) & QMetaType::MovableType), isMovable);
    QCOMPARE(bool(QMetaType::typeFlags(type) & QMetaType::PointerToQObject), isPointerToQObject);
}
#endif

void tst_QMetaType::construct()
{
    QSize x(1, 1);
    void *size = QMetaType::construct(QMetaType::QSize, &x);
    QVERIFY(size);
    QCOMPARE(static_cast<QSize *>(size)->width(), 1);
    QMetaType::destroy(QMetaType::QSize, size);
}

typedef QString CustomString;
Q_DECLARE_METATYPE(CustomString) //this line is useless

void tst_QMetaType::typedefs()
{
    QCOMPARE(QMetaType::type("long long"), int(QMetaType::LongLong));
    QCOMPARE(QMetaType::type("unsigned long long"), int(QMetaType::ULongLong));
    QCOMPARE(QMetaType::type("qint8"), int(QMetaType::Char));
    QCOMPARE(QMetaType::type("quint8"), int(QMetaType::UChar));
    QCOMPARE(QMetaType::type("qint16"), int(QMetaType::Short));
    QCOMPARE(QMetaType::type("quint16"), int(QMetaType::UShort));
    QCOMPARE(QMetaType::type("qint32"), int(QMetaType::Int));
    QCOMPARE(QMetaType::type("quint32"), int(QMetaType::UInt));
    QCOMPARE(QMetaType::type("qint64"), int(QMetaType::LongLong));
    QCOMPARE(QMetaType::type("quint64"), int(QMetaType::ULongLong));

    // make sure the qreal typeId is the type id of the type it's defined to
    QCOMPARE(QMetaType::type("qreal"), ::qMetaTypeId<qreal>());

    qRegisterMetaType<CustomString>("CustomString");
    QCOMPARE(QMetaType::type("CustomString"), ::qMetaTypeId<CustomString>());

    typedef Whity<double> WhityDouble;
    qRegisterMetaType<WhityDouble>("WhityDouble");
    QCOMPARE(QMetaType::type("WhityDouble"), ::qMetaTypeId<WhityDouble>());
}

class IsRegisteredDummyType { };

void tst_QMetaType::isRegistered_data()
{
    QTest::addColumn<int>("typeId");
    QTest::addColumn<bool>("registered");

    // predefined/custom types
    QTest::newRow("QMetaType::Void") << int(QMetaType::Void) << true;
    QTest::newRow("QMetaType::Int") << int(QMetaType::Int) << true;

    int dummyTypeId = qRegisterMetaType<IsRegisteredDummyType>("IsRegisteredDummyType");

    QTest::newRow("IsRegisteredDummyType") << dummyTypeId << true;

    // unknown types
    QTest::newRow("-1") << -1 << false;
    QTest::newRow("-42") << -42 << false;
    QTest::newRow("IsRegisteredDummyType + 1") << (dummyTypeId + 1) << false;
}

void tst_QMetaType::isRegistered()
{
    QFETCH(int, typeId);
    QFETCH(bool, registered);
    QCOMPARE(QMetaType::isRegistered(typeId), registered);
}

class RegUnreg
{
public:
    RegUnreg() {};
    RegUnreg(const RegUnreg &) {};
    ~RegUnreg() {};
};

void tst_QMetaType::unregisterType()
{
    // cannot unregister standard types
    int typeId = qRegisterMetaType<QList<QVariant> >("QList<QVariant>");
    QCOMPARE(QMetaType::isRegistered(typeId), true);
    QMetaType::unregisterType("QList<QVariant>");
    QCOMPARE(QMetaType::isRegistered(typeId), true);
    // allow unregister user types
    typeId = qRegisterMetaType<RegUnreg>("RegUnreg");
    QCOMPARE(QMetaType::isRegistered(typeId), true);
    QMetaType::unregisterType("RegUnreg");
    QCOMPARE(QMetaType::isRegistered(typeId), false);
}

void tst_QMetaType::registerStreamBuiltin()
{
    //should not crash;
    qRegisterMetaTypeStreamOperators<QString>("QString");
    qRegisterMetaTypeStreamOperators<QVariant>("QVariant");
}

Q_DECLARE_METATYPE(QSharedPointer<QObject>)

void tst_QMetaType::automaticTemplateRegistration()
{
  {
    QList<int> intList;
    intList << 42;
    QVERIFY(QVariant::fromValue(intList).value<QList<int> >().first() == 42);
    QVector<QList<int> > vectorList;
    vectorList << intList;
    QVERIFY(QVariant::fromValue(vectorList).value<QVector<QList<int> > >().first().first() == 42);
  }

  {
    QList<QByteArray> bytearrayList;
    bytearrayList << QByteArray("foo");
    QVERIFY(QVariant::fromValue(bytearrayList).value<QList<QByteArray> >().first() == QByteArray("foo"));
    QVector<QList<QByteArray> > vectorList;
    vectorList << bytearrayList;
    QVERIFY(QVariant::fromValue(vectorList).value<QVector<QList<QByteArray> > >().first().first() == QByteArray("foo"));
  }

  QCOMPARE(::qMetaTypeId<QVariantList>(), (int)QMetaType::QVariantList);
  QCOMPARE(::qMetaTypeId<QList<QVariant> >(), (int)QMetaType::QVariantList);

  {
    QList<QVariant> variantList;
    variantList << 42;
    QVERIFY(QVariant::fromValue(variantList).value<QList<QVariant> >().first() == 42);
    QVector<QList<QVariant> > vectorList;
    vectorList << variantList;
    QVERIFY(QVariant::fromValue(vectorList).value<QVector<QList<QVariant> > >().first().first() == 42);
  }

  {
    QList<QSharedPointer<QObject> > sharedPointerList;
    QObject *testObject = new QObject;
    sharedPointerList << QSharedPointer<QObject>(testObject);
    QVERIFY(QVariant::fromValue(sharedPointerList).value<QList<QSharedPointer<QObject> > >().first() == testObject);
    QVector<QList<QSharedPointer<QObject> > > vectorList;
    vectorList << sharedPointerList;
    QVERIFY(QVariant::fromValue(vectorList).value<QVector<QList<QSharedPointer<QObject> > > >().first().first() == testObject);
  }
}

QTEST_MAIN(tst_QMetaType)
#include "tst_qmetatype.moc"
