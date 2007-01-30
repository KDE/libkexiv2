/* ============================================================
 * Authors: Laurent Montel <montel@kde.org>
 * Date   : 2005-09-15
 * Description : Exiv2 library interface
 *
 * Copyright 2005 by Laurent Montel
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef _KEXIV2_EXPORT_H
#define _KEXIV2_EXPORT_H

#ifdef KDEMACROS_USABLE
#include <kdemacros.h>
#endif

#ifdef KDE_EXPORT
#define KEXIV2_EXPORT KDE_EXPORT
#else
#define KEXIV2_EXPORT
#endif

#endif /* _KEXIV2_EXPORT_H */

