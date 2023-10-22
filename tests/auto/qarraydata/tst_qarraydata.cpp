/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtCore/QString>
#include <QtCore/qarraydata.h>

#include "simplevector.h"

class tst_QArrayData : public QObject
{
    Q_OBJECT

private slots:
    void referenceCounting();
    void sharedNullEmpty();
    void staticData();
    void simpleVector();
    void allocate_data();
    void allocate();
    void alignment_data();
    void alignment();
    void typedData();
    void arrayOps();
    void setSharable_data();
    void setSharable();
    void fromRawData();
};

template <class T> const T &const_(const T &t) { return t; }

void tst_QArrayData::referenceCounting()
{
    {
        // Reference counting initialized to 1 (owned)
        QArrayData array = { { Q_BASIC_ATOMIC_INITIALIZER(1) }, 0, 0, 0, 0 };

        QCOMPARE(int(array.ref), 1);

        QVERIFY(!array.ref.isStatic());
        QVERIFY(array.ref.isSharable());

        QVERIFY(array.ref.ref());
        QCOMPARE(int(array.ref), 2);

        QVERIFY(array.ref.deref());
        QCOMPARE(int(array.ref), 1);

        QVERIFY(array.ref.ref());
        QCOMPARE(int(array.ref), 2);

        QVERIFY(array.ref.deref());
        QCOMPARE(int(array.ref), 1);

        QVERIFY(!array.ref.deref());
        QCOMPARE(int(array.ref), 0);

        // Now would be a good time to free/release allocated data
    }

    {
        // Reference counting initialized to 0 (non-sharable)
        QArrayData array = { { Q_BASIC_ATOMIC_INITIALIZER(0) }, 0, 0, 0, 0 };

        QCOMPARE(int(array.ref), 0);

        QVERIFY(!array.ref.isStatic());
        QVERIFY(!array.ref.isSharable());

        QVERIFY(!array.ref.ref());
        // Reference counting fails, data should be copied
        QCOMPARE(int(array.ref), 0);

        QVERIFY(!array.ref.deref());
        QCOMPARE(int(array.ref), 0);

        // Free/release data
    }

    {
        // Reference counting initialized to -1 (static read-only data)
        QArrayData array = { Q_REFCOUNT_INITIALIZE_STATIC, 0, 0, 0, 0 };

        QCOMPARE(int(array.ref), -1);

        QVERIFY(array.ref.isStatic());
        QVERIFY(array.ref.isSharable());

        QVERIFY(array.ref.ref());
        QCOMPARE(int(array.ref), -1);

        QVERIFY(array.ref.deref());
        QCOMPARE(int(array.ref), -1);
    }
}

void tst_QArrayData::sharedNullEmpty()
{
    QArrayData *null = const_cast<QArrayData *>(&QArrayData::shared_null);
    QArrayData *empty = QArrayData::allocate(1, Q_ALIGNOF(QArrayData), 0);

    QVERIFY(null->ref.isStatic());
    QVERIFY(null->ref.isSharable());
    QVERIFY(null->ref.isShared());

    QVERIFY(empty->ref.isStatic());
    QVERIFY(empty->ref.isSharable());
    QVERIFY(empty->ref.isShared());

    QCOMPARE(int(null->ref), -1);
    QCOMPARE(int(empty->ref), -1);

    QVERIFY(null->ref.ref());
    QVERIFY(empty->ref.ref());

    QCOMPARE(int(null->ref), -1);
    QCOMPARE(int(empty->ref), -1);

    QVERIFY(null->ref.deref());
    QVERIFY(empty->ref.deref());

    QCOMPARE(int(null->ref), -1);
    QCOMPARE(int(empty->ref), -1);

    QVERIFY(null != empty);

    QCOMPARE(null->size, 0);
    QCOMPARE(null->alloc, 0u);
    QCOMPARE(null->capacityReserved, 0u);

    QCOMPARE(empty->size, 0);
    QCOMPARE(empty->alloc, 0u);
    QCOMPARE(empty->capacityReserved, 0u);
}

void tst_QArrayData::staticData()
{
    QStaticArrayData<char, 10> charArray = {
        Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(char, 10),
        { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' }
    };
    QStaticArrayData<int, 10> intArray = {
        Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(int, 10),
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
    };
    QStaticArrayData<double, 10> doubleArray = {
        Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(double, 10),
        { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f }
    };

    QCOMPARE(charArray.header.size, 10);
    QCOMPARE(intArray.header.size, 10);
    QCOMPARE(doubleArray.header.size, 10);

    QCOMPARE(charArray.header.data(), reinterpret_cast<void *>(&charArray.data));
    QCOMPARE(intArray.header.data(), reinterpret_cast<void *>(&intArray.data));
    QCOMPARE(doubleArray.header.data(), reinterpret_cast<void *>(&doubleArray.data));
}

void tst_QArrayData::simpleVector()
{
    QArrayData data0 = { Q_REFCOUNT_INITIALIZE_STATIC, 0, 0, 0, 0 };
    QStaticArrayData<int, 7> data1 = {
            Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(int, 7),
            { 0, 1, 2, 3, 4, 5, 6 }
        };

    int array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    SimpleVector<int> v1;
    SimpleVector<int> v2(v1);
    SimpleVector<int> v3(static_cast<QTypedArrayData<int> *>(&data0));
    SimpleVector<int> v4(static_cast<QTypedArrayData<int> *>(&data1.header));
    SimpleVector<int> v5(static_cast<QTypedArrayData<int> *>(&data0));
    SimpleVector<int> v6(static_cast<QTypedArrayData<int> *>(&data1.header));
    SimpleVector<int> v7(10, 5);
    SimpleVector<int> v8(array, array + sizeof(array)/sizeof(*array));

    v3 = v1;
    v1.swap(v3);
    v4.clear();

    QVERIFY(v1.isNull());
    QVERIFY(v2.isNull());
    QVERIFY(v3.isNull());
    QVERIFY(!v4.isNull());
    QVERIFY(!v5.isNull());
    QVERIFY(!v6.isNull());
    QVERIFY(!v7.isNull());
    QVERIFY(!v8.isNull());

    QVERIFY(v1.isEmpty());
    QVERIFY(v2.isEmpty());
    QVERIFY(v3.isEmpty());
    QVERIFY(v4.isEmpty());
    QVERIFY(v5.isEmpty());
    QVERIFY(!v6.isEmpty());
    QVERIFY(!v7.isEmpty());
    QVERIFY(!v8.isEmpty());

    QCOMPARE(v1.size(), size_t(0));
    QCOMPARE(v2.size(), size_t(0));
    QCOMPARE(v3.size(), size_t(0));
    QCOMPARE(v4.size(), size_t(0));
    QCOMPARE(v5.size(), size_t(0));
    QCOMPARE(v6.size(), size_t(7));
    QCOMPARE(v7.size(), size_t(10));
    QCOMPARE(v8.size(), size_t(10));

    QCOMPARE(v1.capacity(), size_t(0));
    QCOMPARE(v2.capacity(), size_t(0));
    QCOMPARE(v3.capacity(), size_t(0));
    QCOMPARE(v4.capacity(), size_t(0));
    QCOMPARE(v5.capacity(), size_t(0));
    // v6.capacity() is unspecified, for now
    QVERIFY(v7.capacity() >= size_t(10));
    QVERIFY(v8.capacity() >= size_t(10));

    QVERIFY(v1.isStatic());
    QVERIFY(v2.isStatic());
    QVERIFY(v3.isStatic());
    QVERIFY(v4.isStatic());
    QVERIFY(v5.isStatic());
    QVERIFY(v6.isStatic());
    QVERIFY(!v7.isStatic());
    QVERIFY(!v8.isStatic());

    QVERIFY(v1.isShared());
    QVERIFY(v2.isShared());
    QVERIFY(v3.isShared());
    QVERIFY(v4.isShared());
    QVERIFY(v5.isShared());
    QVERIFY(v6.isShared());
    QVERIFY(!v7.isShared());
    QVERIFY((SimpleVector<int>(v7), v7.isShared()));
    QVERIFY(!v7.isShared());
    QVERIFY(!v8.isShared());

    QVERIFY(v1.isSharedWith(v2));
    QVERIFY(v1.isSharedWith(v3));
    QVERIFY(!v1.isSharedWith(v4));
    QVERIFY(!v1.isSharedWith(v5));
    QVERIFY(!v1.isSharedWith(v6));

    QVERIFY(v1.constBegin() == v1.constEnd());
    QVERIFY(v4.constBegin() == v4.constEnd());
    QVERIFY(v6.constBegin() + v6.size() == v6.constEnd());
    QVERIFY(v7.constBegin() + v7.size() == v7.constEnd());
    QVERIFY(v8.constBegin() + v8.size() == v8.constEnd());

    QVERIFY(v1 == v2);
    QVERIFY(v1 == v3);
    QVERIFY(v1 == v4);
    QVERIFY(v1 == v5);
    QVERIFY(!(v1 == v6));

    QVERIFY(v1 != v6);
    QVERIFY(v4 != v6);
    QVERIFY(v5 != v6);
    QVERIFY(!(v1 != v5));

    QVERIFY(v1 < v6);
    QVERIFY(!(v6 < v1));
    QVERIFY(v6 > v1);
    QVERIFY(!(v1 > v6));
    QVERIFY(v1 <= v6);
    QVERIFY(!(v6 <= v1));
    QVERIFY(v6 >= v1);
    QVERIFY(!(v1 >= v6));

    {
        SimpleVector<int> temp(v6);

        QCOMPARE(const_(v6).front(), 0);
        QCOMPARE(const_(v6).back(), 6);

        QVERIFY(temp.isShared());
        QVERIFY(temp.isSharedWith(v6));

        QCOMPARE(temp.front(), 0);
        QCOMPARE(temp.back(), 6);

        // Detached
        QVERIFY(!temp.isShared());
        const int *const tempBegin = temp.begin();

        for (size_t i = 0; i < v6.size(); ++i) {
            QCOMPARE(const_(v6)[i], int(i));
            QCOMPARE(const_(v6).at(i), int(i));
            QCOMPARE(&const_(v6)[i], &const_(v6).at(i));

            QCOMPARE(const_(v8)[i], const_(v6)[i]);

            QCOMPARE(temp[i], int(i));
            QCOMPARE(temp.at(i), int(i));
            QCOMPARE(&temp[i], &temp.at(i));
        }

        // A single detach should do
        QCOMPARE(temp.begin(), tempBegin);;
    }

    {
        int count = 0;
        Q_FOREACH (int value, v7) {
            QCOMPARE(value, 5);
            ++count;
        }

        QCOMPARE(count, 10);
    }

    {
        int count = 0;
        Q_FOREACH (int value, v8) {
            QCOMPARE(value, count);
            ++count;
        }

        QCOMPARE(count, 10);
    }

    v5 = v6;
    QVERIFY(v5.isSharedWith(v6));
    QVERIFY(!v1.isSharedWith(v5));

    v1.swap(v6);
    QVERIFY(v6.isNull());
    QVERIFY(v1.isSharedWith(v5));

    {
        using std::swap;
        swap(v1, v6);
        QVERIFY(v5.isSharedWith(v6));
        QVERIFY(!v1.isSharedWith(v5));
    }

    v1.prepend(array, array + sizeof(array)/sizeof(array[0]));
    QCOMPARE(v1.size(), size_t(10));
    QVERIFY(v1 == v8);

    v6 = v1;
    QVERIFY(v1.isSharedWith(v6));

    v1.prepend(array, array + sizeof(array)/sizeof(array[0]));
    QVERIFY(!v1.isSharedWith(v6));
    QCOMPARE(v1.size(), size_t(20));
    QCOMPARE(v6.size(), size_t(10));

    for (int i = 0; i < 20; ++i)
        QCOMPARE(v1[i], v6[i % 10]);

    v1.clear();

    v1.append(array, array + sizeof(array)/sizeof(array[0]));
    QCOMPARE(v1.size(), size_t(10));
    QVERIFY(v1 == v8);

    v6 = v1;
    QVERIFY(v1.isSharedWith(v6));

    v1.append(array, array + sizeof(array)/sizeof(array[0]));
    QVERIFY(!v1.isSharedWith(v6));
    QCOMPARE(v1.size(), size_t(20));
    QCOMPARE(v6.size(), size_t(10));

    for (int i = 0; i < 20; ++i)
        QCOMPARE(v1[i], v6[i % 10]);

    v1.insert(0, v6.constBegin(), v6.constEnd());
    QCOMPARE(v1.size(), size_t(30));

    v6 = v1;
    QVERIFY(v1.isSharedWith(v6));

    v1.insert(10, v6.constBegin(), v6.constEnd());
    QVERIFY(!v1.isSharedWith(v6));
    QCOMPARE(v1.size(), size_t(60));
    QCOMPARE(v6.size(), size_t(30));

    for (int i = 0; i < 30; ++i)
        QCOMPARE(v6[i], v8[i % 10]);

    v1.insert(v1.size(), v6.constBegin(), v6.constEnd());
    QCOMPARE(v1.size(), size_t(90));

    v1.insert(-1, v8.constBegin(), v8.constEnd());
    QCOMPARE(v1.size(), size_t(100));

    v1.insert(-11, v8.constBegin(), v8.constEnd());
    QCOMPARE(v1.size(), size_t(110));

    v1.insert(-200, v8.constBegin(), v8.constEnd());
    QCOMPARE(v1.size(), size_t(120));

    for (int i = 0; i < 120; ++i)
        QCOMPARE(v1[i], v8[i % 10]);
}

struct Deallocator
{
    Deallocator(size_t objectSize, size_t alignment)
        : objectSize(objectSize)
        , alignment(alignment)
    {
    }

    ~Deallocator()
    {
        Q_FOREACH (QArrayData *data, headers)
            QArrayData::deallocate(data, objectSize, alignment);
    }

    size_t objectSize;
    size_t alignment;
    QVector<QArrayData *> headers;
};

Q_DECLARE_METATYPE(const QArrayData *)
Q_DECLARE_METATYPE(QArrayData::AllocateOptions)

void tst_QArrayData::allocate_data()
{
    QTest::addColumn<size_t>("objectSize");
    QTest::addColumn<size_t>("alignment");
    QTest::addColumn<QArrayData::AllocateOptions>("allocateOptions");
    QTest::addColumn<bool>("isCapacityReserved");
    QTest::addColumn<bool>("isSharable");
    QTest::addColumn<const QArrayData *>("commonEmpty");

    struct {
        char const *typeName;
        size_t objectSize;
        size_t alignment;
    } types[] = {
        { "char", sizeof(char), Q_ALIGNOF(char) },
        { "short", sizeof(short), Q_ALIGNOF(short) },
        { "void *", sizeof(void *), Q_ALIGNOF(void *) }
    };

    QArrayData *shared_empty = QArrayData::allocate(0, Q_ALIGNOF(QArrayData), 0);
    QArrayData *unsharable_empty = QArrayData::allocate(0, Q_ALIGNOF(QArrayData), 0, QArrayData::Unsharable);

    QVERIFY(shared_empty);
    QVERIFY(unsharable_empty);

    struct {
        char const *description;
        QArrayData::AllocateOptions allocateOptions;
        bool isCapacityReserved;
        bool isSharable;
        const QArrayData *commonEmpty;
    } options[] = {
        { "Default", QArrayData::Default, false, true, shared_empty },
        { "Reserved", QArrayData::CapacityReserved, true, true, shared_empty },
        { "Reserved | Unsharable",
            QArrayData::CapacityReserved | QArrayData::Unsharable, true, false,
            unsharable_empty },
        { "Unsharable", QArrayData::Unsharable, false, false, unsharable_empty },
    };

    for (size_t i = 0; i < sizeof(types)/sizeof(types[0]); ++i)
        for (size_t j = 0; j < sizeof(options)/sizeof(options[0]); ++j)
            QTest::newRow(qPrintable(
                        QLatin1String(types[i].typeName)
                        + QLatin1String(": ")
                        + QLatin1String(options[j].description)))
                << types[i].objectSize << types[i].alignment
                << options[j].allocateOptions << options[j].isCapacityReserved
                << options[j].isSharable << options[j].commonEmpty;
}

void tst_QArrayData::allocate()
{
    QFETCH(size_t, objectSize);
    QFETCH(size_t, alignment);
    QFETCH(QArrayData::AllocateOptions, allocateOptions);
    QFETCH(bool, isCapacityReserved);
    QFETCH(bool, isSharable);
    QFETCH(const QArrayData *, commonEmpty);

    // Minimum alignment that can be requested is that of QArrayData.
    // Typically, this alignment is sizeof(void *) and ensured by malloc.
    size_t minAlignment = qMax(alignment, Q_ALIGNOF(QArrayData));

    // Shared Empty
    QCOMPARE(QArrayData::allocate(objectSize, minAlignment, 0,
                QArrayData::AllocateOptions(allocateOptions)), commonEmpty);

    Deallocator keeper(objectSize, minAlignment);
    keeper.headers.reserve(1024);

    for (int capacity = 1; capacity <= 1024; capacity <<= 1) {
        QArrayData *data = QArrayData::allocate(objectSize, minAlignment,
                capacity, QArrayData::AllocateOptions(allocateOptions));
        keeper.headers.append(data);

        QCOMPARE(data->size, 0);
        QVERIFY(data->alloc >= uint(capacity));
        QCOMPARE(data->capacityReserved, uint(isCapacityReserved));
        QCOMPARE(data->ref.isSharable(), isSharable);

        // Check that the allocated array can be used. Best tested with a
        // memory checker, such as valgrind, running.
        ::memset(data->data(), 'A', objectSize * capacity);
    }
}

class Unaligned
{
    char dummy[8];
};

void tst_QArrayData::alignment_data()
{
    QTest::addColumn<size_t>("alignment");

    for (int i = 1; i < 10; ++i) {
        size_t alignment = 1u << i;
        QTest::newRow(qPrintable(QString::number(alignment))) << alignment;
    }
}

void tst_QArrayData::alignment()
{
    QFETCH(size_t, alignment);

    // Minimum alignment that can be requested is that of QArrayData.
    // Typically, this alignment is sizeof(void *) and ensured by malloc.
    size_t minAlignment = qMax(alignment, Q_ALIGNOF(QArrayData));

    Deallocator keeper(sizeof(Unaligned), minAlignment);
    keeper.headers.reserve(100);

    for (int i = 0; i < 100; ++i) {
        QArrayData *data = QArrayData::allocate(sizeof(Unaligned),
                minAlignment, 8, QArrayData::Default);
        keeper.headers.append(data);

        QVERIFY(data);
        QCOMPARE(data->size, 0);
        QVERIFY(data->alloc >= uint(8));

        // These conditions should hold as long as header and array are
        // allocated together
        QVERIFY(data->offset >= qptrdiff(sizeof(QArrayData)));
        QVERIFY(data->offset <= qptrdiff(sizeof(QArrayData)
                    + minAlignment - Q_ALIGNOF(QArrayData)));

        // Data is aligned
        QCOMPARE(quintptr(data->data()) % alignment, quintptr(0u));

        // Check that the allocated array can be used. Best tested with a
        // memory checker, such as valgrind, running.
        ::memset(data->data(), 'A', sizeof(Unaligned) * 8);
    }
}

void tst_QArrayData::typedData()
{
    QStaticArrayData<int, 10> data = {
            Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(int, 10),
            { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
        };
    QCOMPARE(data.header.size, 10);

    {
        QTypedArrayData<int> *array =
            static_cast<QTypedArrayData<int> *>(&data.header);
        QCOMPARE(array->data(), data.data);

        int j = 0;
        for (QTypedArrayData<int>::iterator iter = array->begin();
                iter != array->end(); ++iter, ++j)
            QCOMPARE(iter, data.data + j);
        QCOMPARE(j, 10);
    }

    {
        const QTypedArrayData<int> *array =
            static_cast<const QTypedArrayData<int> *>(&data.header);

        QCOMPARE(array->data(), data.data);

        int j = 0;
        for (QTypedArrayData<int>::const_iterator iter = array->begin();
                iter != array->end(); ++iter, ++j)
            QCOMPARE(iter, data.data + j);
        QCOMPARE(j, 10);
    }

    {
        QTypedArrayData<int> *null = QTypedArrayData<int>::sharedNull();
        QTypedArrayData<int> *empty = QTypedArrayData<int>::allocate(0);

        QVERIFY(null != empty);

        QCOMPARE(null->size, 0);
        QCOMPARE(empty->size, 0);

        QCOMPARE(null->begin(), null->end());
        QCOMPARE(empty->begin(), empty->end());
    }


    {
        Deallocator keeper(sizeof(char),
                Q_ALIGNOF(QTypedArrayData<char>::AlignmentDummy));
        QArrayData *array = QTypedArrayData<char>::allocate(10);
        keeper.headers.append(array);

        QVERIFY(array);
        QCOMPARE(array->size, 0);
        QCOMPARE(array->alloc, 10u);

        // Check that the allocated array can be used. Best tested with a
        // memory checker, such as valgrind, running.
        ::memset(array->data(), 0, 10 * sizeof(char));

        keeper.headers.clear();
        QTypedArrayData<short>::deallocate(array);

        QVERIFY(true);
    }

    {
        Deallocator keeper(sizeof(short),
                Q_ALIGNOF(QTypedArrayData<short>::AlignmentDummy));
        QArrayData *array = QTypedArrayData<short>::allocate(10);
        keeper.headers.append(array);

        QVERIFY(array);
        QCOMPARE(array->size, 0);
        QCOMPARE(array->alloc, 10u);

        // Check that the allocated array can be used. Best tested with a
        // memory checker, such as valgrind, running.
        ::memset(array->data(), 0, 10 * sizeof(short));

        keeper.headers.clear();
        QTypedArrayData<short>::deallocate(array);

        QVERIFY(true);
    }

    {
        Deallocator keeper(sizeof(double),
                Q_ALIGNOF(QTypedArrayData<double>::AlignmentDummy));
        QArrayData *array = QTypedArrayData<double>::allocate(10);
        keeper.headers.append(array);

        QVERIFY(array);
        QCOMPARE(array->size, 0);
        QCOMPARE(array->alloc, 10u);

        // Check that the allocated array can be used. Best tested with a
        // memory checker, such as valgrind, running.
        ::memset(array->data(), 0, 10 * sizeof(double));

        keeper.headers.clear();
        QTypedArrayData<double>::deallocate(array);

        QVERIFY(true);
    }
}

struct CountedObject
{
    CountedObject()
        : id(liveCount++)
    {
    }

    CountedObject(const CountedObject &other)
        : id(other.id)
    {
        ++liveCount;
    }

    ~CountedObject()
    {
        --liveCount;
    }

    CountedObject &operator=(const CountedObject &other)
    {
        id = other.id;
        return *this;
    }

    struct LeakChecker
    {
        LeakChecker()
            : previousLiveCount(liveCount)
        {
        }

        ~LeakChecker()
        {
            QCOMPARE(liveCount, previousLiveCount);
        }

    private:
        const size_t previousLiveCount;
    };

    size_t id; // not unique
    static size_t liveCount;
};

size_t CountedObject::liveCount = 0;

void tst_QArrayData::arrayOps()
{
    CountedObject::LeakChecker leakChecker; Q_UNUSED(leakChecker)

    const int intArray[5] = { 80, 101, 100, 114, 111 };
    const QString stringArray[5] = {
        QLatin1String("just"),
        QLatin1String("for"),
        QLatin1String("testing"),
        QLatin1String("a"),
        QLatin1String("vector")
    };
    const CountedObject objArray[5];

    QVERIFY(!QTypeInfo<int>::isComplex && !QTypeInfo<int>::isStatic);
    QVERIFY(QTypeInfo<QString>::isComplex && !QTypeInfo<QString>::isStatic);
    QVERIFY(QTypeInfo<CountedObject>::isComplex && QTypeInfo<CountedObject>::isStatic);

    QCOMPARE(CountedObject::liveCount, size_t(5));
    for (size_t i = 0; i < 5; ++i)
        QCOMPARE(objArray[i].id, i);

    ////////////////////////////////////////////////////////////////////////////
    // copyAppend (I)
    SimpleVector<int> vi(intArray, intArray + 5);
    SimpleVector<QString> vs(stringArray, stringArray + 5);
    SimpleVector<CountedObject> vo(objArray, objArray + 5);

    QCOMPARE(CountedObject::liveCount, size_t(10));
    for (int i = 0; i < 5; ++i) {
        QCOMPARE(vi[i], intArray[i]);
        QVERIFY(vs[i].isSharedWith(stringArray[i]));
        QCOMPARE(vo[i].id, objArray[i].id);
    }

    ////////////////////////////////////////////////////////////////////////////
    // destroyAll
    vi.clear();
    vs.clear();
    vo.clear();

    QCOMPARE(CountedObject::liveCount, size_t(5));

    ////////////////////////////////////////////////////////////////////////////
    // copyAppend (II)
    int referenceInt = 7;
    QString referenceString = QLatin1String("reference");
    CountedObject referenceObject;

    vi = SimpleVector<int>(5, referenceInt);
    vs = SimpleVector<QString>(5, referenceString);
    vo = SimpleVector<CountedObject>(5, referenceObject);

    QCOMPARE(CountedObject::liveCount, size_t(11));
    for (int i = 0; i < 5; ++i) {
        QCOMPARE(vi[i], referenceInt);
        QVERIFY(vs[i].isSharedWith(referenceString));
        QCOMPARE(vo[i].id, referenceObject.id);
    }
}

Q_DECLARE_METATYPE(QArrayDataPointer<int>)

static inline bool arrayIsFilledWith(const QArrayDataPointer<int> &array,
        int fillValue, size_t size)
{
    const int *iter = array->begin();
    const int *const end = array->end();

    for (size_t i = 0; i < size; ++i, ++iter)
        if (*iter != fillValue)
            return false;

    if (iter != end)
        return false;

    return true;
}

void tst_QArrayData::setSharable_data()
{
    QTest::addColumn<QArrayDataPointer<int> >("array");
    QTest::addColumn<size_t>("size");
    QTest::addColumn<size_t>("capacity");
    QTest::addColumn<bool>("isCapacityReserved");
    QTest::addColumn<int>("fillValue");

    QArrayDataPointer<int> null;
    QArrayDataPointer<int> empty; empty.clear();

    static QStaticArrayData<int, 10> staticArrayData = {
            Q_STATIC_ARRAY_DATA_HEADER_INITIALIZER(int, 10),
            { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 }
        };

    QArrayDataPointer<int> emptyReserved(QTypedArrayData<int>::allocate(5,
                QArrayData::CapacityReserved));
    QArrayDataPointer<int> nonEmpty(QTypedArrayData<int>::allocate(10,
                QArrayData::Default));
    QArrayDataPointer<int> nonEmptyReserved(QTypedArrayData<int>::allocate(15,
                QArrayData::CapacityReserved));
    QArrayDataPointer<int> staticArray(
            static_cast<QTypedArrayData<int> *>(&staticArrayData.header));

    nonEmpty->copyAppend(5, 1);
    nonEmptyReserved->copyAppend(7, 2);

    QTest::newRow("shared-null") << null << size_t(0) << size_t(0) << false << 0;
    QTest::newRow("shared-empty") << empty << size_t(0) << size_t(0) << false << 0;
    // unsharable-empty implicitly tested in shared-empty
    QTest::newRow("empty-reserved") << emptyReserved << size_t(0) << size_t(5) << true << 0;
    QTest::newRow("non-empty") << nonEmpty << size_t(5) << size_t(10) << false << 1;
    QTest::newRow("non-empty-reserved") << nonEmptyReserved << size_t(7) << size_t(15) << true << 2;
    QTest::newRow("static-array") << staticArray << size_t(10) << size_t(0) << false << 3;
}

void tst_QArrayData::setSharable()
{
    QFETCH(QArrayDataPointer<int>, array);
    QFETCH(size_t, size);
    QFETCH(size_t, capacity);
    QFETCH(bool, isCapacityReserved);
    QFETCH(int, fillValue);

    QVERIFY(array->ref.isShared()); // QTest has a copy
    QVERIFY(array->ref.isSharable());

    QCOMPARE(size_t(array->size), size);
    QCOMPARE(size_t(array->alloc), capacity);
    QCOMPARE(bool(array->capacityReserved), isCapacityReserved);
    QVERIFY(arrayIsFilledWith(array, fillValue, size));

    // shared-null becomes shared-empty, may otherwise detach
    array.setSharable(true);

    QVERIFY(array->ref.isSharable());
    QVERIFY(arrayIsFilledWith(array, fillValue, size));

    {
        QArrayDataPointer<int> copy(array);
        QVERIFY(array->ref.isShared());
        QVERIFY(array->ref.isSharable());
        QCOMPARE(copy.data(), array.data());
    }

    // Unshare, must detach
    array.setSharable(false);

    // Immutability (alloc == 0) is lost on detach
    if (capacity == 0 && size != 0)
        capacity = size;

    QVERIFY(!array->ref.isShared());
    QVERIFY(!array->ref.isSharable());

    QCOMPARE(size_t(array->size), size);
    QCOMPARE(size_t(array->alloc), capacity);
    QCOMPARE(bool(array->capacityReserved), isCapacityReserved);
    QVERIFY(arrayIsFilledWith(array, fillValue, size));

    {
        QArrayDataPointer<int> copy(array);
        QVERIFY(!array->ref.isShared());
        QVERIFY(!array->ref.isSharable());

        // Null/empty is always shared
        QCOMPARE(copy->ref.isShared(), !(size || isCapacityReserved));
        QVERIFY(copy->ref.isSharable());

        QCOMPARE(size_t(copy->size), size);
        QCOMPARE(size_t(copy->alloc), capacity);
        QCOMPARE(bool(copy->capacityReserved), isCapacityReserved);
        QVERIFY(arrayIsFilledWith(copy, fillValue, size));
    }

    // Make sharable, again
    array.setSharable(true);

    QCOMPARE(array->ref.isShared(), !(size || isCapacityReserved));
    QVERIFY(array->ref.isSharable());

    QCOMPARE(size_t(array->size), size);
    QCOMPARE(size_t(array->alloc), capacity);
    QCOMPARE(bool(array->capacityReserved), isCapacityReserved);
    QVERIFY(arrayIsFilledWith(array, fillValue, size));

    {
        QArrayDataPointer<int> copy(array);
        QVERIFY(array->ref.isShared());
        QCOMPARE(copy.data(), array.data());
    }

    QCOMPARE(array->ref.isShared(), !(size || isCapacityReserved));
    QVERIFY(array->ref.isSharable());
}

void tst_QArrayData::fromRawData()
{
    static const int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    {
        // Default: Immutable, sharable
        SimpleVector<int> raw = SimpleVector<int>::fromRawData(array,
                sizeof(array)/sizeof(array[0]), QArrayData::Default);

        QCOMPARE(raw.size(), size_t(11));
        QCOMPARE(raw.constBegin(), array);
        QCOMPARE(raw.constEnd(), array + sizeof(array)/sizeof(array[0]));

        QVERIFY(!raw.isShared());
        QVERIFY(SimpleVector<int>(raw).isSharedWith(raw));
        QVERIFY(!raw.isShared());

        // Detach
        QCOMPARE(raw.back(), 11);
        QVERIFY(raw.constBegin() != array);
    }

    {
        // Immutable, unsharable
        SimpleVector<int> raw = SimpleVector<int>::fromRawData(array,
                sizeof(array)/sizeof(array[0]), QArrayData::Unsharable);

        QCOMPARE(raw.size(), size_t(11));
        QCOMPARE(raw.constBegin(), array);
        QCOMPARE(raw.constEnd(), array + sizeof(array)/sizeof(array[0]));

        SimpleVector<int> copy(raw);
        QVERIFY(!copy.isSharedWith(raw));
        QVERIFY(!raw.isShared());

        QCOMPARE(copy.size(), size_t(11));

        for (size_t i = 0; i < 11; ++i)
            QCOMPARE(const_(copy)[i], const_(raw)[i]);

        QCOMPARE(raw.size(), size_t(11));
        QCOMPARE(raw.constBegin(), array);
        QCOMPARE(raw.constEnd(), array + sizeof(array)/sizeof(array[0]));

        // Detach
        QCOMPARE(raw.back(), 11);
        QVERIFY(raw.constBegin() != array);
    }
}

QTEST_APPLESS_MAIN(tst_QArrayData)
#include "tst_qarraydata.moc" 
