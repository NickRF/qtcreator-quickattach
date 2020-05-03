#ifndef QUICKATTACH_H
#define QUICKATTACH_H

#include "quickattach_global.h"

#include <extensionsystem/iplugin.h>

namespace ProjectExplorer {
class Project;
class RunConfiguration;
}

namespace Core {
class Command;
}

namespace QuickAttach {
namespace Internal {

class QuickAttachPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QuickAttach.json")

public:
    QuickAttachPlugin();
    ~QuickAttachPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void projectChanged(ProjectExplorer::Project* project);
    void activeRunConfigurationChanged(ProjectExplorer::RunConfiguration* runConfig);
    void attachToProgram();

private:
    ProjectExplorer::RunConfiguration* currentRunConfig;
    Core::Command* attachCmd;
};

} // namespace Internal
} // namespace QuickAttach

#endif // QUICKATTACH_H
