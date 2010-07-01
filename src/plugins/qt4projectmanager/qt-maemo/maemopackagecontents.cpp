/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "maemopackagecontents.h"

#include "maemopackagecreationstep.h"
#include "maemotoolchain.h"
#include "profilewrapper.h"

#include <qt4projectmanager/qt4buildconfiguration.h>
#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4target.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Qt4ProjectManager {
namespace Internal {

MaemoPackageContents::MaemoPackageContents(MaemoPackageCreationStep *packageStep)
    : QAbstractTableModel(packageStep),
      m_packageStep(packageStep),
      m_modified(false),
      m_initialized(false)
{
}

MaemoPackageContents::~MaemoPackageContents() {}

bool MaemoPackageContents::buildModel() const
{
    if (m_initialized)
        return true;

    m_deployables.clear();
    QSharedPointer<ProFileWrapper> proFileWrapper
        = m_packageStep->proFileWrapper();
    const ProFileWrapper::InstallsList &installs = proFileWrapper->installs();
    if (installs.targetPath.isEmpty()) {
        const Qt4ProFileNode * const proFileNode
            = m_packageStep->qt4BuildConfiguration()->qt4Target()
                ->qt4Project()->rootProjectNode();
        const QString remoteDir = proFileNode->projectType() == LibraryTemplate
            ? QLatin1String("/usr/local/lib")
            : QLatin1String("/usr/local/bin");
        m_deployables.prepend(MaemoDeployable(m_packageStep->localExecutableFilePath(),
            remoteDir));
        if (!proFileWrapper->addInstallsTarget(remoteDir)) {
            qWarning("Error updating .pro file.");
            return false;
        }
    } else {
        m_deployables.prepend(MaemoDeployable(m_packageStep->localExecutableFilePath(),
            installs.targetPath));
    }
    foreach (const ProFileWrapper::InstallsElem &elem, installs.normalElems) {
        foreach (const QString &file, elem.files) {
            m_deployables << MaemoDeployable(proFileWrapper->absFilePath(file),
                elem.path);
        }
    }

    m_initialized = true;
    m_modified = true; // ???
    return true;
}

MaemoDeployable MaemoPackageContents::deployableAt(int row) const
{
    Q_ASSERT(row >= 0 && row < rowCount());
    return m_deployables.at(row);
}

bool MaemoPackageContents::addDeployable(const MaemoDeployable &deployable,
    QString *error)
{
    if (m_deployables.contains(deployable)) {
        *error = tr("File already in list.");
        return false;
    }

    if (!m_packageStep->proFileWrapper()->addInstallsElem(deployable.remoteDir,
        deployable.localFilePath)) {
        *error = tr("Failed to update .pro file.");
        return false;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_deployables << deployable;
    endInsertRows();
    return true;
}

bool MaemoPackageContents::removeDeployableAt(int row, QString *error)
{
    Q_ASSERT(row > 0 && row < rowCount());

    const MaemoDeployable &deployable = deployableAt(row);
    if (!m_packageStep->proFileWrapper()
        ->removeInstallsElem(deployable.remoteDir, deployable.localFilePath)) {
        *error = tr("Could not update .pro file.");
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_deployables.removeAt(row);
    endRemoveRows();
    return true;
}

int MaemoPackageContents::rowCount(const QModelIndex &parent) const
{
    buildModel();
    return parent.isValid() ? 0 : m_deployables.count();
}

int MaemoPackageContents::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant MaemoPackageContents::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const MaemoDeployable &d = deployableAt(index.row());
    if (index.column() == 0 && role == Qt::DisplayRole)
        return d.localFilePath;
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return d.remoteDir;
    return QVariant();
}

Qt::ItemFlags MaemoPackageContents::flags(const QModelIndex &index) const
{
    Qt::ItemFlags parentFlags = QAbstractTableModel::flags(index);
    if (index.column() == 1)
        return parentFlags | Qt::ItemIsEditable;
    return parentFlags;
}

bool MaemoPackageContents::setData(const QModelIndex &index,
                                   const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() != 1
        || role != Qt::EditRole)
        return false;

    MaemoDeployable &deployable = m_deployables[index.row()];
    const QString &newRemoteDir = value.toString();
    if (!m_packageStep->proFileWrapper()->replaceInstallPath(deployable.remoteDir,
        deployable.localFilePath, newRemoteDir)) {
        qWarning("Error: Could not update .pro file");
        return false;
    }

    deployable.remoteDir = newRemoteDir;
    emit dataChanged(index, index);
    return true;
}

QVariant MaemoPackageContents::headerData(int section,
             Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    return section == 0 ? tr("Local File Path") : tr("Remote Directory");
}

QString MaemoPackageContents::remoteExecutableFilePath() const
{
    return buildModel() ? deployableAt(0).remoteDir + '/'
        + m_packageStep->executableFileName() : QString();
}

} // namespace Qt4ProjectManager
} // namespace Internal
