/*
 * templatepaintops_plugin.cc -- Part of Krita
 *
 * Copyright (c) 2005 Boudewijn Rempt (boud@valdyas.org)
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
#include <stdlib.h>
#include <vector>

#include <qpoint.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kdebug.h>
#include <kgenericfactory.h>

#include <kis_factory.h>
#include <kis_doc.h>
#include <kis_image.h>
#include <kis_layer.h>
#include <kis_global.h>
#include <kis_types.h>
#include <kis_view.h>
#include <kis_paintop_registry.h>

#include "kis_templateop.h"

#include "templatepaintops_plugin.h"

typedef KGenericFactory<TemplatePaintOpsPlugin> TemplatePaintOpsPluginFactory;
K_EXPORT_COMPONENT_FACTORY( kritatemplatepaintops, TemplatePaintOpsPluginFactory( "kritacore" ) )


TemplatePaintOpsPlugin::TemplatePaintOpsPlugin(QObject *parent, const char *name, const QStringList &)
	: KParts::Plugin(parent, name)
{
       	setInstance(TemplatePaintOpsPluginFactory::instance());

 	kdDebug(DBG_AREA_PLUGINS) << "TemplatePaintOpsPlugin. Class: "
 		  << className()
 		  << ", Parent: "
 		  << parent -> className()
 		  << "\n";

	// This is not a gui plugin; only load it when the doc is created.
	if ( parent->inherits("KisFactory") )
	{
		KisPaintOpRegistry::instance() -> add ( new KisTemplateOpFactory );
	}

}

TemplatePaintOpsPlugin::~TemplatePaintOpsPlugin()
{
}

#include "templatepaintops_plugin.moc"
