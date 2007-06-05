/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2005,2006,2007 by Laurent Montel <montel@kde.org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef _LIBKEXIV2_EXPORT_H
#define _LIBKEXIV2_EXPORT_H

#include <kdemacros.h>

#ifdef Q_WS_WIN

#ifndef KEXIV2_EXPORT
# ifdef MAKE_KEXIV2_LIB
#  define KEXIV2_EXPORT KDE_EXPORT
# else
#  define KEXIV2_EXPORT KDE_IMPORT
# endif
#endif

#else // not windows

#define KEXIV2_EXPORT KDE_EXPORT

#endif

#endif /* _LIBKEXIV2_EXPORT_H */

