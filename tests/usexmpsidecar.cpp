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
        qDebug() << "usexmpsidecar - read from and write to XMP sidecar";
        qDebug() << "Usage: <image>";
        return -1;
    }

    QString filePath(QString::fromLatin1(argv[1]));

    KExiv2 meta;
    meta.setUseXMPSidecar4Reading(true);
    meta.load(filePath);
    // print some metadata
    // add some metadata
    // write changed metadata
    // perhaps check to see if image file or XMP sidecar file was changed

    return 0;
}
