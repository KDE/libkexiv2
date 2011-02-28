/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-14
 * @brief  Embedded preview loading
 *
 * @author Copyright (C) 2009-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2009-2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KEXIV2PREVIEWS
#define KEXIV2PREVIEWS

// Qt includes

#include <QtCore/QByteArray>
#include <QtCore/QSize>
#include <QtCore/QString>

// Local includes.

#include "libkexiv2_export.h"

class QImage;

namespace KExiv2Iface
{

class KExiv2PreviewsPriv;

class KEXIV2_EXPORT KExiv2Previews
{
public:

    /**
     * Open the given file and scan for embedded preview images
     */
    KExiv2Previews(const QString& filePath);

    /**
     * Open the given image data and scan the image for embedded preview images.
     */
    KExiv2Previews(const QByteArray& imgData);
    ~KExiv2Previews();

    /// Returns the pixel size of the original image, as read from the file (not the metadata).
    QSize originalSize() const;
    /// Returns the mimeType of the original image, detected from the file's content.
    QString originalMimeType() const;

    /// Returns if there are any preview images available
    bool       isEmpty();

    /// Returns how many embedded previews are available
    int        count();
    int        size() { return count(); }

    /**
     * For each contained preview image, return the size
     * of the image data in bytes, width and height of the preview,
     * the mimeType and the file extension.
     * Ensure that index < count().
     * Previews are sorted by width*height, largest first.
     */
    int        dataSize(int index = 0);
    int        width(int index = 0);
    int        height(int index = 0);
    QString    mimeType(int index = 0);
    QString    fileExtension(int index = 0);

    /**
     * Retrieve the image data for the specified embedded preview image
     */
    QByteArray data(int index = 0);

    /**
     * Loads the data of the specified preview and creates a QImage
     * from this data. Returns a null QImage if the loading failed.
     */
    QImage image(int index = 0);

private:

    KExiv2PreviewsPriv* const d;
};

} // namespace KExiv2Iface

#endif // KEXIV2PREVIEWS
