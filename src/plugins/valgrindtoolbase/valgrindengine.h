/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Nicolas Arnaud-Cormos, KDAB (nicolas.arnaud-cormos@kdab.com)
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

#ifndef VALGRINDENGINE_H
#define VALGRINDENGINE_H

#include <analyzerbase/ianalyzerengine.h>

#include "valgrindtoolbase_global.h"

#include <analyzerbase/ianalyzerengine.h>

#include <utils/environment.h>

#include <valgrind/valgrindrunner.h>

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QFutureInterface>

namespace Analyzer {
class AnalyzerSettings;
}

namespace Valgrind {
namespace Internal {

class VALGRINDTOOLBASE_EXPORT ValgrindEngine : public Analyzer::IAnalyzerEngine
{
    Q_OBJECT
public:
    explicit ValgrindEngine(const Analyzer::AnalyzerStartParameters &sp,
                            ProjectExplorer::RunConfiguration *runConfiguration);
    virtual ~ValgrindEngine();

    void start();
    void stop();

    QString executable() const;

protected:
    virtual QString progressTitle() const = 0;
    virtual QStringList toolArguments() const = 0;
    virtual Valgrind::ValgrindRunner *runner() = 0;

    Analyzer::AnalyzerSettings *m_settings;
    QFutureInterface<void> *m_progress;

private slots:
    void runnerFinished();

    void receiveStandardOutput(const QByteArray &);
    void receiveStandardError(const QByteArray &);
    void receiveProcessError(const QString &, QProcess::ProcessError);

private:
    bool m_isStopping;
};

} // namespace Internal
} // namespace Valgrind

#endif // VALGRINDENGINE_H
