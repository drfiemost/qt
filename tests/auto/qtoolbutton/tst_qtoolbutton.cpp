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


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qtoolbutton.h>
#include <qmenu.h>
#include <qaction.h>
#include <qwindowsstyle.h>
#include <qstyleoption.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QToolButton : public QObject
{
Q_OBJECT

public:
    tst_QToolButton();
    virtual ~tst_QToolButton();

private slots:
    void getSetCheck();
    void triggered();
    void collapseTextOnPriority();
    void task230994_iconSize();
    void task176137_autoRepeatOfAction();
    void qtbug_34759_sizeHintResetWhenSettingMenu();

protected slots:
    void sendMouseClick();
private:
    QWidget *w;
};

tst_QToolButton::tst_QToolButton()
{
}

tst_QToolButton::~tst_QToolButton()
{
}

// Testing get/set functions
void tst_QToolButton::getSetCheck()
{
    QToolButton obj1;
    // QMenu* QToolButton::menu()
    // void QToolButton::setMenu(QMenu*)
    QMenu *var1 = new QMenu;
    obj1.setMenu(var1);
    QCOMPARE(var1, obj1.menu());
    obj1.setMenu((QMenu *)0);
    QCOMPARE((QMenu *)0, obj1.menu());
    delete var1;

    // ToolButtonPopupMode QToolButton::popupMode()
    // void QToolButton::setPopupMode(ToolButtonPopupMode)
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::DelayedPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::DelayedPopup), obj1.popupMode());
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::MenuButtonPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::MenuButtonPopup), obj1.popupMode());
    obj1.setPopupMode(QToolButton::ToolButtonPopupMode(QToolButton::InstantPopup));
    QCOMPARE(QToolButton::ToolButtonPopupMode(QToolButton::InstantPopup), obj1.popupMode());

    // bool QToolButton::autoRaise()
    // void QToolButton::setAutoRaise(bool)
    obj1.setAutoRaise(false);
    QCOMPARE(false, obj1.autoRaise());
    obj1.setAutoRaise(true);
    QCOMPARE(true, obj1.autoRaise());

    // QAction * QToolButton::defaultAction()
    // void QToolButton::setDefaultAction(QAction *)
    QAction *var4 = new QAction(0);
    obj1.setDefaultAction(var4);
    QCOMPARE(var4, obj1.defaultAction());
    obj1.setDefaultAction((QAction *)0);
    QCOMPARE((QAction *)0, obj1.defaultAction());
    delete var4;

#ifdef QT_HAS_QT3SUPPORT
    //ensure that popup delay is not reset on style change
    obj1.setPopupDelay(5);
    obj1.setStyle(new QWindowsStyle);
    QCOMPARE(obj1.popupDelay(), 5);
#endif
}

Q_DECLARE_METATYPE(QAction*)

void tst_QToolButton::triggered()
{
    qRegisterMetaType<QAction *>("QAction *");
    QToolButton tb;
    tb.show();
    QSignalSpy spy(&tb,SIGNAL(triggered(QAction*)));
    QMenu *menu = new QMenu("Menu");
    QAction *one = menu->addAction("one");
    menu->addAction("two");
    QAction *def = new QAction("def", this);

    tb.setMenu(menu);
    tb.setDefaultAction(def);

#ifdef Q_WS_QWS
    QApplication::processEvents(); //wait for the window system to show the tool button
#endif

    def->trigger();
    QCOMPARE(spy.count(),1);
    QCOMPARE(qvariant_cast<QAction *>(spy.at(0).at(0)), def);

    w = menu;
    QTimer::singleShot(30, this, SLOT(sendMouseClick()));
    tb.showMenu();
    QTest::qWait(20);
    QCOMPARE(spy.count(),2);
    QCOMPARE(qvariant_cast<QAction *>(spy.at(1).at(0)), one);
    delete menu;
}

void tst_QToolButton::collapseTextOnPriority()
{
    class MyToolButton : public QToolButton
    {
        friend class tst_QToolButton;
    public:
        void initStyleOption(QStyleOptionToolButton *option)
        {
            QToolButton::initStyleOption(option);
        }
    };

    MyToolButton button;
    button.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QAction action(button.style()->standardIcon(QStyle::SP_ArrowBack), "test", 0);
    button.setDefaultAction(&action);

    QStyleOptionToolButton option;
    button.initStyleOption(&option);
    QVERIFY(option.toolButtonStyle == Qt::ToolButtonTextBesideIcon);
    action.setPriority(QAction::LowPriority);
    button.initStyleOption(&option);
    QVERIFY(option.toolButtonStyle == Qt::ToolButtonIconOnly);
}


void tst_QToolButton::task230994_iconSize()
{
    //we check that the iconsize returned bu initStyleOption is valid
    //when the toolbutton has no parent
    class MyToolButton : public QToolButton
    {
        friend class tst_QToolButton;
    public:
        void initStyleOption(QStyleOptionToolButton *option)
        {
            QToolButton::initStyleOption(option);
        }
    };

    MyToolButton button;
    QStyleOptionToolButton option;
    button.initStyleOption(&option);
    QVERIFY(option.iconSize.isValid());
}

void tst_QToolButton::task176137_autoRepeatOfAction()
{
    QAction action(0);
    QToolButton tb;
    tb.setDefaultAction (&action);
    tb.show();
    QSignalSpy spy(&action,SIGNAL(triggered()));
    QTest::mousePress ( &tb, Qt::LeftButton);
    QTest::mouseRelease ( &tb, Qt::LeftButton, 0, QPoint (), 2000);
    QCOMPARE(spy.count(),1);

    // try again with auto repeat
    tb.setAutoRepeat (true);
    QSignalSpy repeatSpy(&action,SIGNAL(triggered())); // new spy
    QTest::mousePress ( &tb, Qt::LeftButton);
    QTest::mouseRelease ( &tb, Qt::LeftButton, 0, QPoint (), 3000);
    qreal expected = (3000 - tb.autoRepeatDelay()) / tb.autoRepeatInterval() + 1;
    //we check that the difference is small (on some systems timers are not super accurate)
    qreal diff = (expected - repeatSpy.count()) / expected;
    QVERIFY2(std::abs(diff) < 0.2, qPrintable(
        QString("expected: %1, actual: %2, diff (fraction): %3")
            .arg(expected)
            .arg(repeatSpy.count())
            .arg(diff)));
}


void tst_QToolButton::sendMouseClick()
{
    QTest::mouseClick(w, Qt::LeftButton, 0);
}

void tst_QToolButton::qtbug_34759_sizeHintResetWhenSettingMenu()
{
    // There is no reliable way of checking what's ultimately a style-dependent
    // sizing. So the idea is checking if the size is the "correct" size w.r.t.
    // another toolbutton which has had a menu set before it was shown for the first time

    QToolButton button1;
    QToolButton button2;

    button1.setToolButtonStyle(Qt::ToolButtonIconOnly);
    button1.setPopupMode(QToolButton::MenuButtonPopup);

    button2.setToolButtonStyle(Qt::ToolButtonIconOnly);
    button2.setPopupMode(QToolButton::MenuButtonPopup);

    button2.setMenu(new QMenu(&button2));

    button1.show();
    button2.show();

    QVERIFY(QTest::qWaitForWindowExposed(&button1));
    QVERIFY(QTest::qWaitForWindowExposed(&button2));

    button1.setMenu(new QMenu(&button1));
    QTRY_COMPARE(button1.sizeHint(), button2.sizeHint());
}

QTEST_MAIN(tst_QToolButton)
#include "tst_qtoolbutton.moc"
