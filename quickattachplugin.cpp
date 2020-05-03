#include "quickattachplugin.h"
#include "quickattachconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/applicationlauncher.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/devicesupport/localprocesslist.h>

#include <debugger/debuggerengine.h>
#include <debugger/debuggerruncontrol.h>
#include <debugger/debuggerplugin.h>
#include <debugger/shared/hostutils.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include <QtCore/QtPlugin>

using namespace QuickAttach::Internal;

QuickAttachPlugin::QuickAttachPlugin()
    : currentRunConfig(nullptr)
{
    // Create your members
}

QuickAttachPlugin::~QuickAttachPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool QuickAttachPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize method, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    QAction *action = new QAction(tr("Attach to program"), this);
    attachCmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
    attachCmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(attachToProgram()));

    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addAction(attachCmd);

    connect(ProjectExplorer::SessionManager::instance(),
            SIGNAL(startupProjectChanged(ProjectExplorer::Project*)),
            this,
            SLOT(projectChanged(ProjectExplorer::Project*)));

    return true;
}

void QuickAttachPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
}

ExtensionSystem::IPlugin::ShutdownFlag QuickAttachPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    // Disconnect from session manager during shutdown
    disconnect(ProjectExplorer::SessionManager::instance());

    return SynchronousShutdown;
}

void QuickAttachPlugin::projectChanged(ProjectExplorer::Project* project)
{
    if(!project)
        return;

    ProjectExplorer::RunConfiguration* r = project->activeTarget()->activeRunConfiguration();
    if(r)
    {
        disconnect(this, SLOT(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)));
        connect(r->target(),
                SIGNAL(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)),
                this, SLOT(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)));

        // Call now for new project
        activeRunConfigurationChanged(r);
    }
}

void QuickAttachPlugin::activeRunConfigurationChanged(ProjectExplorer::RunConfiguration* runConfig)
{
    if(runConfig)
    {
        currentRunConfig = runConfig;
        QString exe = runConfig->aspect<ProjectExplorer::ExecutableAspect>()->executable().fileName();
        if(exe.isEmpty())
            exe = "program";
        attachCmd->action()->setText(tr("Attach to %1").arg(exe));
    }
}

void QuickAttachPlugin::attachToProgram()
{
    using namespace Debugger;
    using namespace ProjectExplorer;

    Utils::FilePath targetExe = currentRunConfig->aspect<ExecutableAspect>()->executable();
    if(currentRunConfig && !targetExe.isEmpty())
    {
        QList<DeviceProcessItem> procs = DeviceProcessList::localProcesses();
        foreach(const DeviceProcessItem& p, procs)
        {
            // FIXME: Linux only
            // image not always filled in, let's bodge it
            Utils::FilePath exe = Utils::FilePath::fromString(p.exe);
#ifdef Q_OS_LINUX
            if(exe.isEmpty())
            {
                exe = Utils::FilePath::fromFileInfo(QFileInfo(QString::fromLatin1("/proc/%1/exe").arg(p.pid)));
            }
#endif
            if(exe == targetExe)
            {
                ProjectExplorer::RunControl* rc = new ProjectExplorer::RunControl(ProjectExplorer::Constants::DEBUG_RUN_MODE);
                rc->setRunConfiguration(currentRunConfig);

                Debugger::DebuggerRunTool* rt = new DebuggerRunTool(rc);
                rt->setAttachPid(p.pid);
                rt->setRunControlName(tr("Process %1").arg(p.pid));
                rt->setInferiorExecutable(exe);
                rt->setStartMode(AttachExternal);
                rt->setCloseMode(DetachAtClose);
                rt->setContinueAfterAttach(true);
                rt->startRunControl();
                return;
            }
        }

        QMessageBox::warning(Core::ICore::instance()->mainWindow(),
                             tr("Program not found"),
                             tr("Couldn't find a running instance of \"%1\" to attach to.").arg(targetExe.fileName()));
        return;
    }

    QMessageBox::warning(Core::ICore::instance()->mainWindow(),
                         tr("Project config not found"),
                         tr("Could not find a usable config your for current startup project."));
}
