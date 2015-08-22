/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-15
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2006-2015 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2013 by Marcel Wiesweg
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

// KDE includes

#include <klibloader.h>

// Local includes

#include "version.h"

static const KCatalogLoader loader("libkexiv2");

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

// -------------------------------------------------------------------------------------------

KExiv2::KExiv2()
    : d(new Private)
{
}

KExiv2::KExiv2(const KExiv2& metadata)
    : d(new Private)
{
    d->copyPrivateData(metadata.d);
}

KExiv2::KExiv2(const KExiv2Data& data)
    : d(new Private)
{
    setData(data);
}

KExiv2::KExiv2(const QString& filePath)
    : d(new Private)
{
    load(filePath);
}

KExiv2::~KExiv2()
{
    delete d;
}

KExiv2& KExiv2::operator=(const KExiv2& metadata)
{
    d->copyPrivateData(metadata.d);

    return *this;
}

//-- Statics methods ----------------------------------------------

bool KExiv2::initializeExiv2()
{
#ifdef _XMP_SUPPORT_

    if (!Exiv2::XmpParser::initialize())
        return false;

    registerXmpNameSpace(QString("http://ns.adobe.com/lightroom/1.0/"),  QString("lr"));
    registerXmpNameSpace(QString("http://www.digikam.org/ns/kipi/1.0/"), QString("kipi"));
    registerXmpNameSpace(QString("http://ns.microsoft.com/photo/1.2/"),  QString("MP"));
    registerXmpNameSpace(QString("http://ns.acdsee.com/iptc/1.0/"),      QString("acdsee"));
    registerXmpNameSpace(QString("http://www.video"),                    QString("video"));

#endif // _XMP_SUPPORT_

    return true;
}

bool KExiv2::cleanupExiv2()
{
    // Fix memory leak if Exiv2 support XMP.
#ifdef _XMP_SUPPORT_

    unregisterXmpNameSpace(QString("http://ns.adobe.com/lightroom/1.0/"));
    unregisterXmpNameSpace(QString("http://www.digikam.org/ns/kipi/1.0/"));
    unregisterXmpNameSpace(QString("http://ns.microsoft.com/photo/1.2/"));
    unregisterXmpNameSpace(QString("http://ns.acdsee.com/iptc/1.0/"));
    unregisterXmpNameSpace(QString("http://www.video"));

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
        return true;
    }
    else if (typeMime == QString("image/png"))
    {
        return true;
    }
    else if (typeMime == QString("image/jp2"))
    {
        return true;
    }
    else if (typeMime == QString("image/x-raw"))
    {
        return true;
    }
    else if (typeMime == QString("image/pgf"))
    {
        return true;
    }

    return false;
}

QString KExiv2::Exiv2Version()
{
    // Since 0.14.0 release, we can extract run-time version of Exiv2.
    // else we return make version.

    return QString(Exiv2::version());
}

QString KExiv2::version()
{
    return QString(kexiv2_version);
}

QString KExiv2::sidecarFilePathForFile(const QString& path)
{
    QString ret;
    if (!path.isEmpty())
    {
        ret = path + QString(".xmp");
    }
    return ret;
}

KUrl KExiv2::sidecarUrl(const KUrl& url)
{
    QString sidecarPath = sidecarFilePathForFile(url.path());
    KUrl sidecarUrl(url);
    sidecarUrl.setPath(sidecarPath);
    return sidecarUrl;
}

KUrl KExiv2::sidecarUrl(const QString& path)
{
    return KUrl::fromPath(sidecarFilePathForFile(path));
}

QString KExiv2::sidecarPath(const QString& path)
{
    return sidecarFilePathForFile(path);
}

bool KExiv2::hasSidecar(const QString& path)
{
    return QFileInfo(sidecarFilePathForFile(path)).exists();
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
        // but we never want a null pointer in Private.
        d->data->clear();
    }
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
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::load(const QString& filePath) const
{
    if (filePath.isEmpty())
    {
        return false;
    }

    d->filePath      = filePath;
    bool hasLoaded   = false;

    try
    {
        Exiv2::Image::AutoPtr image;

        image        = Exiv2::ImageFactory::open((const char*)(QFile::encodeName(filePath)));

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

        hasLoaded = true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot load metadata from file ", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

#ifdef _XMP_SUPPORT_
    try
    {
        if (d->useXMPSidecar4Reading)
        {
            QString xmpSidecarPath = sidecarFilePathForFile(filePath);
            QFileInfo xmpSidecarFileInfo(xmpSidecarPath);

            Exiv2::Image::AutoPtr xmpsidecar;
            if (xmpSidecarFileInfo.exists() && xmpSidecarFileInfo.isReadable())
            {
                // Read sidecar data
                xmpsidecar = Exiv2::ImageFactory::open((const char*)QFile::encodeName(xmpSidecarPath));
                xmpsidecar->readMetadata();

                // Merge
                d->loadSidecarData(xmpsidecar);
                hasLoaded = true;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot load XMP sidecar", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

#endif // _XMP_SUPPORT_

    return hasLoaded;
}

bool KExiv2::save(const QString& imageFilePath) const
{
    // If our image is really a symlink, we should follow the symlink so that
    // when we delete the file and rewrite it, we are honoring the symlink
    // (rather than just deleting it and putting a file there).

    // However, this may be surprising to the user when they are writing sidecar
    // files.  They might expect them to show up where the symlink is.  So, we
    // shouldn't follow the link when figuring out what the filename for the
    // sidecar should be.

    // Note, we are not yet handling the case where the sidecar itself is a
    // symlink.
    QString regularFilePath = imageFilePath; // imageFilePath might be a
                                             // symlink.  Below we will change
                                             // regularFile to the pointed to
                                             // file if so.
    QFileInfo givenFileInfo(imageFilePath);
    if (givenFileInfo.isSymLink())
    {
        kDebug() << "filePath" << imageFilePath << "is a symlink."
                 << "Using target" << givenFileInfo.canonicalPath();

        regularFilePath = givenFileInfo.canonicalPath();// Walk all the symlinks
    }

    // NOTE: see bug #137770 & #138540 : never touch the file if is read only.
    QFileInfo finfo(regularFilePath);
    QFileInfo dinfo(finfo.path());
    if (!dinfo.isWritable())
    {
        kDebug() << "Dir '" << dinfo.filePath() << "' is read-only. Metadata not saved.";
        return false;
    }

    bool writeToFile                     = false;
    bool writeToSidecar                  = false;
    bool writeToSidecarIfFileNotPossible = false;
    bool writtenToFile                   = false;
    bool writtenToSidecar                = false;

    kDebug() << "KExiv2::metadataWritingMode" << d->metadataWritingMode;

    switch(d->metadataWritingMode)
    {
        case WRITETOSIDECARONLY:
            writeToSidecar = true;
            break;
        case WRITETOIMAGEONLY:
            writeToFile    = true;
            break;
        case WRITETOSIDECARANDIMAGE:
            writeToFile    = true;
            writeToSidecar = true;
            break;
        case WRITETOSIDECARONLY4READONLYFILES:
            writeToFile = true;
            writeToSidecarIfFileNotPossible = true;
            break;
    }

    if (writeToFile)
    {
        kDebug() << "Will write Metadata to file" << finfo.fileName();
        writtenToFile = d->saveToFile(finfo);
        if (writeToFile)
        {
            kDebug() << "Metadata for file" << finfo.fileName() << "written to file.";
        }
    }

    if (writeToSidecar || (writeToSidecarIfFileNotPossible && !writtenToFile))
    {
        kDebug() << "Will write XMP sidecar for file" << givenFileInfo.fileName();
        writtenToSidecar = d->saveToXMPSidecar(imageFilePath);
        if (writtenToSidecar)
        {
            kDebug() << "Metadata for file '" << givenFileInfo.fileName() << "' written to XMP sidecar.";
        }
    }

    return writtenToFile || writtenToSidecar;
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
