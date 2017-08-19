/** ===========================================================
 *
 * This file is a part of KDE project
 *
 *
 * @date   2009-02-04
 * @brief  a command line tool to set IPTC Preview
 *
 * @author Copyright (C) 2009-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

// Qt includes

#include <QMatrix>
#include <QImage>
#include <QString>
#include <QFile>
#include <QDebug>

// Local includes

#include "kexiv2.h"

using namespace KExiv2Iface;

int main (int argc, char **argv)
{
    if (argc != 2)
    {
        qDebug() << "setiptcpreview - update/add jpeg iptc preview to image";
        qDebug() << "Usage: <image>";
        return -1;
    }

    QImage  preview;
    QString filePath = QString::fromLocal8Bit(argv[1]);
    KExiv2  meta(filePath);

    QImage  image(filePath);
    QMatrix matrix;
    matrix.rotate(90);
    image = image.transformed(matrix);

    QSize previewSize = image.size();
    previewSize.scale(1280, 1024, Qt::KeepAspectRatio);

    // Ensure that preview is not upscaled
    if (previewSize.width() >= (int)image.width())
        preview = image.copy();
    else
        preview = image.scaled(previewSize.width(), previewSize.height(), Qt::IgnoreAspectRatio).copy();

    meta.setImagePreview(preview);
    meta.applyChanges();

    QImage preview2;
    KExiv2 meta2(filePath);
    meta2.getImagePreview(preview2);
    preview2.save(QString::fromLatin1("preview.png"), "PNG");

    return 0;
}
