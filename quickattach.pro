TARGET = QuickAttach
TEMPLATE = lib

DEFINES += QUICKATTACH_LIBRARY

# QuickAttach files

SOURCES += quickattachplugin.cpp

HEADERS += quickattachplugin.h\
        quickattach_global.h\
        quickattachconstants.h

OTHER_FILES = QuickAttach.pluginspec

QT += network # Required by hostutils

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/nick/programming/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/nick/programming/qt-creator/build

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\Nokia\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/Nokia/qtcreator" or "~/.local/share/Nokia/qtcreator" on Linux
##    "~/Library/Application Support/Nokia/Qt Creator" on Mac
USE_USER_DESTDIR = yes

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/coreplugin/coreplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/projectexplorer/projectexplorer.pri)
include($$QTCREATOR_SOURCES/src/plugins/debugger/debugger.pri)

include($$QTCREATOR_SOURCES/src/plugins/debugger/shared/shared.pri)

LIBS += -L$$IDE_PLUGIN_PATH/Nokia

