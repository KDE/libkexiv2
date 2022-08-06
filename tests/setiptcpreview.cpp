/*
    A command line tool to set IPTC Preview

    SPDX-FileCopyrightText: 2009-2012 Gilles Caulier <caulier dot gilles at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt includes

#include <QTransform>
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
    QTransform matrix;
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
