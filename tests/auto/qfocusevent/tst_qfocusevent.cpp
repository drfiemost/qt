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
#include <qapplication.h>
#include <qlineedit.h>

#include <qmenu.h>

#include <qlabel.h>
#include <qdialog.h>
#include <qevent.h>
#include <qlineedit.h>
#include <QBoxLayout>


QT_FORWARD_DECLARE_CLASS(QWidget)

//TESTED_CLASS=
//TESTED_FILES=gui/kernel/qevent.h gui/kernel/qevent.cpp

class FocusLineEdit : public QLineEdit
{
public:
    FocusLineEdit( QWidget* parent = 0, const char* name = 0 ) : QLineEdit(name, parent) {}
    int focusInEventReason;
    int focusOutEventReason;
    bool focusInEventRecieved;
    bool focusInEventGotFocus;
    bool focusOutEventRecieved;
    bool focusOutEventLostFocus;
protected:
    virtual void keyPressEvent( QKeyEvent *e )
    {
//        qDebug( QString("keyPressEvent: %1").arg(e->key()) );
        QLineEdit::keyPressEvent( e );
    }
    void focusInEvent( QFocusEvent* e )
    {
        QLineEdit::focusInEvent( e );
        focusInEventReason = e->reason();
	    focusInEventGotFocus = e->gotFocus();
        focusInEventRecieved = TRUE;
    }
    void focusOutEvent( QFocusEvent* e )
    {
        QLineEdit::focusOutEvent( e );
        focusOutEventReason = e->reason();
	    focusOutEventLostFocus = !e->gotFocus();
        focusOutEventRecieved = TRUE;
    }
};

class tst_QFocusEvent : public QObject
{
    Q_OBJECT

public:
    tst_QFocusEvent();
    virtual ~tst_QFocusEvent();


    void initWidget();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void checkReason_Tab();
    void checkReason_ShiftTab();
    void checkReason_BackTab();
    void checkReason_Popup();
    void checkReason_focusWidget();
    void checkReason_Shortcut();
    void checkReason_ActiveWindow();

private:
    QWidget* testFocusWidget;
    FocusLineEdit* childFocusWidgetOne;
    FocusLineEdit* childFocusWidgetTwo;
};

tst_QFocusEvent::tst_QFocusEvent()
{
}

tst_QFocusEvent::~tst_QFocusEvent()
{

}

void tst_QFocusEvent::initTestCase()
{
    testFocusWidget = new QWidget( 0 );
    childFocusWidgetOne = new FocusLineEdit( testFocusWidget );
    childFocusWidgetOne->setGeometry( 10, 10, 180, 20 );
    childFocusWidgetTwo = new FocusLineEdit( testFocusWidget );
    childFocusWidgetTwo->setGeometry( 10, 50, 180, 20 );

    //qApp->setMainWidget( testFocusWidget ); Qt4
    testFocusWidget->resize( 200,100 );
    testFocusWidget->show();
// Applications don't get focus when launched from the command line on Mac.
#ifdef Q_WS_MAC
    testFocusWidget->raise();
#endif
}

void tst_QFocusEvent::cleanupTestCase()
{
    delete testFocusWidget;
}

void tst_QFocusEvent::init()
{
}

void tst_QFocusEvent::cleanup()
{
    childFocusWidgetTwo->setGeometry( 10, 50, 180, 20 );
}

void tst_QFocusEvent::initWidget()
{
    // On X11 we have to ensure the event was processed before doing any checking, on Windows
    // this is processed straight away.
    QApplication::setActiveWindow(childFocusWidgetOne);

    for (int i = 0; i < 1000; ++i) {
	if (childFocusWidgetOne->isActiveWindow() && childFocusWidgetOne->hasFocus())
	    break;
	childFocusWidgetOne->activateWindow();
	childFocusWidgetOne->setFocus();
	qApp->processEvents();
	QTest::qWait(100);
    }

    // The first lineedit should have focus
    QVERIFY( childFocusWidgetOne->hasFocus() );

    childFocusWidgetOne->focusInEventRecieved = FALSE;
    childFocusWidgetOne->focusInEventGotFocus = FALSE;
    childFocusWidgetOne->focusInEventReason = -1;
    childFocusWidgetOne->focusOutEventRecieved = FALSE;
    childFocusWidgetOne->focusOutEventLostFocus = FALSE;
    childFocusWidgetOne->focusOutEventReason = -1;
    childFocusWidgetTwo->focusInEventRecieved = FALSE;
    childFocusWidgetTwo->focusInEventGotFocus = FALSE;
    childFocusWidgetTwo->focusInEventReason = -1;
    childFocusWidgetTwo->focusOutEventRecieved = FALSE;
    childFocusWidgetTwo->focusOutEventLostFocus = FALSE;
    childFocusWidgetTwo->focusOutEventReason = -1;
}

void tst_QFocusEvent::checkReason_Tab()
{
    initWidget();

    // Now test the tab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Tab );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int) Qt::TabFocusReason );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int) Qt::TabFocusReason );
}

void tst_QFocusEvent::checkReason_ShiftTab()
{
    initWidget();

    // Now test the shift + tab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Tab, Qt::ShiftModifier );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::BacktabFocusReason );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int)Qt::BacktabFocusReason );

}

/*!
    In this test we verify that the Qt::KeyBacktab key is handled in a qfocusevent
*/
void tst_QFocusEvent::checkReason_BackTab()
{
#ifdef Q_OS_WIN32 // key is not supported on Windows
    QSKIP( "Backtab is not supported on Windows", SkipAll );
#else
    initWidget();
    QVERIFY( childFocusWidgetOne->hasFocus() );

    // Now test the backtab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Backtab );
    QTest::qWait(200);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, int(Qt::BacktabFocusReason) );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, int(Qt::BacktabFocusReason) );
#endif
}

void tst_QFocusEvent::checkReason_Popup()
{
    initWidget();

    // Now test the popup reason
    QMenu* popupMenu = new QMenu( testFocusWidget );
    popupMenu->addMenu( "Test" );
    popupMenu->popup( QPoint(0,0) );
    QVERIFY(QTest::qWaitForWindowShown(popupMenu));

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventLostFocus);

    QTRY_VERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, int(Qt::PopupFocusReason));

    popupMenu->hide();

    QVERIFY(childFocusWidgetOne->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusInEventGotFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
}

#ifdef Q_WS_MAC
QT_BEGIN_NAMESPACE
    extern void qt_set_sequence_auto_mnemonic(bool);
QT_END_NAMESPACE
#endif

void tst_QFocusEvent::checkReason_Shortcut()
{
    initWidget();
#ifdef Q_WS_MAC
    qt_set_sequence_auto_mnemonic(true);
#endif
    QLabel* label = new QLabel( "&Test", testFocusWidget );
    label->setBuddy( childFocusWidgetTwo );
    label->setGeometry( 10, 50, 90, 20 );
    childFocusWidgetTwo->setGeometry( 105, 50, 95, 20 );
    label->show();

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( !childFocusWidgetTwo->hasFocus() );

    QTest::keyClick( label, Qt::Key_T, Qt::AltModifier );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::ShortcutFocusReason );
    QVERIFY( childFocusWidgetTwo->focusInEventRecieved );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int)Qt::ShortcutFocusReason );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );

    label->hide();
    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QVERIFY( !childFocusWidgetOne->hasFocus() );
#ifdef Q_WS_MAC
    qt_set_sequence_auto_mnemonic(false);
#endif
}

void tst_QFocusEvent::checkReason_focusWidget()
{
    // This test checks that a widget doesn't loose
    // its focuswidget just because the focuswidget looses focus.
    QWidget window1;
    QWidget frame1;
    QWidget frame2;
    QLineEdit edit1;
    QLineEdit edit2;

    QVBoxLayout outerLayout;
    outerLayout.addWidget(&frame1);
    outerLayout.addWidget(&frame2);
    window1.setLayout(&outerLayout);

    QVBoxLayout leftLayout;
    QVBoxLayout rightLayout;
    leftLayout.addWidget(&edit1);
    rightLayout.addWidget(&edit2);
    frame1.setLayout(&leftLayout);
    frame2.setLayout(&rightLayout);
    window1.show();

    edit1.setFocus();
    QTest::qWait(100);
    edit2.setFocus();

    QVERIFY(frame1.focusWidget() != 0);
    QVERIFY(frame2.focusWidget() != 0);
}

void tst_QFocusEvent::checkReason_ActiveWindow()
{
    initWidget();

    QDialog* d = new QDialog( testFocusWidget );
    d->show();
    d->activateWindow(); // ### CDE
    QApplication::setActiveWindow(d);
    QTest::qWaitForWindowShown(d);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);

    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::ActiveWindowFocusReason);
    QVERIFY( !childFocusWidgetOne->hasFocus() );

    d->hide();
    QTest::qWait(100);

#if defined(Q_OS_MAC)
    QEXPECT_FAIL("", "QTQAINFRA-428", Abort);
#endif
    QTRY_VERIFY(childFocusWidgetOne->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusInEventGotFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( childFocusWidgetOne->focusInEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusInEventReason, (int)Qt::ActiveWindowFocusReason);
}


QTEST_MAIN(tst_QFocusEvent)
#include "tst_qfocusevent.moc"
