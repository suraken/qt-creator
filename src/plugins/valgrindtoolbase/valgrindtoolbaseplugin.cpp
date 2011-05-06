/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "valgrindtoolbaseplugin.h"

#include "valgrindsettings.h"

#include <analyzerbase/analyzersettings.h>

#include <QtCore/QStringList>
#include <QtCore/QtPlugin>

using namespace Analyzer;
using namespace Valgrind::Internal;

ValgrindToolbasePlugin::ValgrindToolbasePlugin()
{

}

ValgrindToolbasePlugin::~ValgrindToolbasePlugin()
{

}

bool ValgrindToolbasePlugin::initialize(const QStringList &/*arguments*/, QString */*errorString*/)
{
    typedef AnalyzerSubConfigFactory<ValgrindSettings, ValgrindSettings> ValgrindConfigFactory;
    AnalyzerGlobalSettings::instance()->registerSubConfigFactory(new ValgrindConfigFactory);
    return true;
}


void ValgrindToolbasePlugin::extensionsInitialized()
{

}

Q_EXPORT_PLUGIN(ValgrindToolbasePlugin)
