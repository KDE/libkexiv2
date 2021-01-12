/*
    SPDX-FileCopyrightText: 2009-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2009-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KEXIV2PREVIEWS
#define KEXIV2PREVIEWS

// Std

#include <memory>

// Qt includes

#include <QByteArray>
#include <QSize>
#include <QString>

// Local includes

#include "libkexiv2_export.h"

class QImage;

namespace KExiv2Iface
{

/**
 * @class KExiv2Previews kexiv2previews.h <KExiv2/KExiv2Previews>
 *
 * KExiv2Previews
 */
class LIBKEXIV2_EXPORT KExiv2Previews
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
    std::unique_ptr<class KExiv2PreviewsPrivate> const d;
};

} // namespace KExiv2Iface

#endif // KEXIV2PREVIEWS
