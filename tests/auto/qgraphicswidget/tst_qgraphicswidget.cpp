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
#include <qgraphicswidget.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qgraphicsview.h>
#include <qstyleoption.h>
#include <qgraphicslinearlayout.h>
#include <qcleanlooksstyle.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <qwidgetaction.h>
#include "../platformquirks.h"


class EventSpy : public QObject
{
    Q_OBJECT
public:
    EventSpy(QObject *watched, QEvent::Type type)
        : _count(0), spied(type)
    {
        watched->installEventFilter(this);
    }

    int count() const { return _count; }

protected:
    bool eventFilter(QObject *watched, QEvent *event)
    {
        Q_UNUSED(watched);
        if (event->type() == spied)
            ++_count;
        return false;
    }

    int _count;
    QEvent::Type spied;
};

#ifndef QT_NO_STYLE_CLEANLOOKS
class tst_QGraphicsWidget : public QObject {
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgraphicswidget();

    void activation();
    void boundingRect_data();
    void boundingRect();
    void dumpFocusChain_data();
    void dumpFocusChain();
    void focusWidget_data();
    void focusWidget();
    void focusWidget2();
    void focusWidget3();
    void focusPolicy_data();
    void focusPolicy();
    void font_data();
    void font();
    void fontPropagation();
    void fontChangedEvent();
    void fontPropagationWidgetItemWidget();
    void fontPropagationSceneChange();
    void geometry_data();
    void geometry();
    void geometryChanged();
    void width();
    void height();
    void getContentsMargins_data();
    void getContentsMargins();
    void initStyleOption_data();
    void initStyleOption();
    void layout_data();
    void layout();
    void layoutDirection_data();
    void layoutDirection();
    void recursiveLayoutDirection_data();
    void recursiveLayoutDirection();
    void paint_data();
    void paint();
    void palettePropagation();
    void parentWidget_data();
    void parentWidget();
    void resize_data();
    void resize();
    void setAttribute_data();
    void setAttribute();
    void setStyle_data();
    void setStyle();
    void setTabOrder_data();
    void setTabOrder();
    void setTabOrderAndReparent();
    void topLevelWidget_data();
    void topLevelWidget();
    void unsetLayoutDirection_data();
    void unsetLayoutDirection();
    void focusNextPrevChild_data();
    void focusNextPrevChild();
    void verifyFocusChain();
    void updateFocusChainWhenChildDie();
    void sizeHint_data();
    void sizeHint();
    void consistentPosSizeGeometry_data();
    void consistentPosSizeGeometry();
    void setSizes_data();
    void setSizes();
    void closePopupOnOutsideClick();
    void defaultSize();
    void explicitMouseGrabber();
    void implicitMouseGrabber();
    void doubleClickAfterExplicitMouseGrab();
    void popupMouseGrabber();
    void windowFlags_data();
    void windowFlags();
    void shortcutsDeletion();
    void painterStateProtectionOnWindowFrame();
    void ensureClipping();
    void widgetSendsGeometryChanges();
    void respectHFW();
    void addChildInpolishEvent();
    void polishEvent();
    void polishEvent2();
    void autoFillBackground();
    void initialShow();
    void initialShow2();
    void itemChangeEvents();
    void itemSendGeometryPosChangesDeactivated();

    void fontPropagatesResolveToChildren();
    void fontPropagatesResolveToGrandChildren();
    void fontPropagatesResolveInParentChange();
    void fontPropagatesResolveViaNonWidget();
    void fontPropagatesResolveFromScene();

    // Task fixes
    void task236127_bspTreeIndexFails();
    void task243004_setStyleCrash();
    void task250119_shortcutContext();
    void QT_BUG_6544_tabFocusFirstUnsetWhenRemovingItems();
    void QT_BUG_12056_tabFocusFirstUnsetWhenRemovingItems();
};


static void sendMouseMove(QWidget *widget, const QPoint &point, Qt::MouseButton button = Qt::NoButton, Qt::MouseButtons buttons = 0)
{
    QTest::mouseMove(widget, point);
    QMouseEvent event(QEvent::MouseMove, point, button, buttons, 0);
    QApplication::sendEvent(widget, &event);
}

// Subclass that exposes the protected functions.
class SubQGraphicsWidget : public QGraphicsWidget {
public:
    SubQGraphicsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags windowFlags = 0)
        : QGraphicsWidget(parent, windowFlags), eventCount(0)
        { }

    void initStyleOption(QStyleOption *option)
        { QGraphicsWidget::initStyleOption(option); }

    void call_changeEvent(QEvent* event)
        { return QGraphicsWidget::changeEvent(event); }

    bool call_event(QEvent *e)
        { return event(e); }

    void call_focusInEvent(QFocusEvent* event)
        { return QGraphicsWidget::focusInEvent(event); }

    bool call_focusNextPrevChild(bool next)
        { return QGraphicsWidget::focusNextPrevChild(next); }

    void call_focusOutEvent(QFocusEvent* event)
        { return QGraphicsWidget::focusOutEvent(event); }

    void call_hideEvent(QHideEvent* event)
        { return QGraphicsWidget::hideEvent(event); }

    QVariant call_itemChange(QGraphicsItem::GraphicsItemChange change, QVariant const& value)
        { return QGraphicsWidget::itemChange(change, value); }

    void call_moveEvent(QGraphicsSceneMoveEvent* event)
        { return QGraphicsWidget::moveEvent(event); }

    void call_polishEvent()
        { return QGraphicsWidget::polishEvent(); }

    QVariant call_propertyChange(QString const& propertyName, QVariant const& value)
        { return QGraphicsWidget::propertyChange(propertyName, value); }

    void call_resizeEvent(QGraphicsSceneResizeEvent* event)
        { return QGraphicsWidget::resizeEvent(event); }

    bool call_sceneEvent(QEvent* event)
        { return QGraphicsWidget::sceneEvent(event); }

    void call_showEvent(QShowEvent* event)
        { return QGraphicsWidget::showEvent(event); }

    QSizeF call_sizeHint(Qt::SizeHint which, QSizeF const& constraint = QSizeF()) const
        { return QGraphicsWidget::sizeHint(which, constraint); }

    void call_updateGeometry()
        { return QGraphicsWidget::updateGeometry(); }

    int eventCount;
    Qt::LayoutDirection m_painterLayoutDirection;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        m_painterLayoutDirection = painter->layoutDirection();
        QGraphicsWidget::paint(painter, option, widget);
        if (hasFocus()) {
            painter->setPen(Qt::DotLine);
            painter->drawRect(rect());
        }
        //painter->drawText(QPointF(0,15), data(0).toString());
    }

protected:
    bool event(QEvent *event)
    {
        eventCount++;
        return QGraphicsWidget::event(event);
    }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGraphicsWidget::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGraphicsWidget::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGraphicsWidget::init()
{
}

// This will be called after every test function.
void tst_QGraphicsWidget::cleanup()
{
}

class SizeHinter : public QGraphicsWidget
{
public:
    SizeHinter(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0,
                const QSizeF &min = QSizeF(5,5),
                const QSizeF &pref = QSizeF(50, 50),
                const QSizeF &max = QSizeF(500, 500))
        : QGraphicsWidget(parent, wFlags)
    {
        m_sizes[Qt::MinimumSize] = min;
        m_sizes[Qt::PreferredSize] = pref;
        m_sizes[Qt::MaximumSize] = max;

    }
    void setSizeHint(Qt::SizeHint which, const QSizeF &newSizeHint)
    {
        m_sizes[which] = newSizeHint;
    }

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
    {
        Q_UNUSED(constraint);
        return m_sizes[which];
    }
private:
    QSizeF m_sizes[4];
};

void tst_QGraphicsWidget::qgraphicswidget()
{
    SubQGraphicsWidget widget;
    QVERIFY(widget.isVisible());

    QVERIFY(!widget.isWindow());
    QCOMPARE(widget.boundingRect(), QRectF(0, 0, 0, 0));
    QCOMPARE(widget.focusWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widget.focusPolicy(), Qt::NoFocus);
    QCOMPARE(widget.font(), QFont());
    QCOMPARE(widget.geometry(), QRectF(widget.pos(), widget.size()));
    QCOMPARE(widget.layout(), (QGraphicsLayout*)0);
    QCOMPARE(widget.layoutDirection(), Qt::LeftToRight);
    QCOMPARE(widget.palette(), QPalette());
    QCOMPARE(widget.parentWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widget.rect(), QRectF(QPointF(), widget.size()));
    QCOMPARE(widget.size(), QSizeF(0, 0));
    QVERIFY(widget.style() != (QStyle*)0);
    QCOMPARE(widget.testAttribute(Qt::WA_AcceptDrops), false);
    QCOMPARE(widget.topLevelWidget(), (QGraphicsWidget*)&widget);
    QCOMPARE(widget.type(), (int)QGraphicsWidget::Type);
    QCOMPARE(widget.call_propertyChange(QString(), QVariant()), QVariant());
    widget.call_sizeHint(Qt::PreferredSize, QSizeF());

    QGraphicsScene scene;
    QGraphicsWidget *parent = new QGraphicsWidget;
    SizeHinter *child = new SizeHinter(parent);

    QCOMPARE(child->minimumSize(), QSizeF(5, 5));
}

void tst_QGraphicsWidget::activation()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *window1 = new QGraphicsWidget(0, Qt::Window);
    QGraphicsWidget *window2 = new QGraphicsWidget(0, Qt::Window);
    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QGraphicsScene scene;
    scene.addItem(widget);
    scene.addItem(window1);
    scene.addItem(window2);

    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QEvent activateEvent(QEvent::WindowActivate);
    QApplication::sendEvent(&scene, &activateEvent);

    QVERIFY(!widget->isActiveWindow());
    QVERIFY(window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    scene.setActiveWindow(window1);
    QVERIFY(!widget->isActiveWindow());
    QVERIFY(window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QEvent deactivateEvent(QEvent::WindowDeactivate);
    QApplication::sendEvent(&scene, &deactivateEvent);

    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());
}

void tst_QGraphicsWidget::boundingRect_data()
{
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null") << QSizeF(0, 0);
    QTest::newRow("avg") << QSizeF(10, 10);
}

// QRectF boundingRect() const public
void tst_QGraphicsWidget::boundingRect()
{
    QFETCH(QSizeF, size);
    SubQGraphicsWidget widget;
    widget.resize(size);
    QCOMPARE(widget.rect(), QRectF(QPointF(), size));
    QCOMPARE(widget.boundingRect(), QRectF(QPointF(0, 0), size));
}

void tst_QGraphicsWidget::dumpFocusChain_data()
{
    QTest::addColumn<bool>("scene");
    QTest::addColumn<int>("children");
    QTest::addColumn<bool>("setFocus");
    QTest::newRow("empty world") << false << 0 << false;
    QTest::newRow("one world") << true << 2 << false;
    QTest::newRow("one world w/focus") << true << 2 << true;
}

// void dumpFocusChain(QGraphicsScene* scene) public (static)
void tst_QGraphicsWidget::dumpFocusChain()
{
    // ### this test is very strange...
    QFETCH(bool, scene);
    SubQGraphicsWidget *parent = new SubQGraphicsWidget;
    QGraphicsScene *theScene = 0;
    if (scene) {
        theScene = new QGraphicsScene(this);
        theScene->addItem(parent);
    }
    QFETCH(int, children);
    QFETCH(bool, setFocus);
    for (int i = 0; i < children; ++i) {
        SubQGraphicsWidget *widget = new SubQGraphicsWidget(parent);
        if (setFocus) {
            widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
            if (scene)
                theScene->setFocusItem(widget);
        }
    }

    if (!scene)
        delete parent;
}

void tst_QGraphicsWidget::focusWidget_data()
{
    QTest::addColumn<int>("childCount");
    QTest::addColumn<int>("childWithFocus");
    QTest::newRow("none") << 0 << 0;
    QTest::newRow("first") << 3 << 0;
    QTest::newRow("last") << 3 << 2;
}

// QGraphicsWidget* focusWidget() const public
void tst_QGraphicsWidget::focusWidget()
{
    SubQGraphicsWidget *parent = new SubQGraphicsWidget;
    QCOMPARE(parent->focusWidget(), (QGraphicsWidget *)0);
    QGraphicsScene scene;
    QEvent windowActivate(QEvent::WindowActivate);
    qApp->sendEvent(&scene, &windowActivate);
    scene.addItem(parent);

    QFETCH(int, childCount);
    QList<SubQGraphicsWidget *> children;
    for (int i = 0; i < childCount; ++i) {
        SubQGraphicsWidget *widget = new SubQGraphicsWidget(parent);
        widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
        children.append(widget);
    }
    if (childCount > 0) {
        QFETCH(int, childWithFocus);
        SubQGraphicsWidget *widget = children[childWithFocus];
        widget->setFocus();
        QTRY_VERIFY(widget->hasFocus());
        QCOMPARE(parent->focusWidget(), static_cast<QGraphicsWidget*>(widget));
    }
}

void tst_QGraphicsWidget::focusWidget2()
{
    QGraphicsScene scene;
    QEvent windowActivate(QEvent::WindowActivate);
    qApp->sendEvent(&scene, &windowActivate);

    QGraphicsWidget *widget = new QGraphicsWidget;
    EventSpy focusInSpy(widget, QEvent::FocusIn);
    EventSpy focusOutSpy(widget, QEvent::FocusOut);

    scene.addItem(widget);

    QTRY_VERIFY(!widget->hasFocus());
    widget->setFocusPolicy(Qt::StrongFocus);
    QTRY_VERIFY(!widget->hasFocus());

    QGraphicsWidget *subWidget = new QGraphicsWidget(widget);
    QTRY_VERIFY(!subWidget->hasFocus());

    scene.setFocus();

    QTRY_VERIFY(!widget->hasFocus());
    QTRY_VERIFY(!subWidget->hasFocus());

    widget->setFocus();

    QTRY_VERIFY(widget->hasFocus());
    QTRY_COMPARE(focusInSpy.count(), 1);
    QTRY_VERIFY(!subWidget->hasFocus());

    QGraphicsWidget *otherSubWidget = new QGraphicsWidget;
    EventSpy otherFocusInSpy(otherSubWidget, QEvent::FocusIn);
    EventSpy otherFocusOutSpy(otherSubWidget, QEvent::FocusOut);

    otherSubWidget->setFocusPolicy(Qt::StrongFocus);
    otherSubWidget->setParentItem(widget);

    QTRY_VERIFY(widget->hasFocus());
    QCOMPARE(scene.focusItem(), (QGraphicsItem *)widget);
    QTRY_VERIFY(!subWidget->hasFocus());
    QTRY_VERIFY(!otherSubWidget->hasFocus());

    widget->hide();
    QTRY_VERIFY(!widget->hasFocus()); // lose but still has subfocus
    QCOMPARE(focusInSpy.count(), 1);
    QCOMPARE(focusOutSpy.count(), 1);

    widget->show();
    QTRY_VERIFY(!widget->hasFocus()); // no regain
    QCOMPARE(focusInSpy.count(), 1);
    QCOMPARE(focusOutSpy.count(), 1);

    widget->hide();

    // try to setup subFocus on item that can't take focus
    subWidget->setFocus();
    QTRY_VERIFY(!subWidget->hasFocus());
    QVERIFY(!scene.focusItem()); // but isn't the scene's focus item

    // try to setup subFocus on item that can take focus
    otherSubWidget->setFocus();
    QTRY_VERIFY(!otherSubWidget->hasFocus());
    QCOMPARE(widget->focusWidget(), otherSubWidget);
    QVERIFY(!scene.focusItem()); // but isn't the scene's focus item

    widget->show();

    QTRY_COMPARE(scene.focusItem(), (QGraphicsItem *)otherSubWidget); // but isn't the scene's focus item
    QCOMPARE(otherFocusInSpy.count(), 1);
    QCOMPARE(otherFocusOutSpy.count(), 0);

    delete otherSubWidget;

    QTRY_COMPARE(otherFocusOutSpy.count(), 1);
    QVERIFY(!scene.focusItem());
    QVERIFY(!widget->focusWidget());
}

class FocusWatchWidget : public QGraphicsWidget
{
public:
    FocusWatchWidget(QGraphicsItem *parent = 0) : QGraphicsWidget(parent) { gotFocusInCount = 0; gotFocusOutCount = 0; }
    int gotFocusInCount, gotFocusOutCount;
protected:
    void focusInEvent(QFocusEvent *fe) { gotFocusInCount++; QGraphicsWidget::focusInEvent(fe); }
    void focusOutEvent(QFocusEvent *fe) { gotFocusOutCount++; QGraphicsWidget::focusOutEvent(fe); }
};

void tst_QGraphicsWidget::focusWidget3()
{
    QGraphicsScene scene;
    QEvent windowActivate(QEvent::WindowActivate);
    qApp->sendEvent(&scene, &windowActivate);

    QGraphicsWidget *widget = new QGraphicsWidget;
    FocusWatchWidget *subWidget = new FocusWatchWidget(widget);
    subWidget->setFocusPolicy(Qt::StrongFocus);

    scene.addItem(widget);
    widget->show();

    QTRY_VERIFY(!widget->hasFocus());
    QTRY_VERIFY(!subWidget->hasFocus());

    subWidget->setFocus();
    QCOMPARE(subWidget->gotFocusInCount, 1);
    QCOMPARE(subWidget->gotFocusOutCount, 0);
    widget->hide();
    QCOMPARE(subWidget->gotFocusOutCount, 1);
}

Q_DECLARE_METATYPE(Qt::FocusPolicy)
void tst_QGraphicsWidget::focusPolicy_data()
{
    QTest::addColumn<Qt::FocusPolicy>("focusPolicy1");
    QTest::addColumn<Qt::FocusPolicy>("focusPolicy2");

    for (int i = 0; i < 25; ++i) {
        QTestData &data = QTest::newRow(QString("%1").arg(i).toLatin1());
        switch(i % 5) {
        case 0: data << Qt::TabFocus; break;
        case 1: data << Qt::ClickFocus; break;
        case 2: data << Qt::StrongFocus; break;
        case 3: data << Qt::WheelFocus; break;
        case 4: data << Qt::NoFocus; break;
        }
        switch(i / 5) {
        case 0: data << Qt::TabFocus; break;
        case 1: data << Qt::ClickFocus; break;
        case 2: data << Qt::StrongFocus; break;
        case 3: data << Qt::WheelFocus; break;
        case 4: data << Qt::NoFocus; break;
        }
    }
}

// Qt::FocusPolicy focusPolicy() const public
void tst_QGraphicsWidget::focusPolicy()
{
    QGraphicsScene scene;
    QEvent windowActivate(QEvent::WindowActivate);
    qApp->sendEvent(&scene, &windowActivate);

    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    scene.addItem(widget);
    QTRY_COMPARE(Qt::NoFocus, widget->focusPolicy());

    QFETCH(Qt::FocusPolicy, focusPolicy1);
    widget->setFocusPolicy(focusPolicy1);
    QTRY_COMPARE(widget->focusPolicy(), focusPolicy1);
    bool isFocusable = widget->flags() & QGraphicsItem::ItemIsFocusable;
    bool wasFocusable = isFocusable;
    QTRY_VERIFY(isFocusable == (focusPolicy1 != Qt::NoFocus));
    widget->setFocus();
    QTRY_COMPARE(widget->hasFocus(), isFocusable);

    QFETCH(Qt::FocusPolicy, focusPolicy2);
    widget->setFocusPolicy(focusPolicy2);
    QCOMPARE(widget->focusPolicy(), focusPolicy2);
    isFocusable = widget->flags() & QGraphicsItem::ItemIsFocusable;
    QVERIFY(isFocusable == (focusPolicy2 != Qt::NoFocus));
    QCOMPARE(widget->hasFocus(), wasFocusable && isFocusable);
}

void tst_QGraphicsWidget::font_data()
{
    QTest::addColumn<QString>("fontName");
    QTest::newRow("Helvetica") << "Helvetica";
}

// QFont font() const public
void tst_QGraphicsWidget::font()
{
    QFETCH(QString, fontName);
    SubQGraphicsWidget widget;
    QCOMPARE(widget.font(), QFont());

    QFont font(fontName);
    widget.setFont(font);
    QCOMPARE(widget.font().family(), font.family());
}

void tst_QGraphicsWidget::fontPropagatesResolveToChildren()
{
    QGraphicsWidget *root = new QGraphicsWidget();
    QGraphicsWidget *child1 = new QGraphicsWidget(root);

    QGraphicsScene scene;
    scene.addItem(root);

    QFont font;
    font.setItalic(true);
    root->setFont(font);

    QGraphicsWidget *child2 = new QGraphicsWidget(root);
    QGraphicsWidget *child3 = new QGraphicsWidget();
    child3->setParentItem(root);

    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QCOMPARE(font.resolve(), uint(QFont::StyleResolved));
    QCOMPARE(root->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child2->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child3->font().resolve(), uint(QFont::StyleResolved));
}

void tst_QGraphicsWidget::fontPropagatesResolveToGrandChildren()
{
    QGraphicsWidget *root = new QGraphicsWidget();
    QGraphicsWidget *child1 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild1 = new QGraphicsWidget(child1);

    QGraphicsScene scene;
    scene.addItem(root);

    QFont font;
    font.setItalic(true);
    root->setFont(font);

    QGraphicsWidget *child2 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild2 = new QGraphicsWidget(child2);
    QGraphicsWidget *grandChild3 = new QGraphicsWidget(child2);

    QGraphicsWidget *child3 = new QGraphicsWidget();
    QGraphicsWidget *grandChild4 = new QGraphicsWidget(child3);
    QGraphicsWidget *grandChild5 = new QGraphicsWidget(child3);
    child3->setParentItem(root);
    grandChild5->setParentItem(child3);

    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QCOMPARE(font.resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild2->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild3->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild4->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild5->font().resolve(), uint(QFont::StyleResolved));
}

void tst_QGraphicsWidget::fontPropagatesResolveViaNonWidget()
{
    QGraphicsWidget *root = new QGraphicsWidget();
    QGraphicsPixmapItem *child1 = new QGraphicsPixmapItem(root);
    QGraphicsWidget *grandChild1 = new QGraphicsWidget(child1);

    QGraphicsScene scene;
    scene.addItem(root);

    QFont font;
    font.setItalic(true);
    root->setFont(font);

    QGraphicsPixmapItem *child2 = new QGraphicsPixmapItem(root);
    QGraphicsWidget *grandChild2 = new QGraphicsWidget(child2);
    QGraphicsWidget *grandChild3 = new QGraphicsWidget(child2);

    QGraphicsPixmapItem *child3 = new QGraphicsPixmapItem();
    QGraphicsWidget *grandChild4 = new QGraphicsWidget(child3);
    QGraphicsWidget *grandChild5 = new QGraphicsWidget(child3);
    child3->setParentItem(root);
    grandChild5->setParentItem(child3);

    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QCOMPARE(font.resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild2->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild3->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild4->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild5->font().resolve(), uint(QFont::StyleResolved));
}

void tst_QGraphicsWidget::fontPropagatesResolveFromScene()
{
    QGraphicsWidget *root = new QGraphicsWidget();
    QGraphicsWidget *child1 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild1 = new QGraphicsWidget(child1);

    QGraphicsScene scene;
    scene.addItem(root);

    QFont font;
    font.setItalic(true);
    scene.setFont(font);

    QGraphicsWidget *child2 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild2 = new QGraphicsWidget(child2);
    QGraphicsWidget *grandChild3 = new QGraphicsWidget(child2);

    QGraphicsWidget *child3 = new QGraphicsWidget();
    QGraphicsWidget *grandChild4 = new QGraphicsWidget(child3);
    QGraphicsWidget *grandChild5 = new QGraphicsWidget(child3);
    child3->setParentItem(root);
    grandChild5->setParentItem(child3);

    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QCOMPARE(font.resolve(), uint(QFont::StyleResolved));
    QCOMPARE(root->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child2->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(child3->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild2->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild3->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild4->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild5->font().resolve(), uint(QFont::StyleResolved));
}

void tst_QGraphicsWidget::fontPropagatesResolveInParentChange()
{
    QGraphicsWidget *root = new QGraphicsWidget();

    QGraphicsWidget *child1 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild1 = new QGraphicsWidget(child1);

    QGraphicsWidget *child2 = new QGraphicsWidget(root);
    QGraphicsWidget *grandChild2 = new QGraphicsWidget(child2);

    QGraphicsScene scene;
    scene.addItem(root);

    QFont italicFont;
    italicFont.setItalic(true);
    child1->setFont(italicFont);

    QFont boldFont;
    boldFont.setBold(true);
    child2->setFont(boldFont);

    QVERIFY(grandChild1->font().italic());
    QVERIFY(!grandChild1->font().bold());
    QVERIFY(!grandChild2->font().italic());
    QVERIFY(grandChild2->font().bold());

    QCOMPARE(grandChild1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild2->font().resolve(), uint(QFont::WeightResolved));

    grandChild2->setParentItem(child1);

    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QVERIFY(grandChild1->font().italic());
    QVERIFY(!grandChild1->font().bold());
    QVERIFY(grandChild2->font().italic());
    QVERIFY(!grandChild2->font().bold());

    QCOMPARE(grandChild1->font().resolve(), uint(QFont::StyleResolved));
    QCOMPARE(grandChild2->font().resolve(), uint(QFont::StyleResolved));

}

void tst_QGraphicsWidget::fontPropagation()
{
    QGraphicsWidget *root = new QGraphicsWidget;
    QGraphicsWidget *child0 = new QGraphicsWidget(root);
    QGraphicsWidget *child1 = new QGraphicsWidget(child0);
    QGraphicsWidget *child2 = new QGraphicsWidget(child1);
    QGraphicsScene scene;
    scene.addItem(root);

    // Check that only the application fonts apply.
    QFont appFont = QApplication::font();
    QCOMPARE(scene.font(), appFont);
    QCOMPARE(root->font(), appFont);
    QCOMPARE(child0->font(), appFont);
    QCOMPARE(child1->font(), appFont);

    // Set child0's Text, and set ToolTipBase on child1.
    QFont boldFont;
    boldFont.setBold(true);
    child0->setFont(boldFont);
    QFont italicFont;
    italicFont.setItalic(true);
    child1->setFont(italicFont);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->font(), appFont);
    QCOMPARE(scene.font(), appFont);
    QVERIFY(child0->font().bold());
    QVERIFY(!child0->font().italic());
    QVERIFY(child1->font().bold());
    QVERIFY(child1->font().italic());
    QVERIFY(child2->font().bold());
    QVERIFY(child2->font().italic());

    QGraphicsWidget *child3 = new QGraphicsWidget(child2);
    QVERIFY(child3->font().bold());
    QVERIFY(child3->font().italic());

    QGraphicsWidget *child4 = new QGraphicsWidget;
    child4->setParentItem(child3);
    QVERIFY(child4->font().bold());
    QVERIFY(child4->font().italic());

    // Replace the app font for child2. Button should propagate but Text
    // should still be ignored. The previous ToolTipBase setting is gone.
    QFont sizeFont;
    sizeFont.setPointSize(43);
    child1->setFont(sizeFont);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->font(), appFont);
    QCOMPARE(scene.font(), appFont);
    QVERIFY(child0->font().bold());
    QVERIFY(!child0->font().italic());
    QVERIFY(child1->font().bold());
    QVERIFY(!child1->font().italic());
    QCOMPARE(child1->font().pointSize(), 43);
    QVERIFY(child2->font().bold());
    QVERIFY(!child2->font().italic());
    QCOMPARE(child2->font().pointSize(), 43);
}

void tst_QGraphicsWidget::fontChangedEvent()
{
    QGraphicsWidget *root = new QGraphicsWidget;
    QGraphicsScene scene;
    scene.addItem(root);

    // Check that only the application fonts apply.
    QFont appFont = QApplication::font();
    QCOMPARE(scene.font(), appFont);
    QCOMPARE(root->font(), appFont);

    EventSpy rootSpyFont(root, QEvent::FontChange);
    EventSpy rootSpyPolish(root, QEvent::Polish);
    QTRY_COMPARE(rootSpyFont.count(), 0);
    QTRY_COMPARE(rootSpyPolish.count(), 1);
    //The font is still the same so no fontChangeEvent
    QTRY_COMPARE(rootSpyFont.count(), 0);

    QFont font;
    font.setPointSize(43);
    root->setFont(font);
    //The font changed
    QTRY_COMPARE(rootSpyFont.count(), 1);

    //then roll back to the default one.
    root->setFont(appFont);
    //The font changed
    QTRY_COMPARE(rootSpyFont.count(), 2);
}

void tst_QGraphicsWidget::fontPropagationWidgetItemWidget()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsRectItem *rect = new QGraphicsRectItem(widget);
    QGraphicsWidget *widget2 = new QGraphicsWidget(rect);

    QGraphicsScene scene;
    scene.addItem(widget);

    QFont font;
    font.setPointSize(43);
    widget->setFont(font);

    QCOMPARE(widget2->font().pointSize(), 43);
    QCOMPARE(widget2->font().resolve(), uint(QFont::SizeResolved));

    widget->setFont(QFont());

    QCOMPARE(widget2->font().pointSize(), qApp->font().pointSize());
    QCOMPARE(widget2->font().resolve(), QFont().resolve());
}

void tst_QGraphicsWidget::fontPropagationSceneChange()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsRectItem *rect = new QGraphicsRectItem(widget);
    QGraphicsWidget *widget2 = new QGraphicsWidget(rect);

    QGraphicsScene scene;
    QGraphicsScene scene2;

    QFont font;
    font.setPointSize(47);
    scene.setFont(font);

    QFont font2;
    font2.setPointSize(74);
    scene2.setFont(font2);

    scene.addItem(widget);
    QCOMPARE(widget2->font().pointSize(), 47);
    scene2.addItem(widget);
    QCOMPARE(widget2->font().pointSize(), 74);
}

void tst_QGraphicsWidget::geometry_data()
{
    QTest::addColumn<QPointF>("pos");
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null, null") << QPointF() << QSizeF(0, 0);
    QTest::newRow("null, normal") << QPointF() << QSizeF(10, 10);
    QTest::newRow("neg, normal") << QPointF(-5, -5) << QSizeF(10, 10);
}

// QRectF geometry() const public
void tst_QGraphicsWidget::geometry()
{
    SubQGraphicsWidget widget;
    QCOMPARE(widget.geometry(), QRectF(widget.pos(), widget.size()));
    QSignalSpy spy(&widget, SIGNAL(geometryChanged()));
    QFETCH(QPointF, pos);
    QFETCH(QSizeF, size);
    widget.setPos(pos);
    widget.resize(size);
    if (!size.isNull() && !pos.isNull())
        QCOMPARE(spy.count(), 2);
    if (!size.isNull() && pos.isNull())
        QCOMPARE(spy.count(), 1);
    QCOMPARE(widget.geometry(), QRectF(pos, size));
}

void tst_QGraphicsWidget::geometryChanged()
{
    QGraphicsWidget w;
    w.setGeometry(0, 0, 200, 200);
    QCOMPARE(w.geometry(), QRectF(0, 0, 200, 200));
    QSignalSpy spy(&w, SIGNAL(geometryChanged()));
    w.setGeometry(0, 0, 100, 100);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(w.geometry(), QRectF(0, 0, 100, 100));
    w.setPos(10, 10);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(w.geometry(), QRectF(10, 10, 100, 100));

}

void tst_QGraphicsWidget::width()
{
    QGraphicsWidget w;
    QCOMPARE(w.property("width").toReal(), qreal(0));
    QSignalSpy spy(&w, SIGNAL(widthChanged()));
    w.setProperty("width", qreal(50));
    QCOMPARE(w.property("width").toReal(), qreal(50));
    QCOMPARE(spy.count(), 1);
    //calling old school setGeometry should work too
    w.setGeometry(0, 0, 200, 200);
    QCOMPARE(spy.count(), 2);
}

void tst_QGraphicsWidget::height()
{
    QGraphicsWidget w;
    QCOMPARE(w.property("height").toReal(), qreal(0));
    QSignalSpy spy(&w, SIGNAL(heightChanged()));
    w.setProperty("height", qreal(50));
    QCOMPARE(w.property("height").toReal(), qreal(50));
    QCOMPARE(spy.count(), 1);
    //calling old school setGeometry should work too
    w.setGeometry(0, 0, 200, 200);
    QCOMPARE(spy.count(), 2);
}

void tst_QGraphicsWidget::getContentsMargins_data()
{
    QTest::addColumn<qreal>("left");
    QTest::addColumn<qreal>("top");
    QTest::addColumn<qreal>("right");
    QTest::addColumn<qreal>("bottom");
    QTest::newRow("null") << (qreal)0 << (qreal)0 << (qreal)0 << (qreal)0;
    QTest::newRow("something") << (qreal)10 << (qreal)5 << (qreal)3 << (qreal)7;
    QTest::newRow("real") << (qreal)1.7 << (qreal)5.9 << (qreal)3.2 << (qreal)9.7;
}

// void getContentsMargins(qreal* left, qreal* top, qreal* right, qreal* bottom) const public
void tst_QGraphicsWidget::getContentsMargins()
{
    qreal gleft;
    qreal gtop;
    qreal gright;
    qreal gbottom;

    SubQGraphicsWidget widget;
    widget.getContentsMargins(&gleft, &gtop, &gright, &gbottom);
    QCOMPARE(gleft, (qreal)0);
    QCOMPARE(gtop, (qreal)0);
    QCOMPARE(gright, (qreal)0);
    QCOMPARE(gbottom, (qreal)0);

    QFETCH(qreal, left);
    QFETCH(qreal, top);
    QFETCH(qreal, right);
    QFETCH(qreal, bottom);
    int oldEventCounts = widget.eventCount;
    widget.setContentsMargins(left, top, right, bottom);
    QVERIFY(left == 0 || oldEventCounts != widget.eventCount);
    widget.getContentsMargins(&gleft, &gtop, &gright, &gbottom);
    QCOMPARE(gleft, left);
    QCOMPARE(gtop, top);
    QCOMPARE(gright, right);
    QCOMPARE(gbottom, bottom);
}

Q_DECLARE_METATYPE(Qt::LayoutDirection)
void tst_QGraphicsWidget::initStyleOption_data()
{
    QTest::addColumn<bool>("enabled");
    QTest::addColumn<bool>("focus");
    QTest::addColumn<bool>("underMouse");
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::addColumn<QSizeF>("size");
    QTest::addColumn<QPalette>("palette");
    QTest::addColumn<QString>("fontName");
    QTest::newRow("none") << false << false << false << Qt::LeftToRight << QSizeF(0, 0) << QPalette() << QString();
    QTest::newRow("all") << true << true << true << Qt::RightToLeft << QSizeF(300, 300) << QPalette(Qt::magenta) << "Helvetica";
    QTest::newRow("rand") << true << false << false << Qt::RightToLeft << QSizeF(1, 0) << QPalette(Qt::darkCyan) << "Times";
}

// void initStyleOption(QStyleOption* option) const public
void tst_QGraphicsWidget::initStyleOption()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);

    view.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    widget->setAcceptsHoverEvents(true);
    QStyleOption option;
    scene.addItem(widget);

    QFETCH(QSizeF, size);
    widget->resize(size);

    QFETCH(bool, enabled);
    widget->setEnabled(enabled);
    QFETCH(bool, focus);
    if (focus) {
        widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
        widget->setFocus();
        QVERIFY(widget->hasFocus());
    }
    QFETCH(bool, underMouse);
    if (underMouse) {
        view.resize(300, 300);
        view.show();
        QTest::qWaitForWindowShown(&view);
        sendMouseMove(view.viewport(), view.mapFromScene(widget->mapToScene(widget->boundingRect().center())));
    }

    QFETCH(QPalette, palette);
    widget->setPalette(palette);

    QFETCH(QString, fontName);
    widget->setFont(QFont(fontName));

    // The test
    widget->initStyleOption(&option);

    bool isEnabled = option.state & QStyle::State_Enabled;
    QCOMPARE(isEnabled, enabled);
    bool hasFocus = option.state & QStyle::State_HasFocus;
    QCOMPARE(hasFocus, focus);
    bool isUnderMouse = option.state & QStyle::State_MouseOver;
#ifndef Q_OS_WINCE
    QCOMPARE(isUnderMouse, underMouse);
#endif
    // if (layoutDirection != Qt::LeftToRight)
    //QEXPECT_FAIL("", "QApplicaiton::layoutDirection doesn't propagate to QGraphicsWidget", Continue);
    //QCOMPARE(option.direction, layoutDirection);
    QCOMPARE(option.rect, QRectF(QPointF(), size).toRect());
    QCOMPARE(option.palette, palette.resolve(QApplication::palette()));
    QCOMPARE(option.fontMetrics, QFontMetrics(widget->font()));
}

void tst_QGraphicsWidget::layout_data()
{
    QTest::addColumn<int>("childCount");
    QTest::newRow("empty") << 0;
    QTest::newRow("10") << 10;
}

// QGraphicsLayout* layout() const public
void tst_QGraphicsWidget::layout()
{
    SubQGraphicsWidget widget;
    widget.setContentsMargins(10, 5, 50, 100);
    QCOMPARE(widget.layout(), (QGraphicsLayout *)0);
    QFETCH(int, childCount);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < childCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget;
        layout->addItem(item);
        children.append(item);
    }
    QSignalSpy spy(&widget, SIGNAL(layoutChanged()));
    widget.setLayout(layout);

    QTRY_COMPARE(widget.layout(), static_cast<QGraphicsLayout*>(layout));
    for (int i = 0; i < children.count(); ++i) {
        SubQGraphicsWidget *item = children[i];
        QCOMPARE(item->parentWidget(), (QGraphicsWidget *)&widget);
        QVERIFY(item->geometry() != QRectF(0, 0, -1, -1));
    }
    QCOMPARE(spy.count(), 1);
    // don't crash
    widget.setLayout(0);
}

void tst_QGraphicsWidget::layoutDirection_data()
{
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::addColumn<bool>("setDirectionBeforeAddingWidget");

    QTest::newRow("rtl, setting direction before adding widget") << Qt::RightToLeft << true;
    QTest::newRow("ltr, setting direction before adding widget") << Qt::LeftToRight << true;
    QTest::newRow("rtl, setting direction after adding widget") << Qt::RightToLeft << false;
    QTest::newRow("ltr, setting direction after adding widget") << Qt::LeftToRight << false;

}

// Qt::LayoutDirection layoutDirection() const public
void tst_QGraphicsWidget::layoutDirection()
{
    QFETCH(Qt::LayoutDirection, layoutDirection);
    QFETCH(bool, setDirectionBeforeAddingWidget);
    QGraphicsScene scene;
    QGraphicsView *view = new QGraphicsView(&scene);
    SubQGraphicsWidget widget;
    scene.addItem(&widget);
    QCOMPARE(widget.layoutDirection(), Qt::LeftToRight);
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), false);

    if (setDirectionBeforeAddingWidget)
        widget.setLayoutDirection(layoutDirection);
    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < 10; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&widget);
        children.append(item);
        QCOMPARE(item->testAttribute(Qt::WA_SetLayoutDirection), false);
    }
    if (!setDirectionBeforeAddingWidget)
        widget.setLayoutDirection(layoutDirection);
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), true);
    view->show();
    QTest::qWaitForWindowShown(view);
    for (int i = 0; i < children.count(); ++i) {
        QTRY_COMPARE(children[i]->layoutDirection(), layoutDirection);
        QTRY_COMPARE(children[i]->testAttribute(Qt::WA_SetLayoutDirection), false);
        view->repaint();
        QTRY_COMPARE(children[i]->m_painterLayoutDirection, layoutDirection);
    }
    delete view;
}

void tst_QGraphicsWidget::recursiveLayoutDirection_data()
{
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::addColumn<bool>("setDirectionBeforeAddingWidget");

    QTest::newRow("rtl, setting direction before adding widget") << Qt::RightToLeft << true;
    QTest::newRow("ltr, setting direction before adding widget") << Qt::LeftToRight << true;
    QTest::newRow("rtl, setting direction after adding widget") << Qt::RightToLeft << false;
    QTest::newRow("ltr, setting direction after adding widget") << Qt::LeftToRight << false;
}

void tst_QGraphicsWidget::recursiveLayoutDirection()
{
    QFETCH(Qt::LayoutDirection, layoutDirection);
    QFETCH(bool, setDirectionBeforeAddingWidget);
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(widget);
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    QGraphicsLinearLayout *layout2 = new QGraphicsLinearLayout(widget2);
    QGraphicsWidget *widget3 = new QGraphicsWidget;
    QGraphicsLinearLayout *layout3 = new QGraphicsLinearLayout(widget3);

    if (setDirectionBeforeAddingWidget)
        widget->setLayoutDirection(layoutDirection);
    layout->addItem(widget2);
    layout2->addItem(widget3);
    if (!setDirectionBeforeAddingWidget)
        widget->setLayoutDirection(layoutDirection);

    QCOMPARE(widget->layoutDirection(), layoutDirection);
    QCOMPARE(widget2->layoutDirection(), layoutDirection);
    QCOMPARE(widget3->layoutDirection(), layoutDirection);

    delete widget;
}

void tst_QGraphicsWidget::paint_data()
{
    // currently QGraphicsWidget doesn't paint or do anything ...
}

// void paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget* widget) public
void tst_QGraphicsWidget::paint()
{
    SubQGraphicsWidget widget;
    QPainter painter;
    QStyleOptionGraphicsItem option;
    widget.paint(&painter, &option, 0); // check that widget = 0 works.
}

void tst_QGraphicsWidget::palettePropagation()
{
    QGraphicsWidget *root = new QGraphicsWidget;
    QGraphicsWidget *child0 = new QGraphicsWidget(root);
    QGraphicsWidget *child1 = new QGraphicsWidget(child0);
    QGraphicsWidget *child2 = new QGraphicsWidget(child1);
    QGraphicsScene scene;
    scene.addItem(root);

    // These colors are unlikely to be imposed on the default palette of
    // QWidget ;-).
    QColor sysPalText(21, 22, 23);
    QColor sysPalToolTipBase(12, 13, 14);
    QColor overridePalText(42, 43, 44);
    QColor overridePalToolTipBase(45, 46, 47);
    QColor sysPalButton(99, 98, 97);

    // Check that only the application fonts apply.
    QPalette appPal = QApplication::palette();
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(root->palette(), appPal);
    QCOMPARE(child0->palette(), appPal);
    QCOMPARE(child1->palette(), appPal);

    // Set child0's Text, and set ToolTipBase on child1.
    QPalette textPalette;
    textPalette.setColor(QPalette::Text, overridePalText);
    child0->setPalette(textPalette);
    QPalette toolTipPalette;
    toolTipPalette.setColor(QPalette::ToolTipBase, overridePalToolTipBase);
    child1->setPalette(toolTipPalette);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->palette(), appPal);
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(child0->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child0->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child1->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);
    QCOMPARE(child2->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child2->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    QGraphicsWidget *child3 = new QGraphicsWidget(child2);
    QCOMPARE(child3->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child3->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    QGraphicsWidget *child4 = new QGraphicsWidget;
    child4->setParentItem(child3);
    QCOMPARE(child4->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child4->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    // Replace the app palette for child2. Button should propagate but Text
    // should still be ignored. The previous ToolTipBase setting is gone.
    QPalette buttonPalette;
    buttonPalette.setColor(QPalette::Button, sysPalButton);
    child1->setPalette(buttonPalette);

    QCOMPARE(root->palette(), appPal);
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(child0->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child0->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child1->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Button), sysPalButton);
    QCOMPARE(child2->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child2->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child2->palette().color(QPalette::Button), sysPalButton);
}

void tst_QGraphicsWidget::parentWidget_data()
{
    QTest::addColumn<int>("childrenCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// QGraphicsWidget* parentWidget() const public
void tst_QGraphicsWidget::parentWidget()
{
    QFETCH(int, childrenCount);
    SubQGraphicsWidget standAlongWidget;
    QGraphicsLineItem standAlongItem;

    SubQGraphicsWidget widgetChild(&standAlongWidget);
    SubQGraphicsWidget itemChild(&standAlongItem);

    QCOMPARE(standAlongWidget.parentWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widgetChild.parentWidget(), static_cast<QGraphicsWidget*>(&standAlongWidget));
    QCOMPARE(itemChild.parentWidget(), (QGraphicsWidget*)0);

    for (int i = 0; i < childrenCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&standAlongWidget);
        QCOMPARE(item->parentWidget(), static_cast<QGraphicsWidget*>(&standAlongWidget));
    }
}

void tst_QGraphicsWidget::resize_data()
{
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null") << QSizeF();
    QTest::newRow("10x10") << QSizeF(10, 10);
    QTest::newRow("10x-1") << QSizeF(10, -1);
}

// void resize(qreal w, qreal h) public
void tst_QGraphicsWidget::resize()
{
    QFETCH(QSizeF, size);
    SubQGraphicsWidget widget;

    int oldEventCounts = widget.eventCount;
    QSizeF oldSize = widget.size();
    widget.resize(size);

    QSizeF boundedSize = size.expandedTo(widget.minimumSize()).boundedTo(widget.maximumSize());
    QCOMPARE(widget.eventCount, oldEventCounts + ((oldSize == boundedSize) ? 0 : 1));
    QCOMPARE(widget.size(), boundedSize);
}

Q_DECLARE_METATYPE(Qt::WidgetAttribute)
void tst_QGraphicsWidget::setAttribute_data()
{
    QTest::addColumn<Qt::WidgetAttribute>("attribute");
    QTest::addColumn<bool>("supported");
    QTest::newRow("WA_SetLayoutDirection") << Qt::WA_SetLayoutDirection << true;
    QTest::newRow("WA_RightToLeft") << Qt::WA_RightToLeft << true;
    QTest::newRow("WA_SetStyle") << Qt::WA_SetStyle << true;
    QTest::newRow("WA_Resized") << Qt::WA_Resized << true;
    QTest::newRow("unsupported") << Qt::WA_PaintOutsidePaintEvent << false;
}

// void setAttribute(Qt::WidgetAttribute attribute, bool on = true) public
void tst_QGraphicsWidget::setAttribute()
{
    QFETCH(Qt::WidgetAttribute, attribute);
    QFETCH(bool, supported);
    SubQGraphicsWidget widget;
    if (attribute == Qt::WA_PaintOutsidePaintEvent)
        QTest::ignoreMessage(QtWarningMsg, "QGraphicsWidget::setAttribute: unsupported attribute 13");
    widget.setAttribute(attribute);
    QCOMPARE(widget.testAttribute(attribute), supported);
}

void tst_QGraphicsWidget::setStyle_data()
{
    QTest::addColumn<QString>("style");
    QTest::newRow("null") << "";
    QTest::newRow("cleanlooks") << "QCleanlooksStyle";
}

// void setStyle(QStyle* style) public
void tst_QGraphicsWidget::setStyle()
{
    SubQGraphicsWidget widget;
    QCleanlooksStyle cleanlooksStyle;

    int oldEventCounts = widget.eventCount;

    QFETCH(QString, style);
    if (style == "QCleanlooksStyle") {
        widget.setStyle(&cleanlooksStyle);
        QCOMPARE(widget.style(), static_cast<QStyle*>(&cleanlooksStyle));
    } else {
        widget.setStyle(0);
        QVERIFY(widget.style() != (QStyle *)0);
    }
    QCOMPARE(widget.eventCount, oldEventCounts + 1);
    QCOMPARE(widget.testAttribute(Qt::WA_SetStyle), !style.isEmpty());

    // cleanup
    widget.setStyle(0);
}

void tst_QGraphicsWidget::setTabOrder_data()
{
    QTest::addColumn<int>("childrenCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// void setTabOrder(QGraphicsWidget* first, QGraphicsWidget* second) public
void tst_QGraphicsWidget::setTabOrder()
{
    QFETCH(int, childrenCount);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);

    QGraphicsWidget *lastItem = 0;
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsWidget::setTabOrder(0, 0) is undefined");
    QGraphicsWidget::setTabOrder(0, 0);

    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < childrenCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget();
        item->setFocusPolicy(Qt::TabFocus);
        children.append(item);
        scene.addItem(item);
        if (lastItem)
            QGraphicsWidget::setTabOrder(lastItem, item);
        lastItem = item;
    }

    if (!children.isEmpty()) {
        QGraphicsWidget *first = children.first();
        view.viewport()->setFocus();
        QTRY_VERIFY(view.viewport()->hasFocus());
        first->setFocus();
        QVERIFY(first->hasFocus());
        QVERIFY(scene.hasFocus());
        QVERIFY(view.viewport()->hasFocus());

        int currentItem = 0;
        while (currentItem < children.count() - 1) {
            QTest::keyPress(view.viewport(), Qt::Key_Tab);
            ++currentItem;
            QVERIFY(children[currentItem % children.size()]->hasFocus());
        }
    }
}

static bool compareFocusChain(QGraphicsView *view, const QList<QGraphicsItem*> &order)
{
    QGraphicsScene *scene = view->scene();
    QStringList actual;
    QGraphicsItem *oldFocusItem = scene->focusItem();
    for (int i = 0; i < order.count(); ++i) {
        QGraphicsItem *focusItem = scene->focusItem();
        actual << focusItem->data(0).toString();
        //qDebug() << "i:" << i << "expected:" << QString::number(uint(order.at(i)), 16) << QString::number(uint(focusItem), 16);
        if (focusItem != order.at(i)) {
            qDebug() << "actual:"  << actual;
            scene->setFocusItem(oldFocusItem);
            return false;
        }
        if (i < order.count() - 1)
            QTest::keyPress(view, Qt::Key_Tab);
    }
    scene->setFocusItem(oldFocusItem);
    return true;
}

void tst_QGraphicsWidget::setTabOrderAndReparent()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);

    int i;
    QGraphicsWidget *w1, *w2, *w3, *w4;
    for (i = 1; i < 4; ++i) {
        QGraphicsWidget *wid = new QGraphicsWidget;
        wid->setFocusPolicy(Qt::StrongFocus);
        wid->setData(0, QString::fromAscii("w%1").arg(i));
        scene.addItem(wid);
        if (i == 1)
            w1 = wid;
        else if (i == 2)
            w2 = wid;
        else if (i == 3)
            w3 = wid;
    }

    w1->setFocus();
    QTRY_VERIFY(w1->hasFocus());
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w2 << w3));

    QGraphicsWidget *p = new QGraphicsWidget;
    p->setData(0, QLatin1String("parent"));
    p->setFocusPolicy(Qt::StrongFocus);

    w1->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w2 << w3));

    w1->setParentItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() <<  w2 << w3));

    w2->setParentItem(p);
    w3->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w3));
    w3->setParentItem(p);
    QCOMPARE(scene.focusItem(), static_cast<QGraphicsItem*>(0));

    scene.addItem(p);
    p->setFocus();

    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << p << w1 << w2 << w3));
    delete p;

    for (i = 1; i < 5; ++i) {
        QGraphicsWidget *wid = new QGraphicsWidget;
        wid->setFocusPolicy(Qt::StrongFocus);
        wid->setData(0, QString::fromAscii("w%1").arg(i));
        scene.addItem(wid);
        if (i == 1)
            w1 = wid;
        else if (i == 2)
            w2 = wid;
        else if (i == 3)
            w3 = wid;
        else if (i == 4)
            w4 = wid;
    }
    w4->setParentItem(w1);
    QGraphicsWidget::setTabOrder(w1, w4);
    w1->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w4 << w2 << w3));

    p = new QGraphicsWidget;
    p->setData(0, QLatin1String("parent"));
    p->setFocusPolicy(Qt::StrongFocus);

    w1->setParentItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w2 << w3));

    scene.addItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w2 << w3 << p << w1 << w4));
}

void tst_QGraphicsWidget::topLevelWidget_data()
{
    QTest::addColumn<QString>("str");
    QTest::newRow("test one") << "foo";
}

// QGraphicsWidget* topLevelWidget() const public
void tst_QGraphicsWidget::topLevelWidget()
{
    QFETCH(QString, str);
    SubQGraphicsWidget widget;
    QCOMPARE(widget.topLevelWidget(), (QGraphicsWidget *)&widget);
}

void tst_QGraphicsWidget::unsetLayoutDirection_data()
{
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::newRow("rtl") << Qt::RightToLeft;
    QTest::newRow("ltr") << Qt::LeftToRight;
}

// void unsetLayoutDirection() public
void tst_QGraphicsWidget::unsetLayoutDirection()
{
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QFETCH(Qt::LayoutDirection, layoutDirection);
    SubQGraphicsWidget widget;
    QCOMPARE(Qt::LeftToRight, widget.layoutDirection());

    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < 10; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&widget);
        children.append(item);
    }
    widget.setLayoutDirection(layoutDirection);
    widget.unsetLayoutDirection();
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), false);
    for (int i = 0; i < children.count(); ++i) {
        QCOMPARE(children[i]->layoutDirection(), Qt::LeftToRight);
    }
}

void tst_QGraphicsWidget::focusNextPrevChild_data()
{
    QTest::addColumn<QString>("str");
    QTest::newRow("test one") << "foo";
}

// bool focusNextPrevChild(bool next) protected
void tst_QGraphicsWidget::focusNextPrevChild()
{
    QFETCH(QString, str);
    SubQGraphicsWidget widget;
    // ### write test after just calling it stops crashing :)
    widget.call_focusNextPrevChild(true);
}

void tst_QGraphicsWidget::verifyFocusChain()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);

    {
        // parent/child focus
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        w->setFocusPolicy(Qt::StrongFocus);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w->setFocus();
        QVERIFY(w->hasFocus());
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_1->hasFocus());
        delete w;
    }

    {
        // delete item in focus chain and verify chain
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        SubQGraphicsWidget *w1_2 = new SubQGraphicsWidget(w);
        SubQGraphicsWidget *w1_3 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        w1_2->setFocusPolicy(Qt::StrongFocus);
        w1_3->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w1_1->setFocus();
        QVERIFY(w1_1->hasFocus());
        QCOMPARE(w->call_focusNextPrevChild(true), true);
        QVERIFY(w1_2->hasFocus());
        QCOMPARE(w->call_focusNextPrevChild(true), true);
        QVERIFY(w1_3->hasFocus());
        w1_1->setFocus();
        delete w1_2;
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_3->hasFocus());
        delete w;
    }
    {
        // parent/child focus
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        w->setFocusPolicy(Qt::StrongFocus);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w->setFocus();
        QVERIFY(w->hasFocus());
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_1->hasFocus());
        delete w;
    }
    {
        // remove the tabFocusFirst widget from the scene.
        QWidget *window = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;
        window->setLayout(layout);
        QLineEdit *lineEdit = new QLineEdit;
        layout->addWidget(lineEdit);
        QGraphicsView *view = new QGraphicsView(&scene);
        scene.setSceneRect(-20, -20, 200, 50);
        layout->addWidget(view);
        view->setMinimumSize(150, 50);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget;
        w1_1->setData(0, "w1_1");
        w1_1->setGeometry(0,0,25, 25);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w1_1);
        SubQGraphicsWidget *w1_2 = new SubQGraphicsWidget;
        w1_2->setData(0, "w1_2");
        w1_2->setGeometry(25,0,25, 25);
        w1_2->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w1_2);
        window->show();
        QApplication::setActiveWindow(window);
        QTest::qWaitForWindowShown(window);

        lineEdit->setFocus();
        QTRY_VERIFY(lineEdit->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTRY_VERIFY(w1_1->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTRY_VERIFY(w1_2->hasFocus());

        // remove the tabFocusFirst and insert new item
        delete w1_1;            // calls _q_removeItemLater
        SubQGraphicsWidget *w1_3 = new SubQGraphicsWidget;
        w1_3->setFocusPolicy(Qt::StrongFocus);
        w1_3->setData(0, "w1_3");
        w1_3->setGeometry(50,0,25, 25);
        scene.addItem(w1_3);
        QTRY_VERIFY(w1_2->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Backtab);
        QTRY_VERIFY(lineEdit->hasFocus());
        // tabFocusFirst should now point to w1_2
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTRY_VERIFY(w1_2->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTRY_VERIFY(w1_3->hasFocus());
        scene.removeItem(w1_2);   // does not call _q_removeItemLater
        delete w1_2;            // calls _q_removeItemLater

        SubQGraphicsWidget *w1_4 = new SubQGraphicsWidget;
        w1_4->setFocusPolicy(Qt::StrongFocus);
        w1_4->setData(0, "w1_4");
        w1_4->setGeometry(75,0,25, 25);
        scene.addItem(w1_4);
        QTRY_VERIFY(w1_3->hasFocus());
        QTRY_VERIFY(compareFocusChain(view, QList<QGraphicsItem*>() << w1_3 << w1_4));
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Backtab);
        QTRY_VERIFY(lineEdit->hasFocus());
        // tabFocusFirst should now point to w1_3
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTRY_VERIFY(w1_3->hasFocus());
        QTRY_VERIFY(compareFocusChain(view, QList<QGraphicsItem*>() << w1_3 << w1_4));
        delete window;
    }
}

void tst_QGraphicsWidget::updateFocusChainWhenChildDie()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);

    // delete item in focus chain with no focus and verify chain
    SubQGraphicsWidget *parent = new SubQGraphicsWidget(0, Qt::Window);
    SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
    w->resize(50,50);
    w->resize(100,100);
    SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
    w1_1->setFocusPolicy(Qt::StrongFocus);
    w->setFocusPolicy(Qt::StrongFocus);
    scene.addItem(w);
    scene.addItem(parent);
    w1_1->setFocus();

    QVERIFY(w1_1->hasFocus());
    QWidget myWidget(0);
    QLineEdit edit(&myWidget);
    myWidget.show();
    edit.setFocus();
    QTRY_VERIFY(edit.hasFocus());
    delete w1_1;
    myWidget.hide();
    w->setParentItem(parent);
    //We don't crash perfect
    QVERIFY(w);
    QTest::mouseMove(view.viewport());
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0);
    QTRY_COMPARE(qApp->activeWindow(), static_cast<QWidget *>(&view));
    QTRY_COMPARE(scene.focusItem(), static_cast<QGraphicsItem *>(w));
}

void tst_QGraphicsWidget::sizeHint_data()
{
    QTest::addColumn<bool>("layout");
    QTest::newRow("no layout") << false;
    QTest::newRow("layout") << true;
}

// QSizeF sizeHint(Qt::SizeHint which, QSizeF const& constraint = QSizeF()) const protected
void tst_QGraphicsWidget::sizeHint()
{
    QFETCH(bool, layout);
    SubQGraphicsWidget widget;

    if (layout) {
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
        widget.setLayout(layout);
    }
    widget.call_sizeHint(Qt::MinimumSize, QSizeF());
}

void tst_QGraphicsWidget::consistentPosSizeGeometry_data()
{
    QTest::addColumn<QSizeF>("minSize");
    QTest::addColumn<QSizeF>("maxSize");
    QTest::addColumn<QRectF>("geometry");
    QTest::addColumn<QRectF>("expectedGeometry");

    QTest::newRow("size is valid") << QSizeF(0, 0) << QSizeF(200, 200) << QRectF(0, 0, 100, 100) << QRectF(0, 0, 100, 100);
    QTest::newRow("size is larger than max") << QSizeF(0, 0) << QSizeF(50, 50) << QRectF(0, 0, 100, 100) << QRectF(0, 0, 50, 50);
    QTest::newRow("size is smaller than min") << QSizeF(50, 50) << QSizeF(150, 150) << QRectF(0, 0, 10, 10) << QRectF(0, 0, 50, 50);
}

void tst_QGraphicsWidget::consistentPosSizeGeometry()
{
    QFETCH(QSizeF, minSize);
    QFETCH(QSizeF, maxSize);
    QFETCH(QRectF, geometry);
    QFETCH(QRectF, expectedGeometry);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *w = new QGraphicsWidget;
    scene.addItem(w);
    w->setMinimumSize(minSize);
    w->setMaximumSize(maxSize);
    w->setGeometry(geometry);
    QCOMPARE(w->geometry(), expectedGeometry);
    QCOMPARE(w->pos(), expectedGeometry.topLeft());
    QCOMPARE(w->size(), expectedGeometry.size());

    QRectF otherGeom = QRectF(QPointF(12.34,12.34), minSize);
    w->setGeometry(otherGeom);
    QCOMPARE(w->geometry(), otherGeom);
    QCOMPARE(w->pos(), otherGeom.topLeft());
    QCOMPARE(w->size(), otherGeom.size());

    w->setPos(geometry.topLeft());
    QCOMPARE(w->geometry().topLeft(), expectedGeometry.topLeft());
    QCOMPARE(w->pos(), expectedGeometry.topLeft());

    w->resize(geometry.size());
    QCOMPARE(w->geometry().size(), expectedGeometry.size());
    QCOMPARE(w->geometry(), expectedGeometry);

    view.show();

}


enum WhichSize {
    MinimumWidth,
    PreferredWidth,
    MaximumWidth,
    MinimumHeight,
    PreferredHeight,
    MaximumHeight,
    MinimumSize,
    PreferredSize,
    MaximumSize,
    MinimumSizeHint,
    PreferredSizeHint,
    MaximumSizeHint,
    Size,
    None,
};

typedef QPair<int, QVariant> Inst;

Q_DECLARE_METATYPE(Inst)
Q_DECLARE_METATYPE(QVector<Inst>)

void tst_QGraphicsWidget::setSizes_data()
{

    QTest::addColumn<QVector<Inst> >("inputInstructions");
    QTest::addColumn<QVector<Inst> >("compareInstructions");

    QTest::newRow("minSize1") << (QVector<Inst>() << Inst(Size, QSize(25, 25)) << Inst(MinimumSize, QSize(10, 10)))
                                << (QVector<Inst>() << Inst(Size, QSize(25,25)));
    QTest::newRow("minSize2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumSize, QSizeF(25, 25)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(25, 25)));
    QTest::newRow("minWidth1") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumWidth, 5.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)));
    QTest::newRow("minWidth2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumWidth, 25.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(25, 20)));
    QTest::newRow("minHeight1") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumHeight, 5.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)));
    QTest::newRow("minHeight2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumHeight, 25.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 25)));
    QTest::newRow("maxSize1") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(30, 30)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 30)));
    QTest::newRow("maxSize2") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(30, -1)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 40)));
    QTest::newRow("maxSize3") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(-1, 30)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(40, 30)));
    QTest::newRow("maxWidth1")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumWidth, 30))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 40)));
    QTest::newRow("maxHeight")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumHeight, 20))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(40, 20)));
    QTest::newRow("unsetMinSize")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MinimumSize, QSizeF(-1, -1)))
                                 << (QVector<Inst>() << Inst(MinimumSize, QSizeF(5, 5)));
    QTest::newRow("unsetMaxSize")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(-1, -1)))
                                 << (QVector<Inst>() << Inst(MaximumSize, QSizeF(500, 500)));
    QTest::newRow("unsetMinSize, expand size to minimumSizeHint") << (QVector<Inst>()
                                        << Inst(MinimumSize, QSize(0, 0))
                                        << Inst(Size, QSize(1,1))
                                        << Inst(MinimumSize, QSize(-1.0, -1.0))
                                        )
                                    << (QVector<Inst>()
                                        << Inst(Size, QSize(5,5))
                                        << Inst(MinimumSize, QSize(5,5))
                                        );

}

void tst_QGraphicsWidget::setSizes()
{
    QFETCH(QVector<Inst>, inputInstructions);
    QFETCH(QVector<Inst>, compareInstructions);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    SizeHinter *widget = new SizeHinter(0, Qt::Window);
    QSizeF min = QSizeF(10, 10);
    QSizeF pref = QSizeF(25, 25);
    QSizeF max = QSizeF(50, 50);

    int i;
    for (i = 0; i < inputInstructions.count(); ++i) {
        Inst input = inputInstructions.at(i);

        // defaults
        switch (input.first) {
            case MinimumSize:
                min = input.second.toSizeF();
                break;
            case PreferredSize:
                pref = input.second.toSizeF();
                break;
            case MaximumSize:
                max = input.second.toSizeF();
                break;
            case Size:
                widget->resize(input.second.toSizeF());
                break;
            case MinimumWidth:
                widget->setMinimumWidth(qreal(input.second.toDouble()));
                break;
            case PreferredWidth:
                widget->setPreferredWidth(qreal(input.second.toDouble()));
                break;
            case MaximumWidth:
                widget->setMaximumWidth(qreal(input.second.toDouble()));
                break;
            case MinimumHeight:
                widget->setMinimumHeight(qreal(input.second.toDouble()));
                break;
            case PreferredHeight:
                widget->setPreferredHeight(qreal(input.second.toDouble()));
                break;
            case MaximumHeight:
                widget->setMaximumHeight(qreal(input.second.toDouble()));
                break;
            case MinimumSizeHint:
                widget->setSizeHint(Qt::MinimumSize, input.second.toSizeF());
                break;
            case PreferredSizeHint:
                widget->setSizeHint(Qt::PreferredSize, input.second.toSizeF());
                break;
            case MaximumSizeHint:
                widget->setSizeHint(Qt::MaximumSize, input.second.toSizeF());
                break;
            default:
                qWarning("instruction not implemented");
                break;
        }
    }

    widget->setMinimumSize(min);
    widget->setPreferredSize(pref);
    widget->setMaximumSize(max);

    for (i = 0; i < compareInstructions.count(); ++i) {
        Inst input = compareInstructions.at(i);
        switch (input.first) {
            case MinimumSize:
                QTRY_COMPARE(widget->minimumSize(), input.second.toSizeF());
                break;
            case PreferredSize:
                QTRY_COMPARE(widget->preferredSize(), input.second.toSizeF());
                break;
            case MaximumSize:
                QTRY_COMPARE(widget->maximumSize(), input.second.toSizeF());
                break;
            case Size:
                QTRY_COMPARE(widget->size(), input.second.toSizeF());
                break;
            case MinimumWidth:
                QTRY_COMPARE(widget->minimumWidth(), qreal(input.second.toDouble()));
                break;
            case PreferredWidth:
                QTRY_COMPARE(widget->preferredWidth(), qreal(input.second.toDouble()));
                break;
            case MaximumWidth:
                QTRY_COMPARE(widget->maximumWidth(), qreal(input.second.toDouble()));
                break;
            default:
                qWarning("instruction not implemented");
                break;
        }
    }
    delete widget;
}

void tst_QGraphicsWidget::closePopupOnOutsideClick()
{
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Popup);
    widget->resize(100, 100);

    QGraphicsScene scene;
    scene.addItem(widget);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.ignore();
    event.setScenePos(QPointF(50, 50));
    qApp->sendEvent(&scene, &event);

    QVERIFY(widget->isVisible());
    QVERIFY(event.isAccepted());

    event.ignore();
    event.setScenePos(QPointF(150, 150));
    qApp->sendEvent(&scene, &event);

    QVERIFY(!widget->isVisible());
    QVERIFY(event.isAccepted());
}

void tst_QGraphicsWidget::task236127_bspTreeIndexFails()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    widget->resize(10, 10);
    widget2->resize(10, 10);
    widget2->setZValue(1);
    QCOMPARE(widget2->zValue(), qreal(1));
    QCOMPARE(widget->zValue(), qreal(0));
    widget->setData(0, "widget");
    widget2->setData(0, "widget2");

    QGraphicsScene scene;
    scene.addItem(widget);
    scene.addItem(widget2);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QTRY_VERIFY(!scene.itemAt(25, 25));
    widget->setGeometry(0, 112, 360, 528);
    QTRY_COMPARE(scene.itemAt(15, 120), (QGraphicsItem *)widget);
    widget2->setGeometry(0, 573, 360, 67);
    QTRY_COMPARE(scene.itemAt(15, 120), (QGraphicsItem *)widget);
    QTRY_COMPARE(scene.itemAt(50, 585), (QGraphicsItem *)widget2);
}

void tst_QGraphicsWidget::defaultSize()
{
    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    widget->setMinimumSize(40, 40);
    QGraphicsScene scene;
    scene.addItem(widget);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QSizeF initialSize = widget->size();

    widget->resize(initialSize);
    QCOMPARE(widget->geometry().size(), initialSize);
    widget->setVisible(false);
    widget->setMinimumSize(10, 10);
    widget->setPreferredSize(60, 60);
    widget->setMaximumSize(110, 110);
    widget->setVisible(true);
    // should still have its size set to initialsize
    QTRY_COMPARE(widget->geometry().size(), initialSize);

}

void tst_QGraphicsWidget::explicitMouseGrabber()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    EventSpy widgetGrabEventSpy(widget, QEvent::GrabMouse);
    EventSpy widgetUngrabEventSpy(widget, QEvent::UngrabMouse);

    // Grab without scene
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsItem::grabMouse: cannot grab mouse without scene");
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 0);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsItem::ungrabMouse: cannot ungrab mouse without scene");
    widget->ungrabMouse();
    QCOMPARE(widgetUngrabEventSpy.count(), 0);

    // Add to scene
    QGraphicsScene scene;
    scene.addItem(widget);

    // Ungrab while not grabber
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsItem::ungrabMouse: not a mouse grabber");
    widget->ungrabMouse();

    // Simple grab with scene
    QVERIFY(!scene.mouseGrabberItem());
    widget->grabMouse();
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 1);
    widget->ungrabMouse();
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QCOMPARE(widgetUngrabEventSpy.count(), 1);

    // Grab while grabbing
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsItem::grabMouse: already a mouse grabber");
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    QCOMPARE(widgetUngrabEventSpy.count(), 1);
    widget->ungrabMouse();
    QCOMPARE(widgetUngrabEventSpy.count(), 2);

    // Add two more widgets to the scene
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    scene.addItem(widget2);
    EventSpy widget2GrabEventSpy(widget2, QEvent::GrabMouse);
    EventSpy widget2UngrabEventSpy(widget2, QEvent::UngrabMouse);
    QGraphicsWidget *widget3 = new QGraphicsWidget;
    scene.addItem(widget3);
    EventSpy widget3GrabEventSpy(widget3, QEvent::GrabMouse);
    EventSpy widget3UngrabEventSpy(widget3, QEvent::UngrabMouse);

    widget->setData(0, "widget");
    widget2->setData(0, "widget2");
    widget3->setData(0, "widget3");

    // Simple nested grabbing
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 3);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    widget2->grabMouse();
    QCOMPARE(widgetUngrabEventSpy.count(), 3);
    QCOMPARE(widget2GrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);
    widget3->grabMouse();
    QCOMPARE(widget2UngrabEventSpy.count(), 1);
    QCOMPARE(widget3GrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget3);
    widget3->ungrabMouse();
    QCOMPARE(widget3UngrabEventSpy.count(), 1);
    QCOMPARE(widget2GrabEventSpy.count(), 2);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);
    widget2->ungrabMouse();
    QCOMPARE(widget2UngrabEventSpy.count(), 2);
    QCOMPARE(widgetGrabEventSpy.count(), 4);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    widget->ungrabMouse();
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);

    // Out of order ungrab
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 5);
    widget2->grabMouse();
    QCOMPARE(widget2GrabEventSpy.count(), 3);
    widget3->grabMouse();
    QCOMPARE(widget3GrabEventSpy.count(), 2);
    widget2->ungrabMouse();
    QCOMPARE(widget3UngrabEventSpy.count(), 2);
    QCOMPARE(widget2UngrabEventSpy.count(), 4);
    QCOMPARE(widgetGrabEventSpy.count(), 6);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
}

void tst_QGraphicsWidget::implicitMouseGrabber()
{
    QGraphicsScene scene;
    QGraphicsWidget *widget = new QGraphicsWidget;
    widget->setFlag(QGraphicsItem::ItemIsMovable); // can grab mouse
    widget->resize(200, 200);
    EventSpy widgetGrabEventSpy(widget, QEvent::GrabMouse);
    EventSpy widgetUngrabEventSpy(widget, QEvent::UngrabMouse);
    scene.addItem(widget);

    QVERIFY(!scene.mouseGrabberItem());

    // Click on an item, see if gain and lose implicit mouse grab.
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 1);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QCOMPARE(widgetGrabEventSpy.count(), 1);
    QCOMPARE(widgetUngrabEventSpy.count(), 1);

    // Click on an item that already grabs the mouse. Shouldn't have any effect.
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    QCOMPARE(widgetUngrabEventSpy.count(), 1);
    widget->ungrabMouse();
    QCOMPARE(widgetUngrabEventSpy.count(), 2);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);

    // Implicit mouse grabber tries to explicitly grab the mouse
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 3);
    widget->grabMouse();
    QCOMPARE(widgetUngrabEventSpy.count(), 2);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 3);
    QCOMPARE(widgetUngrabEventSpy.count(), 2);
    widget->ungrabMouse();
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QCOMPARE(widgetGrabEventSpy.count(), 3);
    QCOMPARE(widgetUngrabEventSpy.count(), 3);

    // Arrival of a new widget
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    widget2->setFlag(QGraphicsItem::ItemIsMovable); // can grab mouse
    widget2->resize(200, 200);
    widget2->setPos(205, 0);
    EventSpy widget2GrabEventSpy(widget2, QEvent::GrabMouse);
    EventSpy widget2UngrabEventSpy(widget2, QEvent::UngrabMouse);
    scene.addItem(widget2);

    // Implicit grab while there's an explicit grab is not possible.
    widget->grabMouse();
    QCOMPARE(widgetGrabEventSpy.count(), 4);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(250, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);
    QCOMPARE(widgetGrabEventSpy.count(), 4);
    QCOMPARE(widget2GrabEventSpy.count(), 0);
    QCOMPARE(widget2UngrabEventSpy.count(), 0);

    scene.removeItem(widget);
    QCOMPARE(widgetUngrabEventSpy.count(), 4);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
}

class GrabOnPressItem : public QGraphicsRectItem
{
public:
    GrabOnPressItem(const QRectF &rect)
        : QGraphicsRectItem(rect),
          npress(0), nrelease(0), ndoubleClick(0),
          ngrab(0), nungrab(0)
    {
    }
    int npress;
    int nrelease;
    int ndoubleClick;
    int ngrab;
    int nungrab;
protected:
    bool sceneEvent(QEvent *event)
    {
        switch (event->type()) {
        case QEvent::GrabMouse:
            ++ngrab;
            break;
        case QEvent::UngrabMouse:
            ++nungrab;
            break;
        default:
            break;
        }
        return QGraphicsRectItem::sceneEvent(event);
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *)
    {
        grabMouse();
        ++npress;
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
    {
        ungrabMouse();
        ++nrelease;
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
    {
        ++ndoubleClick;
    }
};

void tst_QGraphicsWidget::doubleClickAfterExplicitMouseGrab()
{
    QGraphicsScene scene;
    GrabOnPressItem *item = new GrabOnPressItem(QRectF(0, 0, 100, 100));
    scene.addItem(item);

    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        event.ignore();
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
    QCOMPARE(item->npress, 1);
    QCOMPARE(item->ngrab, 1);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.setButton(Qt::LeftButton);
        event.setButtons(0);
        event.ignore();
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QCOMPARE(item->nrelease, 1);
    QCOMPARE(item->nungrab, 1);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseDoubleClick);
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        event.ignore();
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
    QCOMPARE(item->ndoubleClick, 1);
    QCOMPARE(item->ngrab, 2);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.setButton(Qt::LeftButton);
        event.setButtons(0);
        event.ignore();
        event.setScenePos(QPointF(50, 50));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QCOMPARE(item->nrelease, 2);
    QCOMPARE(item->nungrab, 2);
}

void tst_QGraphicsWidget::popupMouseGrabber()
{
    QGraphicsScene scene;
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Popup);
    widget->setFlag(QGraphicsItem::ItemIsMovable); // can grab mouse
    widget->resize(200, 200);
    EventSpy widgetGrabEventSpy(widget, QEvent::GrabMouse);
    EventSpy widgetUngrabEventSpy(widget, QEvent::UngrabMouse);

    // Simply adding a visible popup to the scene immediately grabs the mouse.
    scene.addItem(widget);
    QCOMPARE(widgetGrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);

    // Hiding it loses the grab again.
    widget->hide();
    QCOMPARE(widgetUngrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);

    // Showing it grabs the mouse again
    widget->show();
    QCOMPARE(widgetGrabEventSpy.count(), 2);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget);

    // Add two popups
    QGraphicsWidget *widget2 = new QGraphicsWidget(0, Qt::Popup);
    widget2->setFlag(QGraphicsItem::ItemIsMovable); // can grab mouse
    widget2->resize(200, 200);
    EventSpy widget2GrabEventSpy(widget2, QEvent::GrabMouse);
    EventSpy widget2UngrabEventSpy(widget2, QEvent::UngrabMouse);
    QGraphicsWidget *widget3 = new QGraphicsWidget(0, Qt::Popup);
    widget3->setFlag(QGraphicsItem::ItemIsMovable); // can grab mouse
    widget3->resize(200, 200);
    EventSpy widget3GrabEventSpy(widget3, QEvent::GrabMouse);
    EventSpy widget3UngrabEventSpy(widget3, QEvent::UngrabMouse);

    // Adding to the scene grabs
    scene.addItem(widget2);
    QCOMPARE(widgetUngrabEventSpy.count(), 2);
    QCOMPARE(widget2GrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);

    // Adding to the scene grabs again
    scene.addItem(widget3);
    QCOMPARE(widget2UngrabEventSpy.count(), 1);
    QCOMPARE(widget3GrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget3);

    // Hiding the topmost widget causes widget 2 to regain grab.
    widget3->hide();
    QCOMPARE(widget2GrabEventSpy.count(), 2);
    QCOMPARE(widget3UngrabEventSpy.count(), 1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);
    widget3->show();
    QCOMPARE(widget2UngrabEventSpy.count(), 2);
    QCOMPARE(widget3GrabEventSpy.count(), 2);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget3);

    // Clicking outside the popup still causes it to close (despite that it's
    // an explicit mouse grabber).
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.ignore();
        event.setButton(Qt::LeftButton);
        event.setScenePos(QPointF(500, 500)); // outside
        qApp->sendEvent(&scene, &event);
    }
    QVERIFY(!widget3->isVisible());
    QCOMPARE(widget3UngrabEventSpy.count(), 2);
    QCOMPARE(widget2GrabEventSpy.count(), 3);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);
    QVERIFY(widget2->isVisible());
    QVERIFY(widget->isVisible());
    widget3->show();
    QCOMPARE(widget3GrabEventSpy.count(), 3);
    QCOMPARE(widget2UngrabEventSpy.count(), 3);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget3);

    // This is something of a curiosity. What happens if you call
    // ungrabMouse() on a popup? The answer is - it loses the grab. If you
    // hide and show the popup again, it will regain the grab.
    widget3->ungrabMouse();
    QCOMPARE(widget3UngrabEventSpy.count(), 3);
    QCOMPARE(widget2GrabEventSpy.count(), 4);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget2);
    widget3->hide();
    widget3->show();
    QCOMPARE(widget3GrabEventSpy.count(), 4);
    QCOMPARE(widget2UngrabEventSpy.count(), 4);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)widget3);
}

void tst_QGraphicsWidget::windowFlags_data()
{
    QTest::addColumn<int>("inputFlags");
    QTest::addColumn<int>("outputFlags");

    QTest::newRow("nil") << 0 << 0;

    // Window types
    QTest::newRow("Qt::Window") << int(Qt::Window)
                                << int(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                                       | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QTest::newRow("Qt::SubWindow") << int(Qt::SubWindow)
                                   << int(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                                          | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QTest::newRow("Qt::Dialog") << int(Qt::Dialog)
                                << int(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                                       | Qt::WindowContextHelpButtonHint);
    QTest::newRow("Qt::Sheet") << int(Qt::Sheet)
                               << int(Qt::Sheet | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                                      | Qt::WindowContextHelpButtonHint);
    QTest::newRow("Qt::Tool") << int(Qt::Tool)
                              << int(Qt::Tool | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // Custom window flags
    QTest::newRow("Qt::FramelessWindowHint") << int(Qt::FramelessWindowHint)
                                             << int(Qt::FramelessWindowHint);
    QTest::newRow("Qt::CustomizeWindowHint") << int(Qt::CustomizeWindowHint)
                                             << int(Qt::CustomizeWindowHint);
}

void tst_QGraphicsWidget::windowFlags()
{
    QFETCH(int, inputFlags);
    QFETCH(int, outputFlags);

    // Construct with flags set already
    QGraphicsWidget widget(0, Qt::WindowFlags(inputFlags));
    QCOMPARE(widget.windowFlags(), Qt::WindowFlags(outputFlags));

    // Set flags after construction
    QGraphicsWidget widget2;
    widget2.setWindowFlags(Qt::WindowFlags(inputFlags));
    QCOMPARE(widget2.windowFlags(), Qt::WindowFlags(outputFlags));

    // Reset flags
    widget2.setWindowFlags(0);
    QVERIFY(!widget2.windowFlags());

    // Set flags back again
    widget2.setWindowFlags(Qt::WindowFlags(inputFlags));
    QCOMPARE(widget2.windowFlags(), Qt::WindowFlags(outputFlags));

    // Construct with custom flags set already
    QGraphicsWidget widget3(0, Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));
    QCOMPARE(widget3.windowFlags(), Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));

    // Set custom flags after construction
    QGraphicsWidget widget4;
    widget4.setWindowFlags(Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));
    QCOMPARE(widget4.windowFlags(), Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));

    // Reset flags
    widget4.setWindowFlags(0);
    QVERIFY(!widget4.windowFlags());

    // Set custom flags back again
    widget4.setWindowFlags(Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));
    QCOMPARE(widget4.windowFlags(), Qt::WindowFlags(inputFlags | Qt::FramelessWindowHint));

    QGraphicsWidget *widget5 = new QGraphicsWidget;
    widget5->setWindowFlags(Qt::WindowFlags(inputFlags));
    QCOMPARE(widget5->windowFlags(), Qt::WindowFlags(outputFlags));
    QGraphicsWidget window(0, Qt::Window);
    widget5->setParentItem(&window);
    QCOMPARE(widget5->windowFlags(), Qt::WindowFlags(outputFlags));
}

void tst_QGraphicsWidget::shortcutsDeletion()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    widget->setMinimumSize(40, 40);
    QWidgetAction *del = new QWidgetAction(widget);
    del->setIcon(QIcon("edit-delete"));
    del->setShortcut(Qt::Key_Delete);
    del->setShortcutContext(Qt::WidgetShortcut);
    widget2->addAction(del);
    widget2->addAction(del);
    delete widget;
}

class MessUpPainterWidget : public QGraphicsWidget
{
public:
    MessUpPainterWidget(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0)
    : QGraphicsWidget(parent, wFlags)
    {}

    void paintWindowFrame(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QCOMPARE(painter->opacity(), 1.0);
        painter->setOpacity(0.0);
        QGraphicsWidget::paintWindowFrame(painter, option, widget);
    }
    void paint(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QCOMPARE(painter->opacity(), 1.0);
        painter->drawRect(0, 0, 100, 100);
        QGraphicsWidget::paint(painter, option, widget);
    }

};

void tst_QGraphicsWidget::painterStateProtectionOnWindowFrame()
{
    MessUpPainterWidget *widget = new MessUpPainterWidget(0, Qt::Window);
    QGraphicsScene scene(0, 0, 300, 300);
    QGraphicsView view(&scene);
    scene.addItem(widget);
    view.show();
    QTest::qWaitForWindowShown(&view);
}

class ProxyStyle : public QCommonStyle
{
public:
    ProxyStyle(QStyle *proxyStyle) : QCommonStyle()
    {
        m_proxyStyle = proxyStyle;
    }

    int pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const
    {
        return m_proxyStyle->pixelMetric(metric, option, widget);
    }

private:
    QStyle *m_proxyStyle;
};

class StyledGraphicsWidget : public QGraphicsWidget
{
public:
    StyledGraphicsWidget(bool useOwnStyle) : QGraphicsWidget(), m_style(0) {
        if (useOwnStyle) {
            QStyle *oldStyle = style();
            m_style = new ProxyStyle(oldStyle);
            setStyle(m_style);
        }

        style()->pixelMetric(QStyle::PM_SmallIconSize); // crash when style() is still in widgetStyles
    }

    ~StyledGraphicsWidget() {
        delete m_style;
    }

private:
    QStyle *m_style;
};

void tst_QGraphicsWidget::task243004_setStyleCrash()
{
    QGraphicsItem *item1 = new StyledGraphicsWidget(true);
    delete item1; // item1 not removed from widgetStyles

    QGraphicsItem *item2 = new StyledGraphicsWidget(false);
    delete item2;
}

class GraphicsWidget_task250119 : public QGraphicsWidget
{
public:
    GraphicsWidget_task250119()
        : shortcutEvents(0)
    {
        setFocusPolicy(Qt::StrongFocus);
        resize(100, 100);
    }

    int shortcutEvents;

private:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Shortcut)
            shortcutEvents++;
        return QGraphicsWidget::event(event);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        if (hasFocus()) {
            painter->setPen(QPen(Qt::black, 0, Qt::DashLine));
            painter->drawRect(rect());
        }
        painter->setPen(QPen(Qt::black, 0, Qt::SolidLine));
        painter->fillRect(rect().adjusted(2, 2, -2, -2), Qt::yellow);
        painter->drawRect(rect().adjusted(2, 2, -2, -2));
    }
};

void tst_QGraphicsWidget::task250119_shortcutContext()
{
    QGraphicsScene scene;
    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), (QWidget*)&view);


    // *** Event: ***

    GraphicsWidget_task250119 w_event;
    scene.addItem(&w_event);

    const int id = w_event.grabShortcut(Qt::Key_A, Qt::WidgetWithChildrenShortcut);
    w_event.setShortcutEnabled(id, true);

    w_event.setFocus();
    QTest::keyPress(&view, Qt::Key_A);
    QCOMPARE(w_event.shortcutEvents, 1);

    w_event.clearFocus();
    QTest::keyPress(&view, Qt::Key_A);
    QCOMPARE(w_event.shortcutEvents, 1);

    scene.removeItem(&w_event);


    // *** Signal: ***

    GraphicsWidget_task250119 w_signal;
    scene.addItem(&w_signal);

    QAction action(0);
    action.setShortcut(Qt::Key_B);
    action.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QSignalSpy spy(&action, SIGNAL(triggered()));

    w_signal.addAction(&action);

    w_signal.setFocus();
    QTest::keyPress(&view, Qt::Key_B);
    QCOMPARE(spy.count(), 1);

    w_signal.clearFocus();
    QTest::keyPress(&view, Qt::Key_B);
    QCOMPARE(spy.count(), 1);

    scene.removeItem(&w_signal);
}

class ClippingAndTransformsScene : public QGraphicsScene
{
public:
    QList<QGraphicsItem *> drawnItems;
protected:
    void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[],
                   const QStyleOptionGraphicsItem options[], QWidget *widget = 0)
    {
        drawnItems.clear();
        for (int i = 0; i < numItems; ++i)
            drawnItems << items[i];
        QGraphicsScene::drawItems(painter, numItems, items, options, widget);
    }
};

class RectWidget : public QGraphicsWidget
{
public:

    RectWidget(Qt::GlobalColor color, QGraphicsItem *parent=0) : QGraphicsWidget(parent), mColor(color) {}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->setBrush(QBrush(mColor));
        painter->drawRect(boundingRect());
    }

    Qt::GlobalColor mColor;
};

class RectItem : public QGraphicsItem
{
public:

    RectItem(Qt::GlobalColor color, QGraphicsItem *parent=0) : QGraphicsItem(parent), mColor(color) {}

    QRectF boundingRect() const
    {return QRectF(10,10,50,50);}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->setBrush(QBrush(mColor));
        painter->drawRect(boundingRect());
    }

    Qt::GlobalColor mColor;
};

void tst_QGraphicsWidget::ensureClipping()
{
    ClippingAndTransformsScene scene;
    scene.setSceneRect(-50, -50, 200, 200);

    //A root that clip children
    RectWidget *clipWidget = new RectWidget(Qt::black);
    scene.addItem(clipWidget);

    clipWidget->setFlag(QGraphicsItem::ItemClipsChildrenToShape);

    //a child
    RectWidget *childWidget = new RectWidget(Qt::red, clipWidget);
    clipWidget->setGeometry(QRectF(10, 10, 100, 100));
    childWidget->setGeometry(QRectF(25, 25, 50, 50));

    //We put a QGraphicsItem to be sure this one is also paint
    RectItem *childitem = new RectItem(Qt::blue, clipWidget);

    QGraphicsView view(&scene);
    view.setOptimizationFlag(QGraphicsView::IndirectPainting);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QList<QGraphicsItem *> expected;
    expected << clipWidget << childWidget << childitem;
    QTRY_VERIFY(scene.drawnItems.contains(clipWidget));
    QVERIFY(scene.drawnItems.contains(childWidget));
    QVERIFY(scene.drawnItems.contains(childitem));
}

class ItemChangeTester : public QGraphicsWidget
{
public:
    ItemChangeTester()
    { setFlag(ItemSendsGeometryChanges); clear(); }
    ItemChangeTester(QGraphicsItem *parent) : QGraphicsWidget(parent)
    { setFlag(ItemSendsGeometryChanges); clear(); }

    void clear()
    {
        changes.clear();
        values.clear();
        oldValues.clear();
    }
    QList<GraphicsItemChange> changes;
    QList<QVariant> values;
    QList<QVariant> oldValues;
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        changes << change;
        values << value;
        switch (change) {
        case QGraphicsItem::ItemPositionChange:
            oldValues << pos();
            break;
        case QGraphicsItem::ItemPositionHasChanged:
            break;
        default:
            break;
        }
        return value;
    }
};

void tst_QGraphicsWidget::widgetSendsGeometryChanges()
{
    ItemChangeTester widget;
    widget.setFlags(0);
    widget.clear();

    QPointF pos(10, 10);
    widget.setPos(pos);

    QCOMPARE(widget.pos(), pos);
    QCOMPARE(widget.changes.size(), 0);

    widget.setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    QCOMPARE(widget.changes.size(), 2);

    widget.setPos(QPointF());
    QCOMPARE(widget.changes.size(), 4);

    QCOMPARE(widget.pos(), QPointF());

    QRectF geometry(20, 20, 50, 50);
    widget.setGeometry(geometry);
    QCOMPARE(widget.changes.size(), 6);

    QCOMPARE(widget.geometry(), geometry);

    QCOMPARE(widget.changes, QList<QGraphicsItem::GraphicsItemChange>()
         << QGraphicsItem::ItemFlagsChange
         << QGraphicsItem::ItemFlagsHaveChanged
         << QGraphicsItem::ItemPositionChange
         << QGraphicsItem::ItemPositionHasChanged
         << QGraphicsItem::ItemPositionChange
         << QGraphicsItem::ItemPositionHasChanged);
}

class HFWWidget : public QGraphicsWidget
{
public:
    HFWWidget() : QGraphicsWidget(0, Qt::Window)
    {
        QSizePolicy sp;
        sp.setHeightForWidth(true);
        setSizePolicy(sp);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        qreal w = rect().width();
        QRectF box(0, 0, w, 2400/w);
        painter->drawRoundRect(box);
        painter->drawLine(box.topLeft(), box.bottomRight());
        painter->drawLine(box.bottomLeft(), box.topRight());
    }

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
    {
        qreal w = constraint.width();
        switch (which) {
        case Qt::MinimumSize:
            if (w >= 0 && constraint.height() < 0) {
                // keep the same area of 60x40 = 2400
                return QSizeF(w, 2400.0/w);
            } else {
                return QSizeF(10, 10);
            }
            break;
        case Qt::PreferredSize:
            return QSizeF(48.989794, 48.989794);
            default:
            break;
        }
        return QGraphicsWidget::sizeHint(which, constraint);
    }
};

void tst_QGraphicsWidget::respectHFW()
{
#if defined(Q_OS_WINCE) || defined(Q_OS_MAC) || defined(Q_WS_QWS)
    qDebug("This test is platform dependent, it fails on wince, mac and qws. Please fix.");
#else
    QGraphicsScene scene;
    HFWWidget *window = new HFWWidget;
    scene.addItem(window);
    QGraphicsView *view = new QGraphicsView(&scene);
    view->resize(400, 400);
    view->setSceneRect(-100, -100, 300,300);

    view->show();
    window->setGeometry(0, 0, 70, 70);
    QTest::qWaitForWindowShown(view);

    {   // here we go - simulate a interactive resize of the window
        QTest::mouseMove(view, view->mapFromScene(71, 71)); // bottom right corner

        QTest::mousePress(view->viewport(), Qt::LeftButton, 0, view->mapFromScene(71, 71), 200);
        view->grabMouse();
        // move both mouse cursor and set correct event in order to emulate resize
        QTest::mouseMove(view->viewport(), view->mapFromScene(60, 30), 200);
        QMouseEvent e = QMouseEvent(QEvent::MouseMove,
                      view->mapFromScene(60, 20),
                      Qt::NoButton,
                      Qt::LeftButton,
                      Qt::NoModifier);
        QApplication::sendEvent(view->viewport(), &e);
        view->releaseMouse();
    }
    const QSizeF winSize = window->size();
    qreal minHFW = window->effectiveSizeHint(Qt::MinimumSize, QSizeF(winSize.width(), -1)).height();
    QTRY_VERIFY(std::abs(minHFW - winSize.height()) < 1);
#endif
}

class PolishWidget : public QGraphicsWidget
{
public:

    PolishWidget(Qt::GlobalColor color, QGraphicsItem *parent=0) :
    QGraphicsWidget(parent), mColor(color)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->setBrush(QBrush(mColor));
        painter->drawRect(boundingRect());
    }

    void polishEvent()
    {
        if (!parentWidget()) {
            //We add a child in the polish event for the parent
            PolishWidget *childWidget = new PolishWidget(Qt::black, this);
            childWidget->setGeometry(QRectF(10,10,30,30));
        }

        QGraphicsWidget::polishEvent();
        mColor = Qt::red;
        update();
        numberOfPolish++;
    }

    static int numberOfPolish;

private:
    Qt::GlobalColor mColor;
};

int PolishWidget::numberOfPolish = 0;

void tst_QGraphicsWidget::addChildInpolishEvent()
{
    QGraphicsScene scene;

    PolishWidget *parentWidget = new PolishWidget(Qt::white);
    scene.addItem(parentWidget);

    QGraphicsView view(&scene);
    view.resize(200, 200);
    view.show();
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(PolishWidget::numberOfPolish, 2);
}

void tst_QGraphicsWidget::polishEvent()
{
    class MyGraphicsWidget : public QGraphicsWidget
    { public:
        void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
        { events << QEvent::Paint; }
        void polishEvent()
        { events << QEvent::Polish; }
        QList<QEvent::Type> events;
    };

    QGraphicsScene scene;

    MyGraphicsWidget *widget = new MyGraphicsWidget;
    scene.addItem(widget);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    // Make sure the item is painted.
    QTRY_VERIFY(widget->events.contains(QEvent::Paint));

    // Make sure the item got polish before paint.
    QCOMPARE(widget->events.at(0), QEvent::Polish);
}

void tst_QGraphicsWidget::polishEvent2()
{
    class MyGraphicsWidget : public QGraphicsWidget
    { public:
    void polishEvent()
    { events << QEvent::Polish; }
    QList<QEvent::Type> events;
    };

    QGraphicsScene scene;

    MyGraphicsWidget *widget = new MyGraphicsWidget;
    widget->hide();
    scene.addItem(widget);

    widget->events.clear();

    // Make sure the item got polish event.
    QTRY_VERIFY(widget->events.contains(QEvent::Polish));
}

void tst_QGraphicsWidget::autoFillBackground()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QCOMPARE(widget->autoFillBackground(), false);
    widget->setAutoFillBackground(true);
    QCOMPARE(widget->autoFillBackground(), true);

    const QColor color(Qt::red);
    const QRect rect(0, 0, 1, 1);

    QGraphicsScene scene;
    scene.addItem(widget);
    widget->setGeometry(rect);

    QPalette palette = widget->palette();
    palette.setColor(QPalette::Window, color);
    widget->setPalette(palette);

    QImage image(rect.size(), QImage::Format_RGB32);
    QPainter painter;
    painter.begin(&image);
    scene.render(&painter, rect, rect);
    painter.end();
    QCOMPARE(image.pixel(0, 0), color.rgb());
}

void tst_QGraphicsWidget::initialShow()
{
    class MyGraphicsWidget : public QGraphicsWidget
    { public:
        MyGraphicsWidget() : repaints(0) {}
        int repaints;
        void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget*) { ++repaints; }
        void polishEvent() { update(); }
    };

    QGraphicsScene scene;
    MyGraphicsWidget *widget = new MyGraphicsWidget;

    QGraphicsView view(&scene);
    if(PlatformQuirks::isAutoMaximizing())
        view.showFullScreen();
    else
        view.show();
    QTest::qWaitForWindowShown(&view);

    scene.addItem(widget);

    QTRY_COMPARE(widget->repaints, 1);
}

void tst_QGraphicsWidget::initialShow2()
{
    class MyGraphicsWidget : public QGraphicsWidget
    { public:
        MyGraphicsWidget() : repaints(0) {}
        int repaints;
        void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget*) { ++repaints; }
        void polishEvent() { update(); }
    };

    // Don't let paint events triggered by the windowing system
    // influence our test case. We're only interested in knowing
    // whether a QGraphicsWidget generates an additional repaint
    // on the initial show. Hence create a dummy scenario to find out
    // how many repaints we should expect.
    QGraphicsScene dummyScene(0, 0, 200, 200);
    dummyScene.addItem(new QGraphicsRectItem(0, 0, 100, 100));

    QGraphicsView *dummyView = new QGraphicsView(&dummyScene);
    dummyView->setWindowFlags(Qt::X11BypassWindowManagerHint);
    EventSpy paintSpy(dummyView->viewport(), QEvent::Paint);
    dummyView->show();
    QTest::qWaitForWindowShown(dummyView);
    const int expectedRepaintCount = paintSpy.count();
    delete dummyView;
    dummyView = 0;

    MyGraphicsWidget *widget = new MyGraphicsWidget;
    widget->resize(100, 100);

    QGraphicsScene scene(0, 0, 200, 200);
    scene.addItem(widget);

    QGraphicsView view(&scene);
    view.setWindowFlags(view.windowFlags()|Qt::X11BypassWindowManagerHint);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QTRY_COMPARE(widget->repaints, expectedRepaintCount);
}

void tst_QGraphicsWidget::itemChangeEvents()
{
    class TestGraphicsWidget : public QGraphicsWidget
    { public:
        TestGraphicsWidget() : QGraphicsWidget() {}
        QHash<QEvent::Type, QVariant> valueDuringEvents;
        bool event(QEvent *event) {
            Q_UNUSED(event);
            switch (event->type()) {
            case QEvent::EnabledChange: {
                valueDuringEvents.insert(QEvent::EnabledChange, isEnabled());
                break;
            }
            case QEvent::ParentAboutToChange: {
                valueDuringEvents.insert(QEvent::ParentAboutToChange, QVariant::fromValue(parentItem()));
                break;
            }
            case QEvent::ParentChange: {
                valueDuringEvents.insert(QEvent::ParentChange, QVariant::fromValue(parentItem()));
                break;
            }
            case QEvent::CursorChange: {
                valueDuringEvents.insert(QEvent::CursorChange, int(cursor().shape()));
                break;
            }
            case QEvent::ToolTipChange: {
                valueDuringEvents.insert(QEvent::ToolTipChange, toolTip());
                break;
            }
            default: {
                break;
            }
            }
            return true;
        }
        void showEvent(QShowEvent *event) {
            Q_UNUSED(event);
            valueDuringEvents.insert(QEvent::Show, isVisible());
        }
        void hideEvent(QHideEvent *event) {
            Q_UNUSED(event);
            valueDuringEvents.insert(QEvent::Hide, isVisible());
        }
    };

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *parent = new QGraphicsWidget;
    scene.addItem(parent);
    view.show();
    QTest::qWaitForWindowShown(&view);

    TestGraphicsWidget *item = new TestGraphicsWidget;
    item->setParentItem(parent);
    // ParentAboutToChange should be triggered before the parent has changed
    QTRY_COMPARE(qvariant_cast<QGraphicsItem *>(item->valueDuringEvents.value(QEvent::ParentAboutToChange)),
             static_cast<QGraphicsItem *>(0));
    // ParentChange should be triggered after the parent has changed
    QTRY_COMPARE(qvariant_cast<QGraphicsItem *>(item->valueDuringEvents.value(QEvent::ParentChange)),
             static_cast<QGraphicsItem *>(parent));

    // ShowEvent should be triggered before the item is shown
    QTRY_VERIFY(!item->valueDuringEvents.value(QEvent::Show).toBool());

    // HideEvent should be triggered after the item is hidden
    QVERIFY(item->isVisible());
    item->setVisible(false);
    QVERIFY(!item->isVisible());
    QTRY_VERIFY(!item->valueDuringEvents.value(QEvent::Hide).toBool());

    // CursorChange should be triggered after the cursor has changed
    item->setCursor(Qt::PointingHandCursor);
    QTRY_COMPARE(item->valueDuringEvents.value(QEvent::CursorChange).toInt(), int(item->cursor().shape()));

    // ToolTipChange should be triggered after the tooltip has changed
    item->setToolTip("tooltipText");
    QTRY_COMPARE(item->valueDuringEvents.value(QEvent::ToolTipChange).toString(), item->toolTip());

    // EnabledChange should be triggered after the enabled state has changed
    QVERIFY(item->isEnabled());
    item->setEnabled(false);
    QVERIFY(!item->isEnabled());
    QTRY_VERIFY(!item->valueDuringEvents.value(QEvent::EnabledChange).toBool());
}

void tst_QGraphicsWidget::itemSendGeometryPosChangesDeactivated()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *item = new QGraphicsWidget;
    scene.addItem(item);
    view.show();
    QTest::qWaitForWindowShown(&view);
    item->setGeometry(QRectF(0, 0, 50, 50));
    QTRY_COMPARE(item->geometry(), QRectF(0, 0, 50, 50));

    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
    item->setGeometry(QRectF(0, 0, 60, 60));
    QCOMPARE(item->geometry(), QRectF(0, 0, 60, 60));
    QCOMPARE(item->pos(), QPointF(0, 0));
    item->setPos(QPointF(10, 10));
    QCOMPARE(item->pos(), QPointF(10, 10));
    QCOMPARE(item->geometry(), QRectF(10, 10, 60, 60));

    item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);
    item->setGeometry(QRectF(0, 0, 60, 60));
    QCOMPARE(item->geometry(), QRectF(0, 0, 60, 60));
    QCOMPARE(item->pos(), QPointF(0, 0));
    item->setPos(QPointF(10, 10));
    QCOMPARE(item->pos(), QPointF(10, 10));
    QCOMPARE(item->geometry(), QRectF(10, 10, 60, 60));
}

void tst_QGraphicsWidget::QT_BUG_6544_tabFocusFirstUnsetWhenRemovingItems()
{
    QGraphicsScene scene;
    QGraphicsWidget* parent1 = new QGraphicsWidget;
    QGraphicsWidget* child1_0 = new QGraphicsWidget;
    QGraphicsWidget* child1_1 = new QGraphicsWidget;

    QGraphicsWidget* parent2 = new QGraphicsWidget;

    // Add the parent and child to the scene.
    scene.addItem(parent1);
    child1_0->setParentItem(parent1);
    child1_1->setParentItem(parent1);

    // Hide and show the child.
    child1_0->setParentItem(NULL);
    scene.removeItem(child1_0);

    // Remove parent from the scene.
    scene.removeItem(parent1);

    delete child1_0;
    delete child1_1;
    delete parent1;

    // Add an item into the scene.
    scene.addItem(parent2);

    //This should not crash
}
void tst_QGraphicsWidget::QT_BUG_12056_tabFocusFirstUnsetWhenRemovingItems()
{
    QGraphicsScene scene;
    QGraphicsWidget* item1 = new QGraphicsWidget;
    QGraphicsWidget* item2 = new QGraphicsWidget;
    QGraphicsWidget* item3 = new QGraphicsWidget;

    scene.addItem(item1);
    scene.addItem(item2);

    scene.removeItem(item2);
    scene.removeItem(item1);
    delete item2;
    delete item1;

    scene.addItem(item3);

    //This should not crash
}

QTEST_MAIN(tst_QGraphicsWidget)
#include "tst_qgraphicswidget.moc"

#else // QT_NO_STYLE_CLEANLOOKS
QTEST_NOOP_MAIN
#endif

