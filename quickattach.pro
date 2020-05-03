DEFINES += QUICKATTACH_LIBRARY

# QuickAttach files

SOURCES += quickattachplugin.cpp

HEADERS += quickattachplugin.h \
           quickattach_global.h \
           quickattachconstants.h

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "$$PWD/qt-creator"

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "$$PWD/qt-creator"

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%QtProjectqtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

# Plugin dependency info
QTC_PLUGIN_NAME = QuickAttach
QTC_LIB_DEPENDS +=
QTC_PLUGIN_DEPENDS += coreplugin debugger projectexplorer
QTC_PLUGIN_RECOMMENDS +=

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
