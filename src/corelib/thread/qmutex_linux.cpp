/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "qplatformdefs.h"
#include "qmutex.h"

#ifndef QT_NO_THREAD
#include "qatomic.h"
#include "qmutex_p.h"
#include "qelapsedtimer.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#ifndef QT_LINUX_FUTEX
# error "Qt build is broken: qmutex_linux.cpp is being built but futex support is not wanted"
#endif

QT_BEGIN_NAMESPACE

static QBasicAtomicInt futexFlagSupport = Q_BASIC_ATOMIC_INITIALIZER(-1);

static int checkFutexPrivateSupport()
{
    int value = 0;
#if defined(FUTEX_PRIVATE_FLAG)
    // check if the kernel supports extra futex flags
    // FUTEX_PRIVATE_FLAG appeared in v2.6.22
    static_assert(FUTEX_PRIVATE_FLAG != 0x80000000);

    // try an operation that has no side-effects: wake up 42 threads
    // futex will return -1 (errno==ENOSYS) if the flag isn't supported
    // there should be no other error conditions
    value = syscall(__NR_futex, &futexFlagSupport,
                    FUTEX_WAKE | FUTEX_PRIVATE_FLAG,
                    42, 0, 0, 0);
    if (value != -1)
        value = FUTEX_PRIVATE_FLAG;
    else
        value = 0;

#else
    value = 0;
#endif
    futexFlagSupport.store(value);
    return value;
}

static inline int futexFlags()
{
    int value = futexFlagSupport.load();
    if (Q_LIKELY(value != -1))
        return value;
    return checkFutexPrivateSupport();
}

static inline int _q_futex(void *addr, int op, int val, const struct timespec *timeout)
{
    volatile int *int_addr = reinterpret_cast<volatile int *>(addr);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN && QT_POINTER_SIZE == 8
    int_addr++; //We want a pointer to the 32 least significant bit of QMutex::d
#endif
    int *addr2 = 0;
    int val2 = 0;
    return syscall(SYS_futex, int_addr, op | futexFlags(), val, timeout, addr2, val2);
}

static inline QMutexData *dummyFutexValue()
{
    return reinterpret_cast<QMutexData *>(quintptr(3));
}


bool QBasicMutex::lockInternal(int timeout)
{
    // we're here because fastTryLock() has just failed
    QMutexData *d = d_ptr.load();
    if (quintptr(d) > 0x3) { //d == dummyLocked() || d == dummyFutexValue()
        Q_ASSERT(d->recursive);
        return static_cast<QRecursiveMutexPrivate *>(d)->lock(timeout);
    }

    if (timeout == 0)
        return false;

    QElapsedTimer elapsedTimer;
    if (timeout >= 1)
        elapsedTimer.start();

    while (!fastTryLock()) {
        d = d_ptr.load();

        if (!d) // if d is 0, the mutex is unlocked
            continue;

        // the mutex is locked already, set a bit indicating we're waiting
        while (d_ptr.fetchAndStoreAcquire(dummyFutexValue()) != 0) {
            struct timespec ts, *pts = 0;
            if (timeout >= 1) {
                // recalculate the timeout
                qint64 xtimeout = qint64(timeout) * 1000 * 1000;
                xtimeout -= elapsedTimer.nsecsElapsed();
                if (xtimeout <= 0) {
                    // timer expired after we returned
                    return false;
                }
                ts.tv_sec = xtimeout / Q_INT64_C(1000) / 1000 / 1000;
                ts.tv_nsec = xtimeout % (Q_INT64_C(1000) * 1000 * 1000);
                pts = &ts;
            }

            // successfully set the waiting bit, now sleep
            int r = _q_futex(&d_ptr, FUTEX_WAIT, quintptr(dummyFutexValue()), pts);
            if (r != 0 && errno == ETIMEDOUT)
                return false;
        }
        return true;
    }
    Q_ASSERT(d_ptr.load());
    return true;
}

void QBasicMutex::unlockInternal()
{
    QMutexData *d = d_ptr.load();
    Q_ASSERT(d); //we must be locked
    Q_ASSERT(d != dummyLocked()); // testAndSetRelease(dummyLocked(), 0) failed

    if (d == dummyFutexValue()) {
        d_ptr.fetchAndStoreRelease(0);
        _q_futex(&d_ptr, FUTEX_WAKE, 1, 0);
        return;
    }

    Q_ASSERT(d->recursive);
    static_cast<QRecursiveMutexPrivate *>(d)->unlock();
}


QT_END_NAMESPACE

#endif // QT_NO_THREAD
