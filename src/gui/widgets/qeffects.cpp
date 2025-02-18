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

#include "qapplication.h"
#ifndef QT_NO_EFFECTS
#include "qdesktopwidget.h"
#include "qeffects_p.h"
#include "qevent.h"
#include "qimage.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qpointer.h"
#include "qtimer.h"
#include "qelapsedtimer.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

/*
  Internal class QAlphaWidget.

  The QAlphaWidget is shown while the animation lasts
  and displays the pixmap resulting from the alpha blending.
*/

class QAlphaWidget: public QWidget, private QEffects
{
    Q_OBJECT
public:
    QAlphaWidget(QWidget* w, Qt::WindowFlags f = nullptr);
    ~QAlphaWidget() override;

    void run(int time);

protected:
    void paintEvent(QPaintEvent* e) override;
    void closeEvent(QCloseEvent*) override;
    void alphaBlend();
    bool eventFilter(QObject *, QEvent *) override;

protected slots:
    void render();

private:
    QPixmap pm;
    double alpha;
    QImage backImage;
    QImage frontImage;
    QImage mixedImage;
    QPointer<QWidget> widget;
    int duration;
    int elapsed;
    bool showWidget;
    QTimer anim;
    QElapsedTimer checkTime;
};

static QAlphaWidget* q_blend = nullptr;

/*
  Constructs a QAlphaWidget.
*/
QAlphaWidget::QAlphaWidget(QWidget* w, Qt::WindowFlags f)
    : QWidget(QApplication::desktop()->screen(QApplication::desktop()->screenNumber(w)), f)
{
#ifndef Q_WS_WIN
    setEnabled(false);
#endif
    setAttribute(Qt::WA_NoSystemBackground, true);
    widget = w;
    alpha = 0;
}

QAlphaWidget::~QAlphaWidget()
{
#if defined(Q_WS_WIN)
    // Restore user-defined opacity value
    if (widget)
        widget->setWindowOpacity(1);
#endif
}

/*
  \reimp
*/
void QAlphaWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.drawPixmap(0, 0, pm);
}

/*
  Starts the alphablending animation.
  The animation will take about \a time ms
*/
void QAlphaWidget::run(int time)
{
    duration = time;

    if (duration < 0)
        duration = 150;

    if (!widget)
        return;

    elapsed = 0;
    checkTime.start();

    showWidget = true;
#if defined(Q_OS_WIN)
    qApp->installEventFilter(this);
    widget->setWindowOpacity(0.0);
    widget->show();
    connect(&anim, SIGNAL(timeout()), this, SLOT(render()));
    anim.start(1);
#else
    //This is roughly equivalent to calling setVisible(true) without actually showing the widget
    widget->setAttribute(Qt::WA_WState_ExplicitShowHide, true);
    widget->setAttribute(Qt::WA_WState_Hidden, false);

    qApp->installEventFilter(this);

    move(widget->geometry().x(),widget->geometry().y());
    resize(widget->size().width(), widget->size().height());

    frontImage = QPixmap::grabWidget(widget).toImage();
    backImage = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                widget->geometry().x(), widget->geometry().y(),
                                widget->geometry().width(), widget->geometry().height()).toImage();

    if (!backImage.isNull() && checkTime.elapsed() < duration / 2) {
        mixedImage = backImage.copy();
        pm = QPixmap::fromImage(mixedImage);
        show();
        setEnabled(false);

        connect(&anim, SIGNAL(timeout()), this, SLOT(render()));
        anim.start(1);
    } else {
       duration = 0;
       render();
    }
#endif
}

/*
  \reimp
*/
bool QAlphaWidget::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::Move:
	    if (o != widget)
	        break;
	    move(widget->geometry().x(),widget->geometry().y());
	    update();
	    break;
    case QEvent::Hide:
    case QEvent::Close:
	    if (o != widget)
	        break;
        // fall through
    case QEvent::MouseButtonPress:
	case QEvent::MouseButtonDblClick:
	    showWidget = false;
	    render();
	    break;
    case QEvent::KeyPress: {
	        QKeyEvent *ke = (QKeyEvent*)e;
            if (ke->key() == Qt::Key_Escape) {
		        showWidget = false;
            } else {
		        duration = 0;
            }
	        render();
	        break;
	}
    default:
	    break;
    }
    return QWidget::eventFilter(o, e);
}

/*
  \reimp
*/
void QAlphaWidget::closeEvent(QCloseEvent *e)
{
    e->accept();
    if (!q_blend)
        return;

    showWidget = false;
    render();

    QWidget::closeEvent(e);
}

/*
  Render alphablending for the time elapsed.

  Show the blended widget and free all allocated source
  if the blending is finished.
*/
void QAlphaWidget::render()
{
    int tempel = checkTime.elapsed();
    if (elapsed >= tempel)
        elapsed++;
    else
        elapsed = tempel;

    if (duration != 0)
        alpha = tempel / double(duration);
    else
        alpha = 1;

#if defined(Q_OS_WIN)
    if (alpha >= 1 || !showWidget) {
        anim.stop();
        qApp->removeEventFilter(this);
        widget->setWindowOpacity(1);
        q_blend = 0;
        deleteLater();
    } else {
        widget->setWindowOpacity(alpha);
    }
#else
    if (alpha >= 1 || !showWidget) {
        anim.stop();
        qApp->removeEventFilter(this);

        if (widget) {
            if (!showWidget) {
#ifdef Q_WS_WIN
                setEnabled(true);
                setFocus();
#endif // Q_WS_WIN
                widget->hide();
            } else {
                //Since we are faking the visibility of the widget 
                //we need to unset the hidden state on it before calling show
                widget->setAttribute(Qt::WA_WState_Hidden, true);
                widget->show();
                lower();
            }
        }
        q_blend = nullptr;
        deleteLater();
    } else {
        alphaBlend();
        pm = QPixmap::fromImage(mixedImage);
        repaint();
    }
#endif // defined(Q_OS_WIN)
}

/*
  Calculate an alphablended image.
*/
void QAlphaWidget::alphaBlend()
{
    const int a = qRound(alpha*256);
    const int ia = 256 - a;

    const int sw = frontImage.width();
    const int sh = frontImage.height();
    const int bpl = frontImage.bytesPerLine();
    switch(frontImage.depth()) {
    case 32:
        {
            uchar *mixed_data = mixedImage.bits();
            const uchar *back_data = backImage.bits();
            const uchar *front_data = frontImage.bits();

            for (int sy = 0; sy < sh; sy++) {
                quint32* mixed = (quint32*)mixed_data;
                const quint32* back = (const quint32*)back_data;
                const quint32* front = (const quint32*)front_data;
                for (int sx = 0; sx < sw; sx++) {
                    quint32 bp = back[sx];
                    quint32 fp = front[sx];

                    mixed[sx] =  qRgb((qRed(bp)*ia + qRed(fp)*a)>>8,
                                      (qGreen(bp)*ia + qGreen(fp)*a)>>8,
                                      (qBlue(bp)*ia + qBlue(fp)*a)>>8);
                }
                mixed_data += bpl;
                back_data += bpl;
                front_data += bpl;
            }
        }
    default:
        break;
    }
}

/*
  Internal class QRollEffect

  The QRollEffect widget is shown while the animation lasts
  and displays a scrolling pixmap.
*/

class QRollEffect : public QWidget, private QEffects
{
    Q_OBJECT
public:
    QRollEffect(QWidget* w, Qt::WindowFlags f, DirFlags orient);

    void run(int time);

protected:
    void paintEvent(QPaintEvent*) override;
    void closeEvent(QCloseEvent*) override;

private slots:
    void scroll();

private:
    QPointer<QWidget> widget;

    int currentHeight;
    int currentWidth;
    int totalHeight;
    int totalWidth;

    int duration;
    int elapsed;
    bool done;
    bool showWidget;
    int orientation;

    QTimer anim;
    QElapsedTimer checkTime;

    QPixmap pm;
};

static QRollEffect* q_roll = nullptr;

/*
  Construct a QRollEffect widget.
*/
QRollEffect::QRollEffect(QWidget* w, Qt::WindowFlags f, DirFlags orient)
    : QWidget(nullptr, f), orientation(orient)
{
#ifndef Q_WS_WIN
    setEnabled(false);
#endif

    widget = w;
    Q_ASSERT(widget);

    setAttribute(Qt::WA_NoSystemBackground, true);

    if (widget->testAttribute(Qt::WA_Resized)) {
        totalWidth = widget->width();
        totalHeight = widget->height();
    } else {
        totalWidth = widget->sizeHint().width();
        totalHeight = widget->sizeHint().height();
    }

    currentHeight = totalHeight;
    currentWidth = totalWidth;

    if (orientation & (RightScroll|LeftScroll))
        currentWidth = 0;
    if (orientation & (DownScroll|UpScroll))
        currentHeight = 0;

    pm = QPixmap::grabWidget(widget);
}

/*
  \reimp
*/
void QRollEffect::paintEvent(QPaintEvent*)
{
    int x = orientation & RightScroll ? std::min(0, currentWidth - totalWidth) : 0;
    int y = orientation & DownScroll ? std::min(0, currentHeight - totalHeight) : 0;

    QPainter p(this);
    p.drawPixmap(x, y, pm);
}

/*
  \reimp
*/
void QRollEffect::closeEvent(QCloseEvent *e)
{
    e->accept();
    if (done)
        return;

    showWidget = false;
    done = true;
    scroll();

    QWidget::closeEvent(e);
}

/*
  Start the animation.

  The animation will take about \a time ms, or is
  calculated if \a time is negative
*/
void QRollEffect::run(int time)
{
    if (!widget)
        return;

    duration  = time;
    elapsed = 0;

    if (duration < 0) {
        int dist = 0;
        if (orientation & (RightScroll|LeftScroll))
            dist += totalWidth - currentWidth;
        if (orientation & (DownScroll|UpScroll))
            dist += totalHeight - currentHeight;
        duration = std::min(std::max(dist/3, 50), 120);
    }

    connect(&anim, SIGNAL(timeout()), this, SLOT(scroll()));

    move(widget->geometry().x(),widget->geometry().y());
    resize(std::min(currentWidth, totalWidth), std::min(currentHeight, totalHeight));

    //This is roughly equivalent to calling setVisible(true) without actually showing the widget
    widget->setAttribute(Qt::WA_WState_ExplicitShowHide, true);
    widget->setAttribute(Qt::WA_WState_Hidden, false);

    show();
    setEnabled(false);

    qApp->installEventFilter(this);

    showWidget = true;
    done = false;
    anim.start(1);
    checkTime.start();
}

/*
  Roll according to the time elapsed.
*/
void QRollEffect::scroll()
{
    if (!done && widget) {
        int tempel = checkTime.elapsed();
        if (elapsed >= tempel)
            elapsed++;
        else
            elapsed = tempel;

        if (currentWidth != totalWidth) {
            currentWidth = totalWidth * (elapsed/duration)
                + (2 * totalWidth * (elapsed%duration) + duration)
                / (2 * duration);
            // equiv. to int((totalWidth*elapsed) / duration + 0.5)
            done = (currentWidth >= totalWidth);
        }
        if (currentHeight != totalHeight) {
            currentHeight = totalHeight * (elapsed/duration)
                + (2 * totalHeight * (elapsed%duration) + duration)
                / (2 * duration);
            // equiv. to int((totalHeight*elapsed) / duration + 0.5)
            done = (currentHeight >= totalHeight);
        }
        done = (currentHeight >= totalHeight) &&
               (currentWidth >= totalWidth);

        int w = totalWidth;
        int h = totalHeight;
        int x = widget->geometry().x();
        int y = widget->geometry().y();

        if (orientation & RightScroll || orientation & LeftScroll)
            w = std::min(currentWidth, totalWidth);
        if (orientation & DownScroll || orientation & UpScroll)
            h = std::min(currentHeight, totalHeight);

        setUpdatesEnabled(false);
        if (orientation & UpScroll)
            y = widget->geometry().y() + std::max(0, totalHeight - currentHeight);
        if (orientation & LeftScroll)
            x = widget->geometry().x() + std::max(0, totalWidth - currentWidth);
        if (orientation & UpScroll || orientation & LeftScroll)
            move(x, y);

        resize(w, h);
        setUpdatesEnabled(true);
        repaint();
    }
    if (done || !widget) {
        anim.stop();
        qApp->removeEventFilter(this);
        if (widget) {
            if (!showWidget) {
#ifdef Q_WS_WIN
                setEnabled(true);
                setFocus();
#endif
                widget->hide();
            } else {
                //Since we are faking the visibility of the widget 
                //we need to unset the hidden state on it before calling show
                widget->setAttribute(Qt::WA_WState_Hidden, true);
                widget->show();
                lower();
            }
        }
        q_roll = nullptr;
        deleteLater();
    }
}

/*!
    Scroll widget \a w in \a time ms. \a orient may be 1 (vertical), 2
    (horizontal) or 3 (diagonal).
*/
void qScrollEffect(QWidget* w, QEffects::DirFlags orient, int time)
{
    if (q_roll) {
        q_roll->deleteLater();
        q_roll = nullptr;
    }

    if (!w)
        return;

    QApplication::sendPostedEvents(w, QEvent::Move);
    QApplication::sendPostedEvents(w, QEvent::Resize);
    Qt::WindowFlags flags = Qt::ToolTip;

    // those can be popups - they would steal the focus, but are disabled
    q_roll = new QRollEffect(w, flags, orient);
    q_roll->run(time);
}

/*!
    Fade in widget \a w in \a time ms.
*/
void qFadeEffect(QWidget* w, int time)
{
    if (q_blend) {
        q_blend->deleteLater();
        q_blend = nullptr;
    }

    if (!w)
        return;

    QApplication::sendPostedEvents(w, QEvent::Move);
    QApplication::sendPostedEvents(w, QEvent::Resize);

    Qt::WindowFlags flags = Qt::ToolTip;

    // those can be popups - they would steal the focus, but are disabled
    q_blend = new QAlphaWidget(w, flags);

    q_blend->run(time);
}

QT_END_NAMESPACE

/*
  Delete this after timeout
*/

#include "qeffects.moc"

#endif //QT_NO_EFFECTS
