/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGROUPBOX_H
#define QGROUPBOX_H

#include <QtGui/qframe.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_GROUPBOX

class QGroupBoxPrivate;
class QStyleOptionGroupBox;
class Q_GUI_EXPORT QGroupBox : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked DESIGNABLE isCheckable NOTIFY toggled USER true)
public:
    explicit QGroupBox(QWidget* parent=nullptr);
    explicit QGroupBox(const QString &title, QWidget* parent=nullptr);
    ~QGroupBox() override;

    QString title() const;
    void setTitle(const QString &title);

    Qt::Alignment alignment() const;
    void setAlignment(int alignment);

    QSize minimumSizeHint() const override;

    bool isFlat() const;
    void setFlat(bool flat);
    bool isCheckable() const;
    void setCheckable(bool checkable);
    bool isChecked() const;

public Q_SLOTS:
    void setChecked(bool checked);

Q_SIGNALS:
    void clicked(bool checked = false);
    void toggled(bool);

protected:
    bool event(QEvent *event) override;
    void childEvent(QChildEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;  
    void initStyleOption(QStyleOptionGroupBox *option) const;

private:
    Q_DISABLE_COPY(QGroupBox)
    Q_DECLARE_PRIVATE(QGroupBox)
    Q_PRIVATE_SLOT(d_func(), void _q_setChildrenEnabled(bool b))
};

#endif // QT_NO_GROUPBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGROUPBOX_H
