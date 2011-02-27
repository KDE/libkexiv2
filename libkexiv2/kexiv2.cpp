/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-15
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2010 by Marcel Wiesweg
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

#include "kexiv2.h"
#include "kexiv2_p.h"

// Local includes.

#include "version.h"


namespace KExiv2Iface
{

KExiv2Data::KExiv2Data()
          : d(0)
{
}

KExiv2Data::KExiv2Data(const KExiv2Data& other)
{
    d = other.d;
}

KExiv2Data::~KExiv2Data()
{
}

KExiv2Data& KExiv2Data::operator=(const KExiv2Data& other)
{
    d = other.d;
    return *this;
}

KExiv2::KExiv2()
      : d(new KExiv2Priv)
{
}

KExiv2::KExiv2(const KExiv2& metadata)
      : d(new KExiv2Priv)
{
    d->data = metadata.d->data;

    setFilePath(metadata.getFilePath());
}

KExiv2::KExiv2(const KExiv2Data& data)
      : d(new KExiv2Priv)
{
    setData(data);
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
    d->data = metadata.d->data;

    setFilePath(metadata.getFilePath());
    return *this;
}

//-- Statics methods ----------------------------------------------

bool KExiv2::initializeExiv2()
{
#ifdef _XMP_SUPPORT_

    if (!Exiv2::XmpParser::initialize())
        return false;

    registerXmpNameSpace(QString("http://ns.adobe.com/lightroom/1.0/"), QString("lr"));
    registerXmpNameSpace(QString("http://www.digikam.org/ns/kipi/1.0/"), QString("kipi"));

#endif // _XMP_SUPPORT_

    return true;
}

bool KExiv2::cleanupExiv2()
{
    // Fix memory leak if Exiv2 support XMP.
#ifdef _XMP_SUPPORT_

    unregisterXmpNameSpace(QString("http://ns.adobe.com/lightroom/1.0/"));
    unregisterXmpNameSpace(QString("http://www.digikam.org/ns/kipi/1.0/"));

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
#if (EXIV2_TEST_VERSION(0,19,1))
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

//-- General methods ----------------------------------------------

KExiv2Data KExiv2::data() const
{
    KExiv2Data data;
    data.d = d->data;
    return data;
}

void KExiv2::setData(const KExiv2Data& data)
{
    if (data.d)
    {
        d->data = data.d;
    }
    else
    {
        // KExiv2Data can have a null pointer,
        // but we never want a null pointer in KExiv2Priv.
        d->data->clear();
    }
}

bool KExiv2::load(const QByteArray& imgData) const
{
    return loadFromData(imgData);
}

bool KExiv2::loadFromData(const QByteArray& imgData) const
{
    if (imgData.isEmpty())
        return false;

    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((Exiv2::byte*)imgData.data(), imgData.size());

        d->filePath.clear();
        image->readMetadata();

        // Size and mimetype ---------------------------------

        d->pixelSize = QSize(image->pixelWidth(), image->pixelHeight());
        d->mimeType  = image->mimeType().c_str();

        // Image comments ---------------------------------

        d->imageComments() = image->comment();

        // Exif metadata ----------------------------------

        d->exifMetadata() = image->exifData();

        // Iptc metadata ----------------------------------

        d->iptcMetadata() = image->iptcData();

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------

        d->xmpMetadata() = image->xmpData();

#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::load(const QString& filePath) const
{
    if (filePath.isEmpty())
    {
        return false;
    }

    // ensure that symlinks are used correctly
    QString fileName = filePath;
    QFileInfo info(fileName);
    if (info.isSymLink()) {
        kDebug() << "filePath" << filePath << "is a symlink."
                 << "Using target" << info.symLinkTarget();
        fileName = info.symLinkTarget();
    }

    try
    {
        Exiv2::Image::AutoPtr image;

        // If XMP sidecar exist and if we want manage it, parse it instead the image.
        if (d->useXMPSidecar4Reading)
        {
            QString xmpSidecarPath(fileName);
            xmpSidecarPath.replace(QRegExp("[^\\.]+$"), "xmp");
            kDebug() << "File path" << fileName;
            kDebug() << "XMP sidecar path" << xmpSidecarPath;
            QFileInfo xmpSidecarFileInfo(xmpSidecarPath);

            if (xmpSidecarFileInfo.exists() && xmpSidecarFileInfo.isReadable())
            {
                // TODO: We should rather read both image and sidecar metadata
                // and merge the two, with sidecar taking precedence
                image = Exiv2::ImageFactory::open((const char*)
                        (QFile::encodeName(xmpSidecarPath)));
            }
        }

        // No XMP sidecar file managed. We load image file metadata instead.
        if (!image.get())
        {
            if (!info.isReadable())
            {
                kDebug() << "File '" << info.fileName().toAscii().constData() << "' is not readable.";
                return false;
            }
            image = Exiv2::ImageFactory::open((const char*)(QFile::encodeName(fileName)));
        }

        d->filePath = fileName;
        image->readMetadata();

        // Size and mimetype ---------------------------------

        d->pixelSize = QSize(image->pixelWidth(), image->pixelHeight());
        d->mimeType  = image->mimeType().c_str();

        // Image comments ---------------------------------

        d->imageComments() = image->comment();

        // Exif metadata ----------------------------------

        d->exifMetadata() = image->exifData();

        // Iptc metadata ----------------------------------

        d->iptcMetadata() = image->iptcData();

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------

        d->xmpMetadata() = image->xmpData();

#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::save(const QString& imageFilePath) const
{
    // NOTE: see B.K.O #137770 & #138540 : never touch the file if is read only.
    QFileInfo finfo(imageFilePath);
    QFileInfo dinfo(finfo.path());
    if (!dinfo.isWritable())
    {
        kDebug() << "Dir '" << dinfo.filePath().toAscii().constData() << "' is read-only. Metadata not saved.";
        return false;
    }

    bool ret = false;

    switch(d->metadataWritingMode)
    {
        case WRITETOSIDECARONLY:
            ret = d->saveToXMPSidecar(imageFilePath);
            if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to XMP sidecar.";
            break;

        case WRITETOSIDECARANDIMAGE:
            ret = d->saveToFile(finfo);
            if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to file.";

            ret |=  d->saveToXMPSidecar(imageFilePath);
            if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to XMP sidecar.";
            break;

        case WRITETOSIDECARONLY4READONLYFILES:
            ret = d->saveToFile(finfo);
            if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to file.";
            if (!ret)
            {
                ret |= d->saveToXMPSidecar(imageFilePath);
                if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to XMP sidecar.";
            }
            break;

        default:          // WRITETOIMAGEONLY:
            ret = d->saveToFile(finfo);
            if (ret) kDebug() << "Metadata for file '" << finfo.fileName().toAscii().constData() << "' written to file.";
            break;
    }

    return ret;
}

bool KExiv2::applyChanges() const
{
    if (d->filePath.isEmpty())
    {
        kDebug() << "Failed to apply changes: file path is empty!";
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

QSize KExiv2::getPixelSize() const
{
    return d->pixelSize;
}

QString KExiv2::getMimeType() const
{
    return d->mimeType;
}

void KExiv2::setWriteRawFiles(const bool on)
{
    d->writeRawFiles = on;
}

bool KExiv2::writeRawFiles() const
{
    return d->writeRawFiles;
}

void KExiv2::setUseXMPSidecar4Reading(const bool on)
{
    d->useXMPSidecar4Reading = on;
}

bool KExiv2::useXMPSidecar4Reading() const
{
    return d->useXMPSidecar4Reading;
}

void KExiv2::setMetadataWritingMode(const int mode)
{
    d->metadataWritingMode = mode;
}

int KExiv2::metadataWritingMode() const
{
    return d->metadataWritingMode;
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
