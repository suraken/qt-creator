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

#include "projectnodes.h"

#include "nodesvisitor.h"
#include "projectexplorerconstants.h"

#include <coreplugin/mimedatabase.h>
#include <coreplugin/fileiconprovider.h>
#include <utils/qtcassert.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QStyle>

using namespace ProjectExplorer;

/*!
  \class ProjectExplorer::Node

  \brief Base class of all nodes in the node hierarchy.

  The nodes are arranged in a tree where leaves are FileNodes and non-leaves are FolderNodes
  A Project is a special Folder that manages the files and normal folders underneath it.

  The Watcher emits signals for structural changes in the hierarchy.
  A Visitor can be used to traverse all Projects and other Folders.

  \sa ProjectExplorer::FileNode, ProjectExplorer::FolderNode, ProjectExplorer::ProjectNode
  \sa ProjectExplorer::NodesWatcher, ProjectExplorer::NodesVisitor
*/

Node::Node(NodeType nodeType,
           const QString &filePath)
        : QObject(),
          m_nodeType(nodeType),
          m_projectNode(0),
          m_folderNode(0),
          m_path(filePath)
{

}

NodeType Node::nodeType() const
{
    return m_nodeType;
}

/*!
  \brief Project that owns & manages the node. It's the first project in list of ancestors.
  */
ProjectNode *Node::projectNode() const
{
    return m_projectNode;
}

/*!
  \brief Parent in node hierarchy.
  */
FolderNode *Node::parentFolderNode() const
{
    return m_folderNode;
}

/*!
  \brief Path of file or folder in the filesystem the node represents.
  */
QString Node::path() const
{
    return m_path;
}

void Node::setNodeType(NodeType type)
{
    m_nodeType = type;
}

void Node::setProjectNode(ProjectNode *project)
{
    m_projectNode = project;
}

void Node::setParentFolderNode(FolderNode *parentFolder)
{
    m_folderNode = parentFolder;
}

void Node::setPath(const QString &path)
{
    m_path = path;
}

/*!
  \class ProjectExplorer::FileNode

  \brief In-memory presentation of a file. All FileNode's are leaf nodes.

  \sa ProjectExplorer::FolderNode, ProjectExplorer::ProjectNode
*/

FileNode::FileNode(const QString &filePath,
                   const FileType fileType,
                   bool generated)
        : Node(FileNodeType, filePath),
          m_fileType(fileType),
          m_generated(generated)
{
}

FileType FileNode::fileType() const
{
    return m_fileType;
}

/*!
  \brief Returns true if the file is automatically generated by a compile step.
  */
bool FileNode::isGenerated() const
{
    return m_generated;
}

/*!
  \class ProjectExplorer::FolderNode

  In-memory presentation of a folder. Note that the node itself + all children (files and folders) are "managed" by the owning project.

  \sa ProjectExplorer::FileNode, ProjectExplorer::ProjectNode
*/
FolderNode::FolderNode(const QString &folderPath)  :
    Node(FolderNodeType, folderPath),
    m_displayName(QDir::toNativeSeparators(folderPath))
{
}

FolderNode::~FolderNode()
{
    qDeleteAll(m_subFolderNodes);
    qDeleteAll(m_fileNodes);
}

/*!
    \brief The display name that should be used in a view.
    \sa setFolderName()
 */

QString FolderNode::displayName() const
{
    return m_displayName;
}

/*!
  \brief The icon that should be used in a view. Default is the directory icon (QStyle::S_PDirIcon).
  s\a setIcon()
 */
QIcon FolderNode::icon() const
{
    // Instantiating the Icon provider is expensive.
    if (m_icon.isNull())
        m_icon = Core::FileIconProvider::instance()->icon(QFileIconProvider::Folder);
    return m_icon;
}

QList<FileNode*> FolderNode::fileNodes() const
{
    return m_fileNodes;
}

QList<FolderNode*> FolderNode::subFolderNodes() const
{
    return m_subFolderNodes;
}

void FolderNode::accept(NodesVisitor *visitor)
{
    visitor->visitFolderNode(this);
    foreach (FolderNode *subFolder, m_subFolderNodes)
        subFolder->accept(visitor);
}

void FolderNode::setDisplayName(const QString &name)
{
    m_displayName = name;
}

void FolderNode::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

/*!
  \class ProjectExplorer::ProjectNode

  \brief In-memory presentation of a Project.

  A concrete subclass must implement the "persistent" stuff

  \sa ProjectExplorer::FileNode, ProjectExplorer::FolderNode
*/

/*!
  \brief Creates uninitialized ProjectNode object.
  */
ProjectNode::ProjectNode(const QString &projectFilePath)
        : FolderNode(projectFilePath)
{
    setNodeType(ProjectNodeType);
    // project node "manages" itself
    setProjectNode(this);
    setDisplayName(QFileInfo(projectFilePath).fileName());
}

QList<ProjectNode*> ProjectNode::subProjectNodes() const
{
    return m_subProjectNodes;
}

void ProjectNode::aboutToChangeHasBuildTargets()
{
    foreach (NodesWatcher *watcher, watchers())
        emit watcher->aboutToChangeHasBuildTargets(this);
}

void ProjectNode::hasBuildTargetsChanged()
{
    foreach (NodesWatcher *watcher, watchers())
        emit watcher->hasBuildTargetsChanged(this);
}

/*!
  \function bool ProjectNode::addSubProjects(const QStringList &)
  */

/*!
  \function bool ProjectNode::removeSubProjects(const QStringList &)
  */

/*!
  \function bool ProjectNode::addFiles(const FileType, const QStringList &, QStringList *)
  */

/*!
  \function bool ProjectNode::removeFiles(const FileType, const QStringList &, QStringList *)
  */

/*!
  \function bool ProjectNode::renameFile(const FileType, const QString &, const QString &)
  */

bool ProjectNode::deploysFolder(const QString &folder) const
{
    Q_UNUSED(folder);
    return false;
}

QList<NodesWatcher*> ProjectNode::watchers() const
{
    return m_watchers;
}

/*!
   \brief Registers a watcher for the current project & all sub projects

   It does not take ownership of the watcher.
*/

void ProjectNode::registerWatcher(NodesWatcher *watcher)
{
    if (!watcher)
        return;
    connect(watcher, SIGNAL(destroyed(QObject *)),
            this, SLOT(watcherDestroyed(QObject *)));
    m_watchers.append(watcher);
    foreach (ProjectNode *subProject, m_subProjectNodes)
        subProject->registerWatcher(watcher);
}

/*!
    \brief Removes a watcher for the current project & all sub projects.
*/
void ProjectNode::unregisterWatcher(NodesWatcher *watcher)
{
    if (!watcher)
        return;
    m_watchers.removeOne(watcher);
    foreach (ProjectNode *subProject, m_subProjectNodes)
        subProject->unregisterWatcher(watcher);
}

void ProjectNode::accept(NodesVisitor *visitor)
{
    visitor->visitProjectNode(this);

    foreach (FolderNode *folder, m_subFolderNodes)
        folder->accept(visitor);
}

/*!
  \brief Adds project nodes to the hierarchy and emits the corresponding signals.
  */
void ProjectNode::addProjectNodes(const QList<ProjectNode*> &subProjects)
{
    if (!subProjects.isEmpty()) {
        QList<FolderNode*> folderNodes;
        foreach (ProjectNode *projectNode, subProjects)
            folderNodes << projectNode;

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAboutToBeAdded(this, folderNodes);

        foreach (ProjectNode *project, subProjects) {
            QTC_ASSERT(!project->parentFolderNode() || project->parentFolderNode() == this,
                       qDebug("Project node has already a parent"));
            project->setParentFolderNode(this);
            foreach (NodesWatcher *watcher, m_watchers)
                project->registerWatcher(watcher);
            m_subFolderNodes.append(project);
            m_subProjectNodes.append(project);
        }
        qSort(m_subFolderNodes.begin(), m_subFolderNodes.end(),
              sortNodesByPath);
        qSort(m_subProjectNodes.begin(), m_subProjectNodes.end(),
              sortNodesByPath);

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAdded();
    }
}

/*!
  \brief Remove project nodes from the hierarchy and emits the corresponding signals.

  All objects in the argument list are deleted.
*/

void ProjectNode::removeProjectNodes(const QList<ProjectNode*> &subProjects)
{
    if (!subProjects.isEmpty()) {
        QList<FolderNode*> toRemove;
        foreach (ProjectNode *projectNode, subProjects)
            toRemove << projectNode;
        qSort(toRemove.begin(), toRemove.end(), sortNodesByPath);

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAboutToBeRemoved(this, toRemove);

        QList<FolderNode*>::const_iterator toRemoveIter = toRemove.constBegin();
        QList<FolderNode*>::iterator folderIter = m_subFolderNodes.begin();
        QList<ProjectNode*>::iterator projectIter = m_subProjectNodes.begin();
        for (; toRemoveIter != toRemove.constEnd(); ++toRemoveIter) {
            while ((*projectIter)->path() != (*toRemoveIter)->path()) {
                ++projectIter;
                QTC_ASSERT(projectIter != m_subProjectNodes.end(),
                    qDebug("Project to remove is not part of specified folder!"));
            }
            while ((*folderIter)->path() != (*toRemoveIter)->path()) {
                ++folderIter;
                QTC_ASSERT(folderIter != m_subFolderNodes.end(),
                    qDebug("Project to remove is not part of specified folder!"));
            }
            delete *projectIter;
            projectIter = m_subProjectNodes.erase(projectIter);
            folderIter = m_subFolderNodes.erase(folderIter);
        }

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersRemoved();
    }
}

/*!
  \brief Adds folder nodes to the hierarchy and emits the corresponding signals.
*/
void ProjectNode::addFolderNodes(const QList<FolderNode*> &subFolders, FolderNode *parentFolder)
{
    Q_ASSERT(parentFolder);

    if (!subFolders.isEmpty()) {
        const bool emitSignals = (parentFolder->projectNode() == this);

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                watcher->foldersAboutToBeAdded(parentFolder, subFolders);

        foreach (FolderNode *folder, subFolders) {
            QTC_ASSERT(!folder->parentFolderNode(),
                qDebug("Project node has already a parent folder"));
            folder->setParentFolderNode(parentFolder);
            folder->setProjectNode(this);

            // Find the correct place to insert
            if (parentFolder->m_subFolderNodes.count() == 0 || sortNodesByPath(parentFolder->m_subFolderNodes.last(), folder)) {
                // empty list or greater then last node
                parentFolder->m_subFolderNodes.append(folder);
            } else {
                // Binary Search for insertion point
                int l = 0;
                int r = parentFolder->m_subFolderNodes.count();
                while (l != r) {
                    int i = (l + r) / 2;
                    if (sortNodesByPath(folder, parentFolder->m_subFolderNodes.at(i))) {
                        r = i;
                    } else {
                        l = i + 1;
                    }
                }
                parentFolder->m_subFolderNodes.insert(l, folder);
            }

            // project nodes have to be added via addProjectNodes
            QTC_ASSERT(folder->nodeType() != ProjectNodeType,
                qDebug("project nodes have to be added via addProjectNodes"));
        }

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->foldersAdded();
    }
}

/*!
  \brief Remove file nodes from the hierarchy and emits the corresponding signals.

  All objects in the subFolders list are deleted.
*/
void ProjectNode::removeFolderNodes(const QList<FolderNode*> &subFolders,
                                   FolderNode *parentFolder)
{
    Q_ASSERT(parentFolder);

    if (!subFolders.isEmpty()) {
        const bool emitSignals = (parentFolder->projectNode() == this);

        QList<FolderNode*> toRemove = subFolders;
        qSort(toRemove.begin(), toRemove.end(), sortNodesByPath);

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->foldersAboutToBeRemoved(parentFolder, toRemove);

        QList<FolderNode*>::const_iterator toRemoveIter = toRemove.constBegin();
        QList<FolderNode*>::iterator folderIter = parentFolder->m_subFolderNodes.begin();
        for (; toRemoveIter != toRemove.constEnd(); ++toRemoveIter) {
            QTC_ASSERT((*toRemoveIter)->nodeType() != ProjectNodeType,
                qDebug("project nodes have to be removed via removeProjectNodes"));
            while ((*folderIter)->path() != (*toRemoveIter)->path()) {
                ++folderIter;
                QTC_ASSERT(folderIter != parentFolder->m_subFolderNodes.end(),
                    qDebug("Folder to remove is not part of specified folder!"));
            }
            delete *folderIter;
            folderIter = parentFolder->m_subFolderNodes.erase(folderIter);
        }

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->foldersRemoved();
    }
}

/*!
  \brief Adds file nodes to the internal list and emits the corresponding signals.

  This method should be called within an implementation of the public method addFiles.
*/

void ProjectNode::addFileNodes(const QList<FileNode*> &files, FolderNode *folder)
{
    Q_ASSERT(folder);

    if (!files.isEmpty()) {
        const bool emitSignals = (folder->projectNode() == this);

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->filesAboutToBeAdded(folder, files);

        foreach (FileNode *file, files) {
            QTC_ASSERT(!file->parentFolderNode(),
                qDebug("File node has already a parent folder"));

            file->setParentFolderNode(folder);
            file->setProjectNode(this);
            // Now find the correct place to insert file
            if (folder->m_fileNodes.count() == 0 || sortNodesByPath(folder->m_fileNodes.last(), file)) {
                // empty list or greater then last node
                folder->m_fileNodes.append(file);
            } else {
                // Binary Search for insertion point
                int l = 0;
                int r = folder->m_fileNodes.count();
                while (l != r) {
                    int i = (l + r) / 2;
                    if (sortNodesByPath(file, folder->m_fileNodes.at(i))) {
                        r = i;
                    } else {
                        l = i + 1;
                    }
                }
                folder->m_fileNodes.insert(l, file);
            }
        }

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->filesAdded();
    }
}

/*!
  \brief Remove file nodes from the internal list and emits the corresponding signals.

  All objects in the argument list are deleted.
  This method should be called within an implementation of the public method removeFiles.
*/

void ProjectNode::removeFileNodes(const QList<FileNode*> &files, FolderNode *folder)
{
    Q_ASSERT(folder);

    if (!files.isEmpty()) {
        const bool emitSignals = (folder->projectNode() == this);

        QList<FileNode*> toRemove = files;
        qSort(toRemove.begin(), toRemove.end(), sortNodesByPath);

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->filesAboutToBeRemoved(folder, toRemove);

        QList<FileNode*>::const_iterator toRemoveIter = toRemove.constBegin();
        QList<FileNode*>::iterator filesIter = folder->m_fileNodes.begin();
        for (; toRemoveIter != toRemove.constEnd(); ++toRemoveIter) {
            while ((*filesIter)->path() != (*toRemoveIter)->path()) {
                ++filesIter;
                QTC_ASSERT(filesIter != folder->m_fileNodes.end(),
                    qDebug("File to remove is not part of specified folder!"));
            }
            delete *filesIter;
            filesIter = folder->m_fileNodes.erase(filesIter);
        }

        if (emitSignals)
            foreach (NodesWatcher *watcher, m_watchers)
                emit watcher->filesRemoved();
    }
}

void ProjectNode::watcherDestroyed(QObject *watcher)
{
    // cannot use qobject_cast here
    unregisterWatcher(static_cast<NodesWatcher*>(watcher));
}

/*!
  \brief Sort pointers to FileNodes
*/
bool ProjectNode::sortNodesByPath(Node *n1, Node *n2) {
    return n1->path() < n2->path();
}

/*!
  \class ProjectExplorer::SessionNode
*/

SessionNode::SessionNode(const QString &sessionPath, QObject *parentObject)
        : FolderNode(sessionPath)
{
    setParent(parentObject);
    setNodeType(SessionNodeType);
}

QList<NodesWatcher*> SessionNode::watchers() const
{
    return m_watchers;
}

/*!
   \brief Registers a watcher for the complete session tree.
   It does not take ownership of the watcher.
*/

void SessionNode::registerWatcher(NodesWatcher *watcher)
{
    if (!watcher)
        return;
    connect(watcher, SIGNAL(destroyed(QObject*)),
            this, SLOT(watcherDestroyed(QObject*)));
    m_watchers.append(watcher);
    foreach (ProjectNode *project, m_projectNodes)
        project->registerWatcher(watcher);
}

/*!
    \brief Removes a watcher from the complete session tree
*/

void SessionNode::unregisterWatcher(NodesWatcher *watcher)
{
    if (!watcher)
        return;
    m_watchers.removeOne(watcher);
    foreach (ProjectNode *project, m_projectNodes)
        project->unregisterWatcher(watcher);
}

void SessionNode::accept(NodesVisitor *visitor)
{
    visitor->visitSessionNode(this);
    foreach (ProjectNode *project, m_projectNodes)
        project->accept(visitor);
}

QList<ProjectNode*> SessionNode::projectNodes() const
{
    return m_projectNodes;
}

void SessionNode::addProjectNodes(const QList<ProjectNode*> &projectNodes)
{
    if (!projectNodes.isEmpty()) {
        QList<FolderNode*> folderNodes;
        foreach (ProjectNode *projectNode, projectNodes)
            folderNodes << projectNode;

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAboutToBeAdded(this, folderNodes);

        foreach (ProjectNode *project, projectNodes) {
            QTC_ASSERT(!project->parentFolderNode(),
                qDebug("Project node has already a parent folder"));
            project->setParentFolderNode(this);
            foreach (NodesWatcher *watcher, m_watchers)
                project->registerWatcher(watcher);
            m_subFolderNodes.append(project);
            m_projectNodes.append(project);
        }

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAdded();
   }
}

void SessionNode::removeProjectNodes(const QList<ProjectNode*> &projectNodes)
{
    if (!projectNodes.isEmpty()) {
        QList<FolderNode*> toRemove;
        foreach (ProjectNode *projectNode, projectNodes)
            toRemove << projectNode;

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersAboutToBeRemoved(this, toRemove);

        QList<FolderNode*>::const_iterator toRemoveIter = toRemove.constBegin();
        QList<FolderNode*>::iterator folderIter = m_subFolderNodes.begin();
        QList<ProjectNode*>::iterator projectIter = m_projectNodes.begin();
        for (; toRemoveIter != toRemove.constEnd(); ++toRemoveIter) {
            while ((*projectIter)->path() != (*toRemoveIter)->path()) {
                ++projectIter;
                QTC_ASSERT(projectIter != m_projectNodes.end(),
                    qDebug("Project to remove is not part of specified folder!"));
            }
            while ((*folderIter)->path() != (*toRemoveIter)->path()) {
                ++folderIter;
                QTC_ASSERT(folderIter != m_subFolderNodes.end(),
                    qDebug("Project to remove is not part of specified folder!"));
            }
            projectIter = m_projectNodes.erase(projectIter);
            folderIter = m_subFolderNodes.erase(folderIter);
        }

        foreach (NodesWatcher *watcher, m_watchers)
            emit watcher->foldersRemoved();
    }
}

void SessionNode::watcherDestroyed(QObject *watcher)
{
    // cannot use qobject_cast here
    unregisterWatcher(static_cast<NodesWatcher*>(watcher));
}

/*!
  \class ProjectExplorer::NodesWatcher

  \brief NodesWatcher lets you keep track of changes in the tree.

  Add a watcher by calling ProjectNode::registerWatcher() or
  SessionNode::registerWatcher(). Whenever the tree underneath the
  ProectNode or SessionNode changes (e.g. nodes are added/removed),
  the corresponding signals of the NodesWatcher are emitted.
  Watchers can be removed from the complete tree or a subtree
  by calling ProjectNode::unregisterWatcher and
  SessionNode::unregisterWatcher().

  The NodesWatcher is similar to the Observer in the  
  well-known Observer pattern (Booch et al).

  \sa ProjectExplorer::Node
*/

NodesWatcher::NodesWatcher(QObject *parent)
        : QObject(parent)
{
}

// TODO Maybe put this in core, so that all can benefit
FileType typeForFileName(const Core::MimeDatabase *db, const QFileInfo &file)
{
    const Core::MimeType mt = db->findByFile(file);
    if (!mt)
        return UnknownFileType;

    const QString typeName = mt.type();
    if (typeName == QLatin1String(Constants::CPP_SOURCE_MIMETYPE)
        || typeName == QLatin1String(Constants::C_SOURCE_MIMETYPE))
        return SourceType;
    if (typeName == QLatin1String(Constants::CPP_HEADER_MIMETYPE)
        || typeName == QLatin1String(Constants::C_HEADER_MIMETYPE))
        return HeaderType;
    if (typeName == QLatin1String(Constants::RESOURCE_MIMETYPE))
        return ResourceType;
    if (typeName == QLatin1String(Constants::FORM_MIMETYPE))
        return FormType;
    return UnknownFileType;
}
