/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// Local includes.

#include "kexiv2private.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

//-- Comments manipulation methods --------------------------------

bool KExiv2::asComments()
{
    return !d->imageComments.empty();
}

bool KExiv2::clearComments()
{
    return setComments(QByteArray());
}

QByteArray KExiv2::getComments() const
{
    return QByteArray(d->imageComments.data(), d->imageComments.size());
}

QString KExiv2::getCommentsDecoded() const
{
    return detectEncodingAndDecode(commentsMetaData());
}

bool KExiv2::setComments(const QByteArray& data)
{
    d->imageComments = std::string(data.data(), data.size());
    return true;
}

}  // NameSpace KExiv2Iface
