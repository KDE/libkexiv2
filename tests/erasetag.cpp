/*
    A command line tool to tag from photo

    SPDX-FileCopyrightText: 2009-2012 Gilles Caulier <caulier dot gilles at gmail dot com>

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
        qDebug() << "erasetag - erase tag from from image";
        qDebug() << "Usage: <image>";
        return -1;
    }

    QString filePath = QString::fromLocal8Bit(argv[1]);

    KExiv2 meta;
    meta.load(filePath);
    meta.setWriteRawFiles(true);
    bool b = meta.removeExifTag("Exif.OlympusIp.BlackLevel", false);
    qDebug() << "Exif.OlympusIp.BlackLevel found = " << b;

    QByteArray ba = meta.getExifTagData("Exif.OlympusIp.BlackLevel");
    qDebug() << "Exif.OlympusIp.BlackLevel removed = " << ba.isEmpty();

    if (b)
    {
        meta.applyChanges();
    }

    return 0;
}
