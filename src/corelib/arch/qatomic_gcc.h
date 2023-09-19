/****************************************************************************
**
** Copyright (C) 2011 Thiago Macieira <thiago@kde.org>
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QATOMIC_GCC_H
#define QATOMIC_GCC_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
QT_END_HEADER

#pragma qt_sync_skip_header_check
#pragma qt_sync_stop_processing
#endif

template<> struct QAtomicOpsSupport<8> { enum { IsSupported = 1 }; };

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

#define Q_ATOMIC_INT32_IS_SUPPORTED
#define Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

#define Q_ATOMIC_POINTER_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

#if QT_POINTER_SIZE == 8
#  define Q_ATOMIC_INT64_IS_SUPPORTED
#  define Q_ATOMIC_INT64_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#  define Q_ATOMIC_INT64_TEST_AND_SET_IS_SOMETIMES_NATIVE
#  define Q_ATOMIC_INT64_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#  define Q_ATOMIC_INT64_FETCH_AND_ADD_IS_SOMETIMES_NATIVE
template<> struct QAtomicIntegerTraits<long long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long long> { enum { IsInteger = 1 }; };
#endif

template <typename X> struct QAtomicOps: QGenericAtomicOps<QAtomicOps<X> >
{
    // The GCC intrinsics all have fully-ordered memory semantics, so we define
    // only the xxxRelaxed functions. The exception is __sync_lock_and_test,
    // which has acquire semantics, so we need to define the Release and
    // Ordered versions too.

    typedef X Type;

#ifndef __ia64__
    template <typename T>
    static T loadAcquire(T &_q_value) noexcept
    {
        T tmp = _q_value;
        __sync_synchronize();
        return tmp;
    }

    template <typename T>
    static void storeRelease(T &_q_value, T newValue) noexcept
    {
        __sync_synchronize();
        _q_value = newValue;
    }
#endif

    static constexpr bool isTestAndSetNative() noexcept { return false; }
    static constexpr bool isTestAndSetWaitFree() noexcept { return false; }
    template <typename T>
    static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue) noexcept
    {
        return __sync_bool_compare_and_swap(&_q_value, expectedValue, newValue);
    }

    template <typename T>
    static T fetchAndStoreRelaxed(T &_q_value, T newValue) noexcept
    {
        return __sync_lock_test_and_set(&_q_value, newValue);
    }

    template <typename T>
    static T fetchAndStoreRelease(T &_q_value, T newValue) noexcept
    {
        __sync_synchronize();
        return __sync_lock_test_and_set(&_q_value, newValue);
    }

    template <typename T>
    static T fetchAndStoreOrdered(T &_q_value, T newValue) noexcept
    {
        return fetchAndStoreRelease(_q_value, newValue);
    }

    template <typename T> static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        return __sync_fetch_and_add(&_q_value, valueToAdd * QAtomicAdditiveType<T>::AddScale);
    }
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QATOMIC_GCC_H
