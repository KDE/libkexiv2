/*
    A command line tool to test XMP sidecar functionality

    SPDX-FileCopyrightText: 2010 Jakob Malm <jakob dot malm at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt includes

#include <QString>
#include <QFile>
#include <QDebug>

// Local includes

#include "kexiv2.h"

using namespace KExiv2Iface;

int main (int argc, char **argv)
{
    if(argc != 2)
    {
        qDebug() << "readimagewritecmpsidecar - read metadata from image and write to XMP sidecar";
        qDebug() << "Usage: <image>";
        return -1;
    }

    QString filePath = QString::fromLocal8Bit(argv[1]);

    KExiv2 meta;

    // Read metadata from the image
    meta.load(filePath);

    // Write metadata to XMP sidecar
    meta.setMetadataWritingMode(KExiv2::WRITETOSIDECARONLY);
    meta.save(filePath);

    return 0;
}
