/*
 * This file is part of Krita
 *
 * Copyright (c) 1999 Matthias Elter (me@kde.org)
 * Copyright (c) 2001-2002 Igor Jansen (rm@kde.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <qpushbutton.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcolor.h>
#include <qdrawutil.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qstyle.h>
#include <qtooltip.h>

#include <klocale.h>
#include <knuminput.h>
#include <koFrameButton.h>

#include <kis_canvas_subject.h>
#include <kis_colorspace_registry.h>
#include <kis_color.h>

#include "kis_colorspace_wet.h"
#include "kis_wet_palette_widget.h"


KisColorCup::KisColorCup(const QColor & c, QWidget * parent, const char * name)
	: QPushButton(parent, name)
{
	m_color = c;
	connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

void KisColorCup::slotClicked()
{
	emit changed(m_color);
}

QSize KisColorCup::sizeHint() const
{
	return style().sizeFromContents(QStyle::CT_PushButton, this, QSize(24, 24)).
			expandedTo(QApplication::globalStrut());
}

void KisColorCup::drawButtonLabel( QPainter *painter )
{
	int x, y, w, h;
	QRect r = style().subRect( QStyle::SR_PushButtonContents, this );
	r.rect(&x, &y, &w, &h);

	int margin = 2; //style().pixelMetric( QStyle::PM_ButtonMargin, this );
	x += margin;
	y += margin;
	w -= 2*margin;
	h -= 2*margin;

	if (isOn() || isDown()) {
		x += style().pixelMetric( QStyle::PM_ButtonShiftHorizontal, this );
		y += style().pixelMetric( QStyle::PM_ButtonShiftVertical, this );
	}

	qDrawShadePanel( painter, x, y, w, h, colorGroup(), true, 1, NULL);
	if ( m_color.isValid() )
		painter->fillRect( x+1, y+1, w-2, h-2, m_color );

	if ( hasFocus() ) {
		QRect focusRect = style().subRect( QStyle::SR_PushButtonFocusRect, this );
		style().drawPrimitive( QStyle::PE_FocusRect, painter, focusRect, colorGroup() );
	}

}

KisWetPaletteWidget::KisWetPaletteWidget(QWidget *parent, const char *name) : super(parent, name)
{
	m_subject = 0;

	QVBoxLayout * vl = new QVBoxLayout(this, 0, -1, "main layout");

	QGridLayout * l = new QGridLayout(vl, 2, 8, 2, "color wells grid");

	KisColorCup * b;
	int WIDTH = 24;
	int HEIGHT = 24;

	b = new KisColorCup(QColor(240, 32, 160), this);
	l -> addWidget(b, 0, 0);
	QToolTip::add(b, i18n("Quinacridone Rose"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(159, 88, 43), this);
	l -> addWidget(b, 0, 1);
	QToolTip::add(b,i18n("Indian Red"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(254, 220, 64), this);
	l -> addWidget(b, 0, 2);
	QToolTip::add(b,i18n("Cadmium Yellow"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(36, 180, 32), this);
	l -> addWidget(b, 0, 3);
	QToolTip::add(b,i18n("Hookers Green"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(16, 185, 215), this);
	l -> addWidget(b, 0, 4);
	QToolTip::add(b,i18n("Cerulean Blue"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(96, 32, 8), this);
	l -> addWidget(b, 0, 5);
	QToolTip::add(b,i18n("Burnt Umber"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(254, 96, 8), this);
	l -> addWidget(b, 0, 6);
	QToolTip::add(b,i18n("Cadmium Red"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(255, 136, 8), this);
	l -> addWidget(b, 0, 7);
	QToolTip::add(b,i18n("Brilliant Orange"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(240, 199, 8), this);
	l -> addWidget(b, 1, 0);
	QToolTip::add(b,i18n("Hansa Yellow"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(96, 170, 130), this);
	l -> addWidget(b, 1, 1);
	QToolTip::add(b,i18n("Phthalo Green"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(48, 32, 170), this);
	l -> addWidget(b, 1, 2);
	QToolTip::add(b,i18n("French Ultramarine"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(118, 16, 135), this);
	l -> addWidget(b, 1, 3);
	QToolTip::add(b,i18n("Interference Lilac"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(254, 254, 254), this);
	l -> addWidget(b, 1, 4);
	QToolTip::add(b,i18n("Titanium White"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(64, 64, 74), this);
	l -> addWidget(b, 1, 5);
	QToolTip::add(b,i18n("Ivory Black"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));

	b = new KisColorCup(QColor(255, 255, 255), this);
	l -> addWidget(b, 1, 6);
	QToolTip::add(b,i18n("Pure Water"));
	b -> setFixedSize(WIDTH, HEIGHT);
	connect(b, SIGNAL(changed(const QColor &)), SLOT(slotFGColorSelected(const QColor &)));



	QHBoxLayout * h1 = new QHBoxLayout(vl, 0, "strength layout");
	QLabel * label = new QLabel(i18n("Paint strength:"), this);
	h1 -> addWidget(label);
	m_strength = new KDoubleNumInput(0.0, 2.0, 1.0, 0.1, 1, this);
	connect(m_strength, SIGNAL(valueChanged(double)), this,  SLOT(slotStrengthChanged(double)));
	h1 -> addWidget(m_strength);

	QHBoxLayout * h2 = new QHBoxLayout(vl, 0, "wet layout");
	label = new QLabel(i18n("Wetness:"), this);
	h2 -> addWidget(label);
	m_wetness = new KIntNumInput(16, this);
	connect(m_wetness, SIGNAL(valueChanged(int)), this, SLOT(slotWetnessChanged(int)));
	m_wetness -> setRange(0, 16, true);
	h2 -> addWidget(m_wetness);


}

void KisWetPaletteWidget::update(KisCanvasSubject *subject)
{
	m_subject = subject;

}

void KisWetPaletteWidget::slotFGColorSelected(const QColor& c)
{
	KisColorSpaceWet* cs = dynamic_cast<KisColorSpaceWet*>(
			KisColorSpaceRegistry::instance() -> get(KisID("WET", "")).data() );
	Q_ASSERT(cs);

	WetPack pack;
	Q_UINT8* data = reinterpret_cast<Q_UINT8*>(&pack);
	cs -> nativeColor(c, data, 0);
	pack.paint.w = 15 * m_wetness -> value();
	// upscale from double to uint16:
	pack.paint.h = static_cast<Q_UINT16>(m_strength -> value() * (double)(0xffff/2));
	KisColor color(data, cs);

	if(m_subject)
		m_subject->setFGColor(color);
}

void KisWetPaletteWidget::slotWetnessChanged(int n)
{
	if (!m_subject)
		return;

	KisColorSpaceWet* cs = dynamic_cast<KisColorSpaceWet*>(
			KisColorSpaceRegistry::instance() -> get(KisID("WET", "")).data() );
	Q_ASSERT(cs);
	
	KisColor color = m_subject -> fgColor();
	color.convertTo(cs);
	WetPack pack = *(reinterpret_cast<WetPack*>(color.data()));
	pack.paint.w = 15 * n;
	
	color.setColor(reinterpret_cast<Q_UINT8*>(&pack), cs);
	m_subject->setFGColor(color);
}

void KisWetPaletteWidget::slotStrengthChanged(double n)
{
	if (!m_subject)
		return;

	KisColorSpaceWet* cs = dynamic_cast<KisColorSpaceWet*>(
			KisColorSpaceRegistry::instance() -> get(KisID("WET", "")).data() );
	Q_ASSERT(cs);
	
	KisColor color = m_subject -> fgColor();
	color.convertTo(cs);
	WetPack pack = *(reinterpret_cast<WetPack*>(color.data()));
	pack.paint.h = static_cast<Q_UINT16>(n * (double)(0xffff/2)); // upscale from double to uint16

	color.setColor(reinterpret_cast<Q_UINT8*>(&pack), cs);
	m_subject->setFGColor(color);
}


#include "kis_wet_palette_widget.moc"
