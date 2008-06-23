/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "version.h"
#include "kexiv2private.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

KExiv2::KExiv2()
{
    d = new KExiv2Priv;
}

KExiv2::KExiv2(const KExiv2& metadata)
{
    d = new KExiv2Priv;

    // No need to use QT containers transormations here. We can use original objects directly.
    d->imageComments = metadata.d->imageComments;
    d->exifMetadata  = metadata.d->exifMetadata;
    d->iptcMetadata  = metadata.d->iptcMetadata;

#ifdef _XMP_SUPPORT_
    d->xmpMetadata   = metadata.d->xmpMetadata;
#endif // _XMP_SUPPORT_

    setFilePath(metadata.getFilePath());
}

KExiv2::KExiv2(const QString& filePath)
{
    d = new KExiv2Priv;
    load(filePath);
}

KExiv2::~KExiv2()
{
    delete d;

    // Fix memory leak if Exiv2 support XMP.
#ifdef _XMP_SUPPORT_
    Exiv2::XmpParser::terminate();
#endif // _XMP_SUPPORT_
}

KExiv2& KExiv2::operator=(const KExiv2& metadata)
{
    // No need to use QT containers transormations here. We can use original objects directly.
    d->imageComments = metadata.d->imageComments;
    d->exifMetadata  = metadata.d->exifMetadata;
    d->iptcMetadata  = metadata.d->iptcMetadata;

#ifdef _XMP_SUPPORT_
    d->xmpMetadata   = metadata.d->xmpMetadata;
#endif // _XMP_SUPPORT_

    setFilePath(metadata.getFilePath());
    return *this;
}

//-- Statics methods ----------------------------------------------

bool KExiv2::supportXmp()
{
#ifdef _XMP_SUPPORT_
    return true;
#else
    return false;
#endif // _XMP_SUPPORT_
}

QString KExiv2::Exiv2Version()
{
    // Since 0.14.0 release, we can extract run-time version of Exiv2.
    // else we return make version.

#if (EXIV2_TEST_VERSION(0,14,0))
    return QString(Exiv2::version());
#else
    return QString("%1.%2.%3").arg(EXIV2_MAJOR_VERSION)
                              .arg(EXIV2_MINOR_VERSION)
                              .arg(EXIV2_PATCH_VERSION);
#endif
}

QString KExiv2::version()
{
    return QString(kexiv2_version);
}

bool KExiv2::isReadOnly(const QString& filePath)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        Exiv2::AccessMode mode;
        mode = image->checkMode(Exiv2::mdComment);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) 
            return false;

        mode = image->checkMode(Exiv2::mdExif);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) 
            return false;

        mode = image->checkMode(Exiv2::mdIptc);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) 
            return false;

#ifdef _XMP_SUPPORT_
        mode = image->checkMode(Exiv2::mdXmp);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) 
            return false;
#endif // _XMP_SUPPORT_

    }
    catch( Exiv2::Error &e )
    {
        std::string s(e.what());
        qDebug("%s (Error #%i: %s)", "Cannot check metadata access mode using Exiv2 ", e.code(), s.c_str());
    }

    return true;
}

//-- General methods ----------------------------------------------

bool KExiv2::load(const QString& filePath) const
{
    QFileInfo finfo(filePath);
    if (filePath.isEmpty() || !finfo.isReadable())
    {
        qDebug("File '%s' is not readable.", finfo.fileName().toAscii().constData());
        return false;
    }

    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));
        image->readMetadata();

        // Image comments ---------------------------------

        d->imageComments = image->comment();

        // Exif metadata ----------------------------------

        d->exifMetadata = image->exifData();

        // Iptc metadata ----------------------------------

        d->iptcMetadata = image->iptcData();

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------

        d->xmpMetadata = image->xmpData();

#endif // _XMP_SUPPORT_

        d->filePath = filePath;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::save(const QString& filePath) const
{
    if (filePath.isEmpty())
        return false;

    // NOTE: see B.K.O #137770 & #138540 : never touch the file if is read only.
    QFileInfo finfo(filePath);
    QFileInfo dinfo(finfo.path());
    if (!finfo.isWritable())
    {
        qDebug("File '%s' is read-only. Metadata not saved.", finfo.fileName().toAscii().constData());
        return false;
    }
    if (!dinfo.isWritable())
    {
        qDebug("Dir '%s' is read-only. Metadata not saved.", dinfo.filePath().toAscii().constData());
        return false;
    }

    try
    {
        Exiv2::AccessMode mode;
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        // Image Comments ---------------------------------

        mode = image->checkMode(Exiv2::mdComment);
        if (!d->imageComments.empty() && 
            (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite))
        {
            image->setComment(d->imageComments);
        }

        // Exif metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdExif);
        if (!d->exifMetadata.empty() && 
            (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite))
        {
            image->setExifData(d->exifMetadata);
        }

        // Iptc metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdIptc);
        if (!d->iptcMetadata.empty() && 
            (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite))
        {
            image->setIptcData(d->iptcMetadata);
        }

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------

        mode = image->checkMode(Exiv2::mdXmp);
        if (!d->xmpMetadata.empty() && 
            (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite))
        {
            image->setXmpData(d->xmpMetadata);
        }

#endif // _XMP_SUPPORT_

        image->writeMetadata();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot save metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::applyChanges() const
{
    return save(d->filePath);
}

bool KExiv2::isEmpty() const
{
    if (!hasComments() && !hasExif() && !hasIptc() && !hasXmp())
        return true;

    return false;
}

void KExiv2::setFilePath(const QString& path)
{
    d->filePath = path;
}

QString KExiv2::getFilePath() const
{
    return d->filePath;
}

bool KExiv2::setProgramId(bool /*on*/) const
{
    return true;
}

}  // NameSpace KExiv2Iface
