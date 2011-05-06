/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
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

#ifndef MAEMODEBUGSUPPORT_H
#define MAEMODEBUGSUPPORT_H

#include "maemorunconfiguration.h"

#include <utils/environment.h>

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>

namespace Debugger {
class DebuggerEngine;
}
namespace ProjectExplorer { class RunControl; }

namespace Qt4ProjectManager {
namespace Internal {

class MaemoDeviceConfig;
class MaemoRunConfiguration;
class MaemoSshRunner;

class MaemoDebugSupport : public QObject
{
    Q_OBJECT
public:
    static ProjectExplorer::RunControl *createDebugRunControl(MaemoRunConfiguration *runConfig);

    MaemoDebugSupport(MaemoRunConfiguration *runConfig,
        Debugger::DebuggerEngine *engine, bool useGdb);
    ~MaemoDebugSupport();

private slots:
    void handleAdapterSetupRequested();
    void handleSshError(const QString &error);
    void startExecution();
    void handleDebuggingFinished();
    void handleRemoteOutput(const QByteArray &output);
    void handleRemoteErrorOutput(const QByteArray &output);
    void handleProgressReport(const QString &progressOutput);
    void handleRemoteProcessStarted();
    void handleRemoteProcessFinished(qint64 exitCode);

private:
    enum State {
        Inactive, StartingRunner, StartingRemoteProcess, Debugging
    };

    void handleAdapterSetupFailed(const QString &error);
    void handleAdapterSetupDone();
    bool useGdb() const;
    void setState(State newState);
    bool setPort(int &port);
    void showMessage(const QString &msg, int channel);

    const QPointer<Debugger::DebuggerEngine> m_engine;
    const QPointer<MaemoRunConfiguration> m_runConfig;
    const QSharedPointer<const MaemoDeviceConfig> m_deviceConfig;
    MaemoSshRunner * const m_runner;
    const MaemoRunConfiguration::DebuggingType m_debuggingType;
    const QList<Utils::EnvironmentItem> m_userEnvChanges;

    QByteArray m_gdbserverOutput;
    State m_state;
    int m_gdbServerPort;
    int m_qmlPort;
    bool m_useGdb;
};

} // namespace Internal
} // namespace Qt4ProjectManager

#endif // MAEMODEBUGSUPPORT_H
