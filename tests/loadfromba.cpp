/*
    A command line tool to load metadata from byte array

    SPDX-FileCopyrightText: 2009-2012 Gilles Caulier <caulier dot gilles at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt includes

#include <QDataStream>
#include <QImage>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QDebug>

// Local includes

#include "kexiv2.h"

using namespace KExiv2Iface;

int main (int argc, char **argv)
{
    if(argc != 2)
    {
        qDebug() << "loadfromba - test to load metadata from image as byte array";
        qDebug() << "Usage: <image>";
        return -1;
    }

    QString filePath = QString::fromLocal8Bit(argv[1]);
    QString baFile(QStringLiteral("ba.dat"));

    QImage image(filePath);
    image.save(baFile, "PNG");

    QFile file(baFile);
    if ( !file.open(QIODevice::ReadOnly) )
        return false;

    QByteArray data;
    data.resize(file.size());
    QDataStream stream( &file );
    stream.readRawData(data.data(), data.size());
    file.close();

    KExiv2 meta;
    meta.loadFromData(data);

    return 0;
}
