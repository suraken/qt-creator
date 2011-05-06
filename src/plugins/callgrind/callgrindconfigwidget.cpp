/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "callgrindconfigwidget.h"

#include "ui_callgrindconfigwidget.h"

#include "callgrindsettings.h"

using namespace Callgrind::Internal;

CallgrindConfigWidget::CallgrindConfigWidget(AbstractCallgrindSettings *settings, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::CallgrindConfigWidget)
    , m_settings(settings)
{
    m_ui->setupUi(this);

    m_ui->enableCacheSim->setChecked(m_settings->enableCacheSim());
    connect(m_ui->enableCacheSim, SIGNAL(toggled(bool)),
            m_settings, SLOT(setEnableCacheSim(bool)));
    connect(m_settings, SIGNAL(enableCacheSimChanged(bool)),
            m_ui->enableCacheSim, SLOT(setChecked(bool)));

    m_ui->enableBranchSim->setChecked(m_settings->enableBranchSim());
    connect(m_ui->enableBranchSim, SIGNAL(toggled(bool)),
            m_settings, SLOT(setEnableBranchSim(bool)));
    connect(m_settings, SIGNAL(enableBranchSimChanged(bool)),
            m_ui->enableBranchSim, SLOT(setChecked(bool)));

    m_ui->collectSystime->setChecked(m_settings->collectSystime());
    connect(m_ui->collectSystime, SIGNAL(toggled(bool)),
            m_settings, SLOT(setCollectSystime(bool)));
    connect(m_settings, SIGNAL(collectSystimeChanged(bool)),
            m_ui->collectSystime, SLOT(setChecked(bool)));

    m_ui->collectBusEvents->setChecked(m_settings->collectBusEvents());
    connect(m_ui->collectBusEvents, SIGNAL(toggled(bool)),
            m_settings, SLOT(setCollectBusEvents(bool)));
    connect(m_settings, SIGNAL(collectBusEventsChanged(bool)),
            m_ui->collectBusEvents, SLOT(setChecked(bool)));
}

CallgrindConfigWidget::~CallgrindConfigWidget()
{
    delete m_ui;
}
