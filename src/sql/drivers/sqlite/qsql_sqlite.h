/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQL_SQLITE_H
#define QSQL_SQLITE_H

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlresult.h>
#include <QtSql/private/qsqlcachedresult_p.h>

struct sqlite3;

#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_SQLITE
#else
#define Q_EXPORT_SQLDRIVER_SQLITE Q_SQL_EXPORT
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QSQLiteDriverPrivate;
class QSQLiteResultPrivate;
class QSQLiteDriver;

class QSQLiteResult : public QSqlCachedResult
{
    friend class QSQLiteDriver;
    friend class QSQLiteResultPrivate;
public:
    explicit QSQLiteResult(const QSQLiteDriver* db);
    ~QSQLiteResult() override;
    QVariant handle() const override;

protected:
    bool gotoNext(QSqlCachedResult::ValueCache& row, int idx) override;
    bool reset(const QString &query) override;
    bool prepare(const QString &query) override;
    bool exec() override;
    int size() override;
    int numRowsAffected() override;
    QVariant lastInsertId() const override;
    QSqlRecord record() const override;
    void virtual_hook(int id, void *data) override;

private:
    QSQLiteResultPrivate* d;
};

class Q_EXPORT_SQLDRIVER_SQLITE QSQLiteDriver : public QSqlDriver
{
    Q_OBJECT
    friend class QSQLiteResult;
public:
    explicit QSQLiteDriver(QObject *parent = nullptr);
    explicit QSQLiteDriver(sqlite3 *connection, QObject *parent = nullptr);
    ~QSQLiteDriver() override;
    bool hasFeature(DriverFeature f) const override;
    bool open(const QString & db,
                   const QString & user,
                   const QString & password,
                   const QString & host,
                   int port,
                   const QString & connOpts) override;
    void close() override;
    QSqlResult *createResult() const override;
    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;
    QStringList tables(QSql::TableType) const override;

    QSqlRecord record(const QString& tablename) const override;
    QSqlIndex primaryIndex(const QString &table) const override;
    QVariant handle() const override;
    QString escapeIdentifier(const QString &identifier, IdentifierType) const override;

private:
    QSQLiteDriverPrivate* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQL_SQLITE_H
