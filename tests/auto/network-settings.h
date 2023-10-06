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

#include <QString>
#ifdef QT_NETWORK_LIB
#include <QtNetwork/QHostInfo>
#endif


class QtNetworkSettings
{
public:

    static QString serverLocalName()
    {
        return QString("qt-test-server");
    }
    static QString serverDomainName()
    {
        return QString("qt-test-net");
    }
    static QString serverName()
    {
        return serverLocalName() + "." + serverDomainName();
    }
    static QString winServerName()
    {
        return serverName();
    }
    static QString wildcardServerName()
    {
        return "qt-test-server.wildcard.dev." + serverDomainName();
    }

#ifdef QT_NETWORK_LIB
    static QHostAddress serverIP()
    {
        return QHostInfo::fromName(serverName()).addresses().first();
    }
#endif

    static bool compareReplyIMAP(QByteArray const& actual)
    {
        // Server greeting may contain capability, version and server name
        // But spec only requires "* OK" and "\r\n"
        // Match against a prefix and postfix that covers all Cyrus versions
        if (actual.startsWith("* OK ")
            && actual.endsWith("server ready\r\n")) {
            return true;
        }

        return false;
    }

    static bool compareReplyIMAPSSL(QByteArray const& actual)
    {
        return compareReplyIMAP(actual);
    }

    static bool compareReplyFtp(QByteArray const& actual)
    {
        QList<QByteArray> expected;

        // A few different vsFTPd versions.
        // Feel free to add more as needed
        expected << QByteArray( "220 (vsFTPd 2.0.5)\r\n221 Goodbye.\r\n" );
        expected << QByteArray( "220 (vsFTPd 2.2.2)\r\n221 Goodbye.\r\n" );

        Q_FOREACH (QByteArray const& ba, expected) {
            if (ba == actual) {
                return true;
            }
        }

        return false;
    }
};

#define Q_SET_DEFAULT_IAP

#ifdef QT_NETWORK_LIB
class QtNetworkSettingsInitializerCode {
public:
    QtNetworkSettingsInitializerCode() {
        QHostInfo testServerResult = QHostInfo::fromName(QtNetworkSettings::serverName());
        if (testServerResult.error() != QHostInfo::NoError) {
            qWarning() << "Could not lookup" << QtNetworkSettings::serverName();
            qWarning() << "Please configure the test environment!";
            qWarning() << "See /etc/hosts or network-settings.h";
            qFatal("Exiting");
        }
    }
};
QtNetworkSettingsInitializerCode qtNetworkSettingsInitializer;
#endif
