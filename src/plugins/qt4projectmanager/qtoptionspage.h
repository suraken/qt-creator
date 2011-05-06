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

#ifndef QTOPTIONSPAGE_H
#define QTOPTIONSPAGE_H

#include "debugginghelperbuildtask.h"

#include <coreplugin/dialogs/ioptionspage.h>

#include <QtCore/QSharedPointer>
#include <QtCore/QFutureInterface>

#include <QtGui/QWidget>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace Qt4ProjectManager {

class QtVersion;

namespace Internal {
namespace Ui {
class QtVersionManager;
class QtVersionInfo;
class DebuggingHelper;
}

class QtOptionsPageWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(QtOptionsPageWidget)
public:
    QtOptionsPageWidget(QWidget *parent, QList<QtVersion *> versions);
    ~QtOptionsPageWidget();
    QList<QtVersion *> versions() const;
    void finish();
    QString searchKeywords() const;

    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    void showEnvironmentPage(QTreeWidgetItem * item);
    void fixQtVersionName(int index);
    int indexForTreeItem(const QTreeWidgetItem *item) const;
    QTreeWidgetItem *treeItemForIndex(int index) const;
    QtVersion *currentVersion() const;
    int currentIndex() const;

    const QString m_specifyNameString;
    const QString m_specifyPathString;

    Internal::Ui::QtVersionManager *m_ui;
    Internal::Ui::QtVersionInfo *m_versionUi;
    Internal::Ui::DebuggingHelper *m_debuggingHelperUi;
    QList<QtVersion *> m_versions;
    int m_defaultVersion;
    QIcon m_invalidVersionIcon;
    QIcon m_validVersionIcon;

public slots:
    void updateState();

private slots:
    void versionChanged(QTreeWidgetItem *item, QTreeWidgetItem *old);
    void addQtDir();
    void removeQtDir();
    void makeS60Visible(bool visible);
    void onQtBrowsed();
    void updateCurrentQtName();
    void updateCurrentQMakeLocation();
    void updateCurrentS60SDKDirectory();
    void updateCurrentSbsV2Directory();
    void updateDebuggingHelperUi();
    void buildDebuggingHelper(DebuggingHelperBuildTask::Tools tools
                              = DebuggingHelperBuildTask::AllTools);
    void buildGdbHelper();
    void buildQmlDump();
    void buildQmlDebuggingLibrary();
    void buildQmlObserver();
    void slotShowDebuggingBuildLog();
    void debuggingHelperBuildFinished(int qtVersionId, const QString &output, DebuggingHelperBuildTask::Tools tools);
    void cleanUpQtVersions();

private:
    void updateDescriptionLabel();
    void showDebuggingBuildLog(const QTreeWidgetItem *currentItem);
};

class QtOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    QtOptionsPage();
    QString id() const;
    QString displayName() const;
    QString category() const;
    QString displayCategory() const;
    QIcon categoryIcon() const;
    QWidget *createPage(QWidget *parent);
    void apply();
    void finish() {}
    virtual bool matches(const QString &) const;

private:
    QtOptionsPageWidget *m_widget;
    QString m_searchKeywords;
};

} //namespace Internal
} //namespace Qt4ProjectManager


#endif // QTOPTIONSPAGE_H
