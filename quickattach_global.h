#ifndef QUICKATTACH_GLOBAL_H
#define QUICKATTACH_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(QUICKATTACH_LIBRARY)
#  define QUICKATTACHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUICKATTACHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QUICKATTACH_GLOBAL_H

