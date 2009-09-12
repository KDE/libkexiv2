/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// C ANSI includes.

extern "C"
{
#include <sys/stat.h>
#include <utime.h>
}

// Local includes.

#include "version.h"
#include "kexiv2_p.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

KExiv2::KExiv2()
      : d(new KExiv2Priv)
{
}

KExiv2::KExiv2(const KExiv2& metadata)
      : d(new KExiv2Priv)
{
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
      : d(new KExiv2Priv)

{
    load(filePath);
}

KExiv2::~KExiv2()
{
    delete d;
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

bool KExiv2::initializeExiv2()
{
#ifdef _XMP_SUPPORT_
    return (Exiv2::XmpParser::initialize());
#endif // _XMP_SUPPORT_

    return true;
}

bool KExiv2::cleanupExiv2()
{
    // Fix memory leak if Exiv2 support XMP.
#ifdef _XMP_SUPPORT_
    Exiv2::XmpParser::terminate();
#endif // _XMP_SUPPORT_

    return true;
}

bool KExiv2::supportXmp()
{
#ifdef _XMP_SUPPORT_
    return true;
#else
    return false;
#endif // _XMP_SUPPORT_
}

bool KExiv2::supportMetadataWritting(const QString& typeMime)
{
    if (typeMime == QString("image/jpeg"))
    {
        return true;
    }
    else if (typeMime == QString("image/tiff"))
    {
#if (EXIV2_TEST_VERSION(0,17,91))
        return true;
#else
        return false;
#endif
    }
    else if (typeMime == QString("image/png"))
    {
#if (EXIV2_TEST_VERSION(0,17,91))
        return true;
#else
        return false;
#endif
    }
    else if (typeMime == QString("image/jp2"))
    {
#if (EXIV2_TEST_VERSION(0,17,91))
        return true;
#else
        return false;
#endif
    }
    else if (typeMime == QString("image/x-raw"))
    {
#if (EXIV2_TEST_VERSION(0,17,91))
        return true;
#else
        return false;
#endif
    }
    else if (typeMime == QString("image/pgf"))
    {
#if (EXIV2_TEST_VERSION(0,19,0))
        return true;
#else
        return false;
#endif
    }

    return false;
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
    if (!canWriteComment(filePath))
        return true;

    if (!canWriteExif(filePath))
        return true;

    if (!canWriteIptc(filePath))
        return true;

#ifdef _XMP_SUPPORT_
    if (!canWriteXmp(filePath))
        return true;
#endif // _XMP_SUPPORT_

    return false;
}

//-- General methods ----------------------------------------------

bool KExiv2::load(const QByteArray& imgData) const
{
    if (imgData.isEmpty())
        return false;

    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((Exiv2::byte*)imgData.data(), imgData.size());

        d->filePath = QString();
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

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::load(const QString& filePath) const
{
    QFileInfo finfo(filePath);
    if (filePath.isEmpty() || !finfo.isReadable())
    {
        kDebug(51003) << "File '" << finfo.fileName().toAscii().constData() << "' is not readable." << endl;
        return false;
    }

    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        d->filePath = filePath;
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
        kDebug(51003) << "File '" << finfo.fileName().toAscii().constData() << "' is read-only. Metadata not saved." << endl;
        return false;
    }
    if (!dinfo.isWritable())
    {
        kDebug(51003) << "Dir '" << dinfo.filePath().toAscii().constData() << "' is read-only. Metadata not saved." << endl;
        return false;
    }

    // TIFF/EP Raw file based supported by Exiv2 0.18 are : DNG, NEF, PEF.
    QStringList rawTiffBasedSupported = QStringList()
        << "dng" << "nef" << "pef";
    QStringList rawTiffBasedNotSupported = QStringList()
        << "3fr" << "arw" << "cr2" << "dcr" << "erf" << "k25"
        << "kdc" << "mos" << "orf" << "raw" << "sr2" << "srf";
    QString ext = finfo.suffix().toLower();
    if (rawTiffBasedNotSupported.contains(ext))
    {
        kDebug(51003) << finfo.fileName()
                      << "is TIFF based RAW file not yet supported. Metadata not saved." << endl;
        return false;
    }

    if (rawTiffBasedSupported.contains(ext) && !d->writeRawFiles)
    {
        kDebug(51003) << finfo.fileName()
                      << "is TIFF based RAW file supported but writing mode is disabled. "
                      << "Metadata not saved." << endl;
        return false;
    }
    kDebug(51003) << "File Extension: " << ext << " is supported for writing mode" << endl;

    try
    {
        Exiv2::AccessMode mode;
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        // We need to load target file metadata to merge with new one. It's mandatory with TIFF format: 
        // like all tiff file structure is based on Exif.
        image->readMetadata();

        // Image Comments ---------------------------------

        mode = image->checkMode(Exiv2::mdComment);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        {
            image->setComment(d->imageComments);
        }

        // Exif metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdExif);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        {
            if (image->mimeType() == "image/tiff")
            {
                Exiv2::ExifData orgExif = image->exifData();
                Exiv2::ExifData newExif;
                QStringList     untouchedTags;

                // With tiff image we cannot overwrite whole Exif data as well, because
                // image data are stored in Exif container. We need to take a care about
                // to not lost image data.
                untouchedTags << "Exif.Image.ImageWidth";
                untouchedTags << "Exif.Image.ImageLength";
                untouchedTags << "Exif.Image.BitsPerSample";
                untouchedTags << "Exif.Image.Compression";
                untouchedTags << "Exif.Image.PhotometricInterpretation";
                untouchedTags << "Exif.Image.FillOrder";
                untouchedTags << "Exif.Image.SamplesPerPixel";
                untouchedTags << "Exif.Image.StripOffsets";
                untouchedTags << "Exif.Image.RowsPerStrip";
                untouchedTags << "Exif.Image.StripByteCounts";
                untouchedTags << "Exif.Image.XResolution";
                untouchedTags << "Exif.Image.YResolution";
                untouchedTags << "Exif.Image.PlanarConfiguration";
                untouchedTags << "Exif.Image.ResolutionUnit";

                for (Exiv2::ExifData::iterator it = orgExif.begin(); it != orgExif.end(); ++it)
                {
                    if (untouchedTags.contains(it->key().c_str()))
                    {
                        newExif[it->key().c_str()] = orgExif[it->key().c_str()];
                    }
                }

                for (Exiv2::ExifData::iterator it = d->exifMetadata.begin(); it != d->exifMetadata.end(); ++it)
                {
                    if (!untouchedTags.contains(it->key().c_str()))
                    {
                        newExif[it->key().c_str()] = d->exifMetadata[it->key().c_str()];
                    }
                }

                image->setExifData(newExif);
            }
            else
            {
                image->setExifData(d->exifMetadata);
            }
        }

        // Iptc metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdIptc);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        {
            image->setIptcData(d->iptcMetadata);
        }

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------

        mode = image->checkMode(Exiv2::mdXmp);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        {
            image->setXmpData(d->xmpMetadata);
        }

#endif // _XMP_SUPPORT_

        if (!d->updateFileTimeStamp)
        {
            // NOTE: Don't touch access and modification timestamp of file.
            struct stat st;
            ::stat(QFile::encodeName(filePath), &st);

            struct utimbuf ut;
            ut.modtime = st.st_mtime;
            ut.actime  = st.st_atime;

            image->writeMetadata();

            ::utime(QFile::encodeName(filePath), &ut);
        }
        else
        {
            image->writeMetadata();
        }

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
    if (d->filePath.isEmpty())
    {
        kDebug(51003) << "Failed to apply changes: file path is empty!" << endl;
        return false;
    }

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

void KExiv2::setWriteRawFiles(bool on)
{
    d->writeRawFiles = on;
}

bool KExiv2::writeRawFiles() const
{
    return d->writeRawFiles;
}

void KExiv2::setUpdateFileTimeStamp(bool on)
{
    d->updateFileTimeStamp = on;
}

bool KExiv2::updateFileTimeStamp() const
{
    return d->updateFileTimeStamp;
}

bool KExiv2::setProgramId(bool /*on*/) const
{
    return true;
}

}  // NameSpace KExiv2Iface
