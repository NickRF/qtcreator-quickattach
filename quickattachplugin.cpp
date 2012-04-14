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
#include <projectexplorer/applicationrunconfiguration.h>

#include <debugger/debuggerplugin.h>
#include <debugger/debuggerstartparameters.h>
#include <debugger/debuggerrunner.h>
#include <debugger/shared/hostutils.h>

#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>

#include <QtCore/QtPlugin>

using namespace QuickAttach::Internal;

QuickAttachPlugin::QuickAttachPlugin()
    : currentRunConfig(NULL)
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
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    Core::ActionManager *am = Core::ICore::instance()->actionManager();

    QAction *action = new QAction(tr("Attach to program"), this);
    attachCmd = am->registerAction(action, Constants::ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
    attachCmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(attachToProgram()));

    am->actionContainer(Core::Constants::M_TOOLS)->addAction(attachCmd);

    connect(
        ProjectExplorer::ProjectExplorerPlugin::instance()->session(),
        SIGNAL(startupProjectChanged(ProjectExplorer::Project*)),
        this,
        SLOT(projectChanged(ProjectExplorer::Project*))
    );

    return true;
}

void QuickAttachPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag QuickAttachPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void QuickAttachPlugin::projectChanged(ProjectExplorer::Project* project)
{
    using namespace ProjectExplorer;
    LocalApplicationRunConfiguration* r = qobject_cast<LocalApplicationRunConfiguration*>(project->activeTarget()->activeRunConfiguration());
    if(r)
    {
        disconnect(this, SLOT(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)));
        connect(
            r->target(),
            SIGNAL(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)),
            this, SLOT(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*))
        );

        // Call now for new project
        activeRunConfigurationChanged(r);
    }
}

void QuickAttachPlugin::activeRunConfigurationChanged(ProjectExplorer::RunConfiguration* runConfig)
{
    using namespace ProjectExplorer;
    LocalApplicationRunConfiguration* r = qobject_cast<LocalApplicationRunConfiguration*>(runConfig);
    if(r)
    {
        currentRunConfig = r;
        QString exe = QFileInfo(r->executable()).fileName();
        attachCmd->action()->setText(tr("Attach to %1").arg(exe));
    }
}

void QuickAttachPlugin::attachToProgram()
{
    using namespace Debugger;
    using namespace ProjectExplorer;

    if(currentRunConfig && !currentRunConfig->executable().isEmpty())
    {
        QList<Debugger::Internal::ProcData> procs = Debugger::Internal::hostProcessList();
        foreach(const Debugger::Internal::ProcData& p, procs)
        {
            // FIXME: Linux only
            // image not always filled in, let's bodge it
            QString image = p.image;
#ifdef Q_OS_LINUX
            if(image.isEmpty())
            {
                image = QFileInfo(QString::fromLatin1("/proc/%1/exe").arg(p.ppid)).canonicalFilePath();
            }
#endif
            if(image == currentRunConfig->executable())
            {
                DebuggerStartParameters sp;
                sp.attachPID = p.ppid.toLongLong();
                sp.displayName = tr("Process %1").arg(p.ppid);
                sp.executable = currentRunConfig->executable();
                sp.startMode = AttachExternal;
                sp.closeMode = DetachAtClose;
                sp.toolChainAbi = currentRunConfig->abi();
                sp.debuggerCommand = currentRunConfig->target()->activeBuildConfiguration()->toolChain()->debuggerCommand().toString();
                RunControl* rc = DebuggerPlugin::createDebugger(sp, currentRunConfig);
                ProjectExplorerPlugin::instance()->startRunControl(rc, DebugRunMode);
                return;
            }
        }

        QMessageBox::warning(Core::ICore::instance()->mainWindow(),
                             tr("Program not found"),
                             tr("Couldn't find a running instance of your program to attach to."));
        return;
    }

    QMessageBox::warning(Core::ICore::instance()->mainWindow(),
                         tr("Project config not found"),
                         tr("Could not find a usable config your for current startup project."));
}

Q_EXPORT_PLUGIN2(QuickAttach, QuickAttachPlugin)

