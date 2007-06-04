/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

 // C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>

// Qt includes.

#include <QFile>
#include <QImage>
#include <QSize>
#include <QTextCodec>
#include <QMatrix>
#include <QFileInfo>

// KDE includes.

#include <ktempfile.h>
#include <kstringhandler.h>
#include <kdeversion.h>

// Exiv2 includes.

#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/datasets.hpp>
#include <exiv2/tags.hpp>
#include <exiv2/types.hpp>
#include <exiv2/exif.hpp>

// Local includes.

#include "kexiv2.h"

namespace KExiv2Iface
{

class KExiv2Priv
{
public:

    KExiv2Priv(){}

    QString         filePath;

    std::string     imageComments;

    Exiv2::ExifData exifMetadata;

    Exiv2::IptcData iptcMetadata;
};

KExiv2::KExiv2()
{
    d = new KExiv2Priv;
}

KExiv2::KExiv2(const QString& filePath)
{
    d = new KExiv2Priv;
    load(filePath);
}

KExiv2::~KExiv2()
{
    delete d;
}

QString KExiv2::Exiv2Version()
{
    return QString("%1.%2.%3").arg(EXIV2_MAJOR_VERSION)
                              .arg(EXIV2_MINOR_VERSION)
                              .arg(EXIV2_PATCH_VERSION);
}

void KExiv2::printExiv2ExceptionError(const QString& msg, Exiv2::Error& e)
{
    std::string s(e.what());
    qDebug("%s (%s)", msg.toAscii().constData(), s.c_str());
}

std::string& KExiv2::commentsMetaData()
{
    return d->imageComments;
}

Exiv2::ExifData& KExiv2::exifMetaData()
{
    return d->exifMetadata;
}

Exiv2::IptcData& KExiv2::iptcMetaData()
{
    return d->iptcMetadata;
}

bool KExiv2::clearComments()
{
    return setComments(QByteArray());
}

bool KExiv2::clearExif()
{
    try
    {
        d->exifMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot clear Exif data using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::clearIptc()
{
    try
    {
        d->iptcMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot clear Iptc data using Exiv2 ", e);
    }

    return false;
}

QString KExiv2::getFilePath() const
{
    return d->filePath;
}

QByteArray KExiv2::getComments() const
{
    return QByteArray(d->imageComments.data(), d->imageComments.size());
}

QString KExiv2::getCommentsDecoded() const
{
    return detectEncodingAndDecode(getCommentsString());
}

std::string KExiv2::getCommentsString() const
{
    return d->imageComments;
}

QByteArray KExiv2::getExif() const
{
    try
    {
        if (!d->exifMetadata.empty())
        {

            Exiv2::ExifData& exif = d->exifMetadata;
            Exiv2::DataBuf c2 = exif.copy();
            QByteArray data((const char*)c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot get Exif data using Exiv2 ", e);
    }

    return QByteArray();
}

QByteArray KExiv2::getIptc(bool addIrbHeader) const
{
    try
    {
        if (!d->iptcMetadata.empty())
        {
            Exiv2::IptcData& iptc = d->iptcMetadata;
            Exiv2::DataBuf c2;

            if (addIrbHeader) 
            {
#ifdef EXIV2_CHECK_VERSION 
                if (EXIV2_CHECK_VERSION(0,10,0))
                    c2 = Exiv2::Photoshop::setIptcIrb(0, 0, iptc);
                else
                {
                    qDebug("Exiv2 version is to old. Cannot add Irb header to IPTC metadata");
                    return QByteArray();
                }
#endif
            }
            else 
                c2 = iptc.copy();

            QByteArray data((const char*)c2.pData_, c2.size_);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot get Iptc data using Exiv2 ",e);
    }

    return QByteArray();
}

bool KExiv2::setComments(const QByteArray& data)
{
    d->imageComments = std::string(data.data(), data.size());
    return true;
}

bool KExiv2::setExif(const QByteArray& data)
{
    try
    {
        if (!data.isEmpty())
        {
            if (d->exifMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
		return false;
	    else		
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Exif data using Exiv2 ", e);
    }        

    return false;
}

bool KExiv2::setIptc(const QByteArray& data)
{
    try
    {    
        if (!data.isEmpty())
        {
            if (d->iptcMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
		return false;
	    else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Iptc data using Exiv2 ", e);
    }    

    return false;    
}

bool KExiv2::setExif(Exiv2::DataBuf const data)
{
    try
    {    
        if (data.size_ != 0)
        {
            if (d->exifMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Exif data using Exiv2 ", e);
    }    

    return false;    
}

bool KExiv2::setIptc(Exiv2::DataBuf const data)
{
    try
    {    
        if (data.size_ != 0)
        {
            if (d->iptcMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Iptc data using Exiv2 ", e);
    }        

    return false;
}

bool KExiv2::load(const QString& filePath)
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

        d->filePath = filePath;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }
    
    return false;
}

bool KExiv2::save(const QString& filePath)
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
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));
        
        // Image Comments ---------------------------------
        
        if (!d->imageComments.empty())
        {
            image->setComment(d->imageComments);
        }

        // Exif metadata ----------------------------------
        
        if (!d->exifMetadata.empty())
        {
            image->setExifData(d->exifMetadata);
        }

        // Iptc metadata ----------------------------------
        
        if (!d->iptcMetadata.empty())
        {
            image->setIptcData(d->iptcMetadata);
        }
    
        image->writeMetadata();

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot save metadata using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::applyChanges()
{
    return save(d->filePath);
}

bool KExiv2::isReadOnly(const QString& filePath)
{
    QFileInfo fi(filePath);
    QString ext = fi.completeSuffix().toUpper();

    if (ext != QString("JPG") && ext != QString("JPEG") && ext != QString("JPE"))
        return true;

    return false;
}

bool KExiv2::setImageProgramId(const QString& program, const QString& version)
{
    try
    {
        // Record program info in Exif.Image.ProcessingSoftware tag (only available with Exiv2 >= 0.14.0).

#ifdef EXIV2_CHECK_VERSION 
        if (EXIV2_CHECK_VERSION(0,14,0))
        {
            QString software(program);
            software.append("-");
            software.append(version);
            d->exifMetadata["Exif.Image.ProcessingSoftware"] = software.toAscii().constData();
        }
#endif

        // See B.K.O #142564: Check if Exif.Image.Software already exist. If yes, do not touch this tag.

        if (!d->exifMetadata.empty())
        {
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifKey key("Exif.Image.Software");
            Exiv2::ExifData::iterator it = exifData.findKey(key);

            if (it == exifData.end())
            {
                QString software(program);
                software.append("-");
                software.append(version);
                d->exifMetadata["Exif.Image.Software"]      = software.toAscii().constData();
            }
	}

	// Record program info in IPTC tags.

        d->iptcMetadata["Iptc.Application2.Program"]        = program.toAscii().constData();
        d->iptcMetadata["Iptc.Application2.ProgramVersion"] = version.toAscii().constData();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Program identity into image using Exiv2 ", e);
    }

    return false;
}

QSize KExiv2::getImageDimensions() const
{
    if (d->exifMetadata.empty())
        return QSize();

    try
    {
        long width=-1, height=-1;

        // Try to get Exif.Photo tags

        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifKey key("Exif.Photo.PixelXDimension");
        Exiv2::ExifData::iterator it = exifData.findKey(key);

        if (it != exifData.end())
            width = it->toLong();

        Exiv2::ExifKey key2("Exif.Photo.PixelYDimension");
        Exiv2::ExifData::iterator it2 = exifData.findKey(key2);

        if (it2 != exifData.end())
            height = it2->toLong();

        if (width != -1 && height != -1)
            return QSize(width, height);

        // Try to get Exif.Image tags

        width  = -1;
        height = -1;

        Exiv2::ExifKey key3("Exif.Image.ImageWidth");
        Exiv2::ExifData::iterator it3 = exifData.findKey(key3);

        if (it3 != exifData.end())
            width = it3->toLong();

        Exiv2::ExifKey key4("Exif.Image.ImageLength");
        Exiv2::ExifData::iterator it4 = exifData.findKey(key4);

        if (it4 != exifData.end())
            height = it4->toLong();

        if (width != -1 && height != -1)
            return QSize(width, height);
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot parse image dimensions tag using Exiv2 ", e);
    }

    return QSize();
}

bool KExiv2::setImageDimensions(const QSize& size, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // NOTE: see B.K.O #144604: you a cast to record an unsigned integer value.
        d->exifMetadata["Exif.Image.ImageWidth"]      = static_cast<uint32_t>(size.width());
        d->exifMetadata["Exif.Image.ImageLength"]     = static_cast<uint32_t>(size.height());
        d->exifMetadata["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(size.width());
        d->exifMetadata["Exif.Photo.PixelYDimension"] = static_cast<uint32_t>(size.height());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set image dimensions using Exiv2 ", e);
    }

    return false;
}

QImage KExiv2::getExifThumbnail(bool fixOrientation) const
{
    QImage thumbnail;

    if (d->exifMetadata.empty())
       return thumbnail;

    try
    {
        Exiv2::DataBuf const c1(d->exifMetadata.copyThumbnail());
        thumbnail.loadFromData(c1.pData_, c1.size_);

        if (!thumbnail.isNull())
        {
            if (fixOrientation)
            {
                Exiv2::ExifKey key("Exif.Thumbnail.Orientation");
                Exiv2::ExifData exifData(d->exifMetadata);
                Exiv2::ExifData::iterator it = exifData.findKey(key);
                if (it != exifData.end())
                {
                    QMatrix matrix;
                    long orientation = it->toLong();
                    qDebug("Exif Thumbnail Orientation: %i", (int)orientation);

                    switch (orientation) 
                    {
                        case ORIENTATION_HFLIP:
                            matrix.scale(-1, 1);
                            break;

                        case ORIENTATION_ROT_180:
                            matrix.rotate(180);
                            break;

                        case ORIENTATION_VFLIP:
                            matrix.scale(1, -1);
                            break;

                        case ORIENTATION_ROT_90_HFLIP:
                            matrix.scale(-1, 1);
                            matrix.rotate(90);
                            break;

                        case ORIENTATION_ROT_90:
                            matrix.rotate(90);
                            break;

                        case ORIENTATION_ROT_90_VFLIP:
                            matrix.scale(1, -1);
                            matrix.rotate(90);
                            break;

                        case ORIENTATION_ROT_270:
                            matrix.rotate(270);
                            break;

                        default:
                            break;
                    }

                    if ( orientation != ORIENTATION_NORMAL )
                        thumbnail = thumbnail.transformed( matrix );
                }

                return thumbnail;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get Exif Thumbnail using Exiv2 ", e);
    }

    return thumbnail;
}

bool KExiv2::setExifThumbnail(const QImage& thumb, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {   
        KTempFile thumbFile(QString::null, "KExiv2ExifThumbnail");
        thumbFile.setAutoDelete(true);
        thumb.save(thumbFile.name(), "JPEG");

        const std::string &fileName( (const char*)(QFile::encodeName(thumbFile.name())) );
        d->exifMetadata.setJpegThumbnail( fileName );
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif Thumbnail using Exiv2 ", e);
    }        
    
    return false;
}

KExiv2::ImageOrientation KExiv2::getImageOrientation() const
{
    if (d->exifMetadata.empty())
       return ORIENTATION_UNSPECIFIED;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it;
        long orientation;
        ImageOrientation imageOrient = ORIENTATION_NORMAL;

        // Because some camera set a wrong standard exif orientation tag, 
        // We need to check makernote tags in first!

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = exifData.findKey(minoltaKey1);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                qDebug("Minolta Makernote Orientation: %i", (int)orientation);
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }

            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = exifData.findKey(minoltaKey2);

            if (it != exifData.end())
            {
                orientation = it->toLong();
                qDebug("Minolta Makernote Orientation: %i", (int)orientation);
                switch(orientation)
                {
                    case 76:
                        imageOrient = ORIENTATION_ROT_90;
                        break;
                    case 82:
                        imageOrient = ORIENTATION_ROT_270;
                        break;
                }
                return imageOrient;
            }
        }

        // -- Standard Exif tag --------------------------------

        Exiv2::ExifKey keyStd("Exif.Image.Orientation");
        it = exifData.findKey(keyStd);

        if (it != exifData.end())
        {
            orientation = it->toLong();
            qDebug("Exif Orientation: %i", (int)orientation);
            return (ImageOrientation)orientation;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot parse Exif Orientation tag using Exiv2 ", e);
    }

    return ORIENTATION_UNSPECIFIED;
}

bool KExiv2::setImageOrientation(ImageOrientation orientation, bool setProgramName)
{
    if (d->exifMetadata.empty())
       return false;

    if (!setProgramId(setProgramName))
        return false;

    // Workaround for older Exiv2 versions which do not support
    // Minolta Makernotes and throw an error for such keys.
    bool supportMinolta = true;
    try
    {
        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
    }
    catch( Exiv2::Error &e )
    {
        supportMinolta = false;
    }

    try
    {    
        if (orientation < ORIENTATION_UNSPECIFIED || orientation > ORIENTATION_ROT_270)
        {
            qDebug("Exif orientation tag value is not correct!");
            return false;
        }
        
        d->exifMetadata["Exif.Image.Orientation"] = static_cast<uint16_t>(orientation);
        qDebug("Exif orientation tag set to: %i", (int)orientation);

        // -- Minolta Cameras ----------------------------------

        if (supportMinolta)
        {
            // Minolta camera store image rotation in Makernote.
            // We remove these information to prevent duplicate values. 
    
            Exiv2::ExifData::iterator it;

            Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey1);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                qDebug("Removing Exif.MinoltaCs7D.Rotation tag");
            }
        
            Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
            it = d->exifMetadata.findKey(minoltaKey2);
            if (it != d->exifMetadata.end())
            {
                d->exifMetadata.erase(it);
                qDebug("Removing Exif.MinoltaCs5D.Rotation tag");
            }
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif Orientation tag using Exiv2 ", e);
    }        
    
    return false;
}

KExiv2::ImageColorWorkSpace KExiv2::getImageColorWorkSpace() const
{
    if (d->exifMetadata.empty())
        return WORKSPACE_UNSPECIFIED;

    try
    {    
        // Try to get Exif.Image tags
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifKey key("Exif.Photo.ColorSpace");
        Exiv2::ExifData::iterator it = exifData.findKey(key);
       
        if (it != exifData.end())
        {
            switch (it->toLong())
            {
                case 1:
                    return WORKSPACE_SRGB;
                    break;
                case 2:
                    return WORKSPACE_ADOBERGB;
                    break;
                case 65535:
                    return WORKSPACE_UNCALIBRATED;
                    break;
                default:
                    return WORKSPACE_UNSPECIFIED;
                    break;
            }
        }
        
        // TODO : add here Makernote parsing if necessary.
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot parse image color workspace tag using Exiv2 ", e);
    }        
    
    return WORKSPACE_UNSPECIFIED;    
}

bool KExiv2::setImageColorWorkSpace(ImageColorWorkSpace workspace, bool setProgramName)
{
    if (d->exifMetadata.empty())
       return false;

    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        d->exifMetadata["Exif.Photo.ColorSpace"] = static_cast<uint16_t>(workspace);
        qDebug("Exif color workspace tag set to: %i",  (int)workspace);
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif color workspace tag using Exiv2 ", e);
    }        
    
    return false;
}

QDateTime KExiv2::getImageDateTime() const
{
    try
    {    
        // In first, trying to get Date & time from Exif tags.
        
        if (!d->exifMetadata.empty())
        {        
            // Try Exif date time original.
    
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifKey key2("Exif.Photo.DateTimeOriginal");
            Exiv2::ExifData::iterator it2 = exifData.findKey(key2);
            
            if (it2 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it2->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // qDebug("DateTime (Exif original): %s", dateTime.toString().toAscii().constData());
                    return dateTime;
                }
            }
    
            // Bogus Exif date time original entry. Try Exif date time digitized.
    
            Exiv2::ExifKey key3("Exif.Photo.DateTimeDigitized");
            Exiv2::ExifData::iterator it3 = exifData.findKey(key3);
            
            if (it3 != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it3->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // qDebug("DateTime (Exif digitalized): %s", dateTime.toString().toAscii().constData());
                    return dateTime;
                }
            }

            // Bogus Exif date time digitized. Try standard Exif date time entry.
    
            Exiv2::ExifKey key("Exif.Image.DateTime");
            Exiv2::ExifData::iterator it = exifData.findKey(key);
            
            if (it != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
    
                if (dateTime.isValid())
                {
                    // qDebug("DateTime (Exif standard): %s", dateTime.toString().toAscii().constData());
                    return dateTime;
                }
            }
    
        }
        
        // In second, trying to get Date & time from Iptc tags.
            
        if (!d->iptcMetadata.empty())
        {        
            // Try creation Iptc date time entries.

            Exiv2::IptcKey keyDateCreated("Iptc.Application2.DateCreated");
            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcData::iterator it = iptcData.findKey(keyDateCreated);
                        
            if (it != iptcData.end())
            {
                QString IptcDateCreated(it->toString().c_str());
    
                Exiv2::IptcKey keyTimeCreated("Iptc.Application2.TimeCreated");
                Exiv2::IptcData::iterator it2 = iptcData.findKey(keyTimeCreated);
                
                if (it2 != iptcData.end())
                {
                    QString IptcTimeCreated(it2->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateCreated, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeCreated, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        // qDebug("Date (IPTC created): %s", dateTime.toString().toAscii().constData());
                        return dateTime;
                    }                    
                }
            }                        
            
            // Try digitization Iptc date time entries.
    
            Exiv2::IptcKey keyDigitizationDate("Iptc.Application2.DigitizationDate");
            Exiv2::IptcData::iterator it3 = iptcData.findKey(keyDigitizationDate);
                        
            if (it3 != iptcData.end())
            {
                QString IptcDateDigitization(it3->toString().c_str());
    
                Exiv2::IptcKey keyDigitizationTime("Iptc.Application2.DigitizationTime");
                Exiv2::IptcData::iterator it4 = iptcData.findKey(keyDigitizationTime);
                
                if (it4 != iptcData.end())
                {
                    QString IptcTimeDigitization(it4->toString().c_str());
                    
                    QDate date = QDate::fromString(IptcDateDigitization, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeDigitization, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);
                    
                    if (dateTime.isValid())
                    {
                        //qDebug("Date (IPTC digitalized): %s", dateTime.toString().toAscii().constData());
                        return dateTime;
                    }                    
                }
            }                       
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot parse Exif date & time tag using Exiv2 ", e);
    }        
    
    return QDateTime();
}

bool KExiv2::setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized, bool setProgramName)
{
    if(!dateTime.isValid())
        return false;
 
    if (!setProgramId(setProgramName))
        return false;
   
    try
    {    
        // In first we write date & time into Exif.
        
        // DateTimeDigitized is set by slide scanners etc. when a picture is digitized.
        // DateTimeOriginal specifies the date/time when the picture was taken.
        // For digital cameras, these dates should be both set, and identical.
        // Reference: http://www.exif.org/Exif2-2.PDF, chapter 4.6.5, table 4, section F.

        const std::string &exifdatetime(dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii().constData());
        d->exifMetadata["Exif.Image.DateTime"]         = exifdatetime;
        d->exifMetadata["Exif.Photo.DateTimeOriginal"] = exifdatetime;
        if(setDateTimeDigitized)
            d->exifMetadata["Exif.Photo.DateTimeDigitized"] = exifdatetime;
        
        // In Second we write date & time into Iptc.

        const std::string &iptcdate(dateTime.date().toString(Qt::ISODate).toAscii().constData());
        const std::string &iptctime(dateTime.time().toString(Qt::ISODate).toAscii().constData());
        d->iptcMetadata["Iptc.Application2.DateCreated"] = iptcdate;
        d->iptcMetadata["Iptc.Application2.TimeCreated"] = iptctime;
        if(setDateTimeDigitized)
        {
            d->iptcMetadata["Iptc.Application2.DigitizationDate"] = iptcdate;
            d->iptcMetadata["Iptc.Application2.DigitizationTime"] = iptctime;
        }
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Date & Time into image using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::getImagePreview(QImage& preview) const
{
    try
    {
        // In first we trying to get from Iptc preview tag.
        if (preview.loadFromData(getIptcTagData("Iptc.Application2.Preview")) )
            return true;

        // TODO : Added here Makernotes preview extraction when Exiv2 will be fixed for that.
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get image preview using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setImagePreview(const QImage& preview, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        KTempFile previewFile(QString::null, "KExiv2ImagePreview");
        previewFile.setAutoDelete(true);
        // A little bit compressed preview jpeg image to limit IPTC size.
        preview.save(previewFile.name(), "JPEG");

        QFile file(previewFile.name());
        if ( !file.open(QIODevice::ReadOnly) ) 
            return false;

        qDebug("(%i x %i) JPEG image preview size: %i bytes", 
               preview.width(), preview.height(), (int)file.size());
        
        QByteArray data(file.size());
        QDataStream stream( &file );
        stream.readRawBytes(data.data(), data.size());
        file.close();
        
        Exiv2::DataValue val;
        val.read((Exiv2::byte *)data.data(), data.size());
        d->iptcMetadata["Iptc.Application2.Preview"] = val;
        
        // See http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf Appendix A for details.
        d->iptcMetadata["Iptc.Application2.PreviewFormat"]  = 11;  // JPEG 
        d->iptcMetadata["Iptc.Application2.PreviewVersion"] = 1;
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get image preview using Exiv2 ", e);
    }

    return false;
}

QString KExiv2::getExifTagString(const char* exifTagName, bool escapeCR) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
                                 .arg(exifTagName), e);
    }

    return QString();
}

bool KExiv2::setExifTagString(const char *exifTagName, const QString& value, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata[exifTagName] = value.toAscii().constData();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif tag string into image using Exiv2 ", e);
    }

    return false;
}

QString KExiv2::getIptcTagString(const char* iptcTagName, bool escapeCR) const
{
    try
    {
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.findKey(iptcKey);
        if (it != iptcData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Iptc key '%1' into image using Exiv2 ")
                                 .arg(iptcTagName), e);
    }

    return QString();
}

bool KExiv2::setIptcTagString(const char *iptcTagName, const QString& value, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->iptcMetadata[iptcTagName] = value.toAscii().constData();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Iptc tag string into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::getExifTagLong(const char* exifTagName, long &val) const
{
    try
    {    
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            val = it->toLong();
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
                                 .arg(exifTagName), e);
    }        
    
    return false;    
}

QByteArray KExiv2::getExifTagData(const char* exifTagName) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            QByteArray data((*it).size());
            if (data.size())
                (*it).copy((Exiv2::byte*)data.data(), exifData.byteOrder());
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
                                 .arg(exifTagName), e);
    }

    return QByteArray();
}

QByteArray KExiv2::getIptcTagData(const char *iptcTagName) const
{
    try
    {
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.findKey(iptcKey);
        if (it != iptcData.end())
        {
            QByteArray data((*it).size());
            if (data.size())
                (*it).copy((Exiv2::byte*)data.data(), Exiv2::bigEndian);
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Iptc key '%1' into image using Exiv2 ")
                                 .arg(iptcTagName), e);
    }

    return QByteArray();
}

bool KExiv2::getExifTagRational(const char *exifTagName, long int &num, long int &den, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            num = (*it).toRational(component).first;
            den = (*it).toRational(component).second;
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Exif Rational value from key '%1' " 
                                         "into image using Exiv2 ").arg(exifTagName), e);
    }

    return false;
}

bool KExiv2::setExifTagLong(const char *exifTagName, long val, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata[exifTagName] = static_cast<int32_t>(val);
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif tag long value into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setExifTagRational(const char *exifTagName, long int num, long int den, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata[exifTagName] = Exiv2::Rational(num, den);
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif tag rational value into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setExifTagData(const char *exifTagName, const QByteArray& data, bool setProgramName)
{
    if (data.isEmpty())
	return false;

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        Exiv2::DataValue val((Exiv2::byte *)data.data(), data.size());
        d->exifMetadata[exifTagName] = val;
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif tag data into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setIptcTagData(const char *iptcTagName, const QByteArray& data, bool setProgramName)
{
    if (data.isEmpty())
	return false;
	
    if (!setProgramId(setProgramName))
        return false;

    try
    {	
        Exiv2::DataValue val((Exiv2::byte *)data.data(), data.size());
        d->iptcMetadata[iptcTagName] = val;
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Iptc tag data into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::removeExifTag(const char *exifTagName, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {  
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData::iterator it = d->exifMetadata.findKey(exifKey);
        if (it != d->exifMetadata.end())
        {
            d->exifMetadata.erase(it);
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot remove Exif tag using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::removeIptcTag(const char *iptcTagName, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {  
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData::iterator it = d->iptcMetadata.findKey(iptcKey);
        if (it != d->iptcMetadata.end())
        {
            d->iptcMetadata.erase(it);
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot remove Iptc tag using Exiv2 ", e);
    }        
    
    return false;
}

QString KExiv2::getExifTagTitle(const char *exifTagName)
{
    try 
    {
        std::string exifkey(exifTagName);
        Exiv2::ExifKey ek(exifkey); 
        return QString::fromLocal8Bit( Exiv2::ExifTags::tagTitle(ek.tag(), ek.ifdId()) );
    }
    catch (Exiv2::Error& e) 
    {
        printExiv2ExceptionError("Cannot get metadata tag title using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2::getExifTagDescription(const char *exifTagName)
{
    try 
    {
        std::string exifkey(exifTagName);
        Exiv2::ExifKey ek(exifkey); 
        return QString::fromLocal8Bit( Exiv2::ExifTags::tagDesc(ek.tag(), ek.ifdId()) );
    }
    catch (Exiv2::Error& e) 
    {
        printExiv2ExceptionError("Cannot get metadata tag description using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2::getIptcTagTitle(const char *iptcTagName)
{
    try 
    {
        std::string iptckey(iptcTagName);
        Exiv2::IptcKey ik(iptckey); 
        return QString::fromLocal8Bit( Exiv2::IptcDataSets::dataSetTitle(ik.tag(), ik.record()) );
    }
    catch (Exiv2::Error& e) 
    {
        printExiv2ExceptionError("Cannot get metadata tag title using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2::getIptcTagDescription(const char *iptcTagName)
{
    try 
    {
        std::string iptckey(iptcTagName);
        Exiv2::IptcKey ik(iptckey); 
        return QString::fromLocal8Bit( Exiv2::IptcDataSets::dataSetDesc(ik.tag(), ik.record()) );
    }
    catch (Exiv2::Error& e) 
    {
        printExiv2ExceptionError("Cannot get metadata tag description using Exiv2 ", e);
    }

    return QString();
}

KExiv2::MetaDataMap KExiv2::getExifTagsDataList(const QStringList exifKeysFilter, bool invertSelection)
{
    if (d->exifMetadata.empty())
       return MetaDataMap();

    try
    {
        Exiv2::ExifData exifData = d->exifMetadata;
        exifData.sortByKey();
        
        QString     ifDItemName;
        MetaDataMap metaDataMap;

        for (Exiv2::ExifData::iterator md = exifData.begin(); md != exifData.end(); ++md)
        {
            QString key = QString::fromAscii(md->key().c_str());

            // Decode the tag value with a user friendly output.
            QString tagValue;
            if (key == "Exif.Photo.UserComment")
            {
                tagValue = convertCommentValue(*md);
            }
            else
            {
                std::ostringstream os;
                os << *md;

                // Exif tag contents can be an i18n strings, no only simple ascii.
                tagValue = QString::fromLocal8Bit(os.str().c_str());
            }
            tagValue.replace("\n", " ");

            // We apply a filter to get only the Exif tags that we need.

            if (!invertSelection)
            {
                if (exifKeysFilter.contains(key.section(".", 1, 1)))
                    metaDataMap.insert(key, tagValue);
            }
            else
            {
                if (!exifKeysFilter.contains(key.section(".", 1, 1)))
                    metaDataMap.insert(key, tagValue);
            }
        }

        return metaDataMap;
    }
    catch (Exiv2::Error& e)
    {
        printExiv2ExceptionError("Cannot parse EXIF metadata using Exiv2 ", e);
    }

    return MetaDataMap();
}

KExiv2::MetaDataMap KExiv2::getIptcTagsDataList(const QStringList iptcKeysFilter, bool invertSelection)
{
    if (d->iptcMetadata.empty())
       return MetaDataMap();

    try
    {
        Exiv2::IptcData iptcData = d->iptcMetadata;
        iptcData.sortByKey();
        
        QString     ifDItemName;
        MetaDataMap metaDataMap;

        for (Exiv2::IptcData::iterator md = iptcData.begin(); md != iptcData.end(); ++md)
        {
            QString key = QString::fromAscii(md->key().c_str());
            
            // Decode the tag value with a user friendly output.
            std::ostringstream os;
            os << *md;
            QString value = QString::fromAscii(os.str().c_str());
            // To make a string just on one line.
            value.replace("\n", " ");

            // Some IPTC key are redondancy. check if already one exist...
            MetaDataMap::iterator it = metaDataMap.find(key);

            // We apply a filter to get only the Exif tags that we need.

            if (!invertSelection)
            {
                if (iptcKeysFilter.contains(key.section(".", 1, 1)))
                {
                    if (it == metaDataMap.end())
                        metaDataMap.insert(key, value);
                    else
                    {
                        QString v = *it;
                        v.append(", ");
                        v.append(value);
                        metaDataMap.replace(key, v);
                    }                
                }
            }
            else
            {
                if (!iptcKeysFilter.contains(key.section(".", 1, 1)))
                {
                    if (it == metaDataMap.end())
                        metaDataMap.insert(key, value);
                    else
                    {
                        QString v = *it;
                        v.append(", ");
                        v.append(value);
                        metaDataMap.replace(key, v);
                    }                
                }
            }
        }

        return metaDataMap;
    }
    catch (Exiv2::Error& e)
    {
        printExiv2ExceptionError("Cannot parse IPTC metadata using Exiv2 ", e);
    }

    return MetaDataMap();
}

bool KExiv2::getGPSInfo(double& altitude, double& latitude, double& longitude) const
{
    try
    {    
        double num, den, min, sec;
        latitude=0.0, longitude=0.0, altitude=0.0;
        
        // Get the reference in first.

        QByteArray latRef = getExifTagData("Exif.GPSInfo.GPSLatitudeRef");
        if (latRef.isEmpty()) return false;

        QByteArray lngRef = getExifTagData("Exif.GPSInfo.GPSLongitudeRef");
        if (lngRef.isEmpty()) return false;

        QByteArray altRef = getExifTagData("Exif.GPSInfo.GPSAltitudeRef");

        // Latitude decoding.

        Exiv2::ExifKey exifKey("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            latitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                latitude = latitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                latitude = latitude + sec/3600.0;
        }
        else 
            return false;
        
        if (latRef[0] == 'S') latitude *= -1.0;
    
        // Longitude decoding.

        Exiv2::ExifKey exifKey2("Exif.GPSInfo.GPSLongitude");
        it = exifData.findKey(exifKey2);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            longitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                longitude = longitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                longitude = longitude + sec/3600.0;
        }
        else 
            return false;
        
        if (lngRef[0] == 'W') longitude *= -1.0;

        // Altitude decoding.

        if (!altRef.isEmpty()) 
        {
            Exiv2::ExifKey exifKey3("Exif.GPSInfo.GPSAltitude");
            it = exifData.findKey(exifKey3);
            if (it != exifData.end())
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                altitude = num/den;
            }
        
            if (altRef[0] == '1') altitude *= -1.0;
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get Exif GPS tag using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::setGPSInfo(double altitude, double latitude, double longitude, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        // In first, we need to clean up all existing GPS info.
        removeGPSInfo();

        char scratchBuf[100];
        long int nom, denom;
        long int deg, min;
        
        // Do all the easy constant ones first.
        // GPSVersionID tag: standard says is should be four bytes: 02 00 00 00
        // (and, must be present).
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("2 0 0 0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSVersionID"), value.get());

        // Datum: the datum of the measured data. If not given, we insert WGS-84.
        d->exifMetadata["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
        
        // Now start adding data.

        // ALTITUDE.
        // Altitude reference: byte "00" meaning "sea level".
        value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"), value.get());
        
        // And the actual altitude.
        convertToRational(altitude, &nom, &denom, 4);
        snprintf(scratchBuf, 100, "%ld/%ld", nom, denom);
        d->exifMetadata["Exif.GPSInfo.GPSAltitude"] = scratchBuf;

        // LATTITUDE
        // Latitude reference: "N" or "S".
        if (latitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Southern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "S";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Northern hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "N";
        }
        
        // Now the actual lattitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LatRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x1000000 to get to mmmm/1000000.
        deg   = (int)floor(fabs(latitude)); // Slice off after decimal.
        min   = (int)floor((fabs(latitude) - floor(fabs(latitude))) * 60000000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/1000000 0/1", deg, min);
        d->exifMetadata["Exif.GPSInfo.GPSLatitude"] = scratchBuf;
        
        // LONGITUDE
        // Longitude reference: "E" or "W".
        if (longitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Western hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "W";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Eastern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "E";
        }

        // Now the actual longitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LongRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x1000000 to get to mmmm/1000000.
        deg   = (int)floor(fabs(longitude)); // Slice off after decimal.
        min   = (int)floor((fabs(longitude) - floor(fabs(longitude))) * 60000000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/1000000 0/1", deg, min);
        d->exifMetadata["Exif.GPSInfo.GPSLongitude"] = scratchBuf; 
    
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif GPS tag using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::removeGPSInfo(bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {  
        QStringList gpsTagsKeys;
  
        for (Exiv2::ExifData::iterator it = d->exifMetadata.begin();
             it != d->exifMetadata.end(); ++it)
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key.section(".", 1, 1) == QString("GPSInfo"))
                gpsTagsKeys.append(key);
        }

        for(QStringList::Iterator it2 = gpsTagsKeys.begin(); it2 != gpsTagsKeys.end(); ++it2)             
        {
            Exiv2::ExifKey gpsKey((*it2).toAscii().constData());
            Exiv2::ExifData::iterator it3 = d->exifMetadata.findKey(gpsKey);
            if (it3 != d->exifMetadata.end())
                d->exifMetadata.erase(it3);
        }
        
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot remove Exif GPS tag using Exiv2 ", e);
    }        
    
    return false;
}

void KExiv2::convertToRational(double number, long int* numerator, 
                                  long int* denominator, int rounding)
{
    // This function converts the given decimal number
    // to a rational (fractional) number.
    //
    // Examples in comments use Number as 25.12345, Rounding as 4.
    
    // Split up the number.
    double whole      = trunc(number);
    double fractional = number - whole;

    // Calculate the "number" used for rounding.
    // This is 10^Digits - ie, 4 places gives us 10000.
    double rounder = pow(10, rounding);

    // Round the fractional part, and leave the number
    // as greater than 1.
    // To do this we: (for example)
    //  0.12345 * 10000 = 1234.5
    //  floor(1234.5) = 1234 - now bigger than 1 - ready...
    fractional = trunc(fractional * rounder);

    // Convert the whole thing to a fraction.
    // Fraction is:
    //     (25 * 10000) + 1234   251234
    //     ------------------- = ------ = 25.1234
    //           10000            10000
    double numTemp = (whole * rounder) + fractional;
    double denTemp = rounder;

    // Now we should reduce until we can reduce no more.
    
    // Try simple reduction...
    // if   Num
    //     ----- = integer out then....
    //      Den
    if (trunc(numTemp / denTemp) == (numTemp / denTemp))
    {
        // Divide both by Denominator.
        numTemp /= denTemp;
        denTemp /= denTemp;
    }
    
    // And, if that fails, brute force it.
    while (1)
    {
        // Jump out if we can't integer divide one.
        if ((numTemp / 2) != trunc(numTemp / 2)) break;
        if ((denTemp / 2) != trunc(denTemp / 2)) break;
        // Otherwise, divide away.
        numTemp /= 2;
        denTemp /= 2;
    }

    // Copy out the numbers.
    *numerator   = (int)numTemp;
    *denominator = (int)denTemp;
}

QStringList KExiv2::getImageKeywords() const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {
            QStringList keywords;          
            Exiv2::IptcData iptcData(d->iptcMetadata);

            for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it)
            {
                QString key = QString::fromLocal8Bit(it->key().c_str());
                
                if (key == QString("Iptc.Application2.Keywords"))
                {
                    QString val(it->toString().c_str());
                    keywords.append(val);
                }
            }
            
            return keywords;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get IPTC Keywords from image using Exiv2 ", e);
    }        
    
    return QStringList();
}

bool KExiv2::setImageKeywords(const QStringList& oldKeywords, const QStringList& newKeywords, 
                              bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        QStringList oldkeys = oldKeywords;
        QStringList newkeys = newKeywords;
        
        qDebug("%s ==> Keywords: %s", d->filePath.toAscii().constData(), newkeys.join(",").toAscii().constData());
        
        // Remove all old keywords.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());

            // Also remove new keywords to avoid duplicates. They will be added again below.
            if ( key == QString("Iptc.Application2.Keywords") &&
                 (oldKeywords.contains(val) || newKeywords.contains(val))
               )
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new keywords. Note that Keywords IPTC tag is limited to 64 char but can be redondant.

        Exiv2::IptcKey iptcTag("Iptc.Application2.Keywords");

        for (QStringList::iterator it = newkeys.begin(); it != newkeys.end(); ++it)
        {
            QString key = *it;
            key.truncate(64);
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::asciiString);
            val->read(key.latin1());
            iptcData.add(iptcTag, val.get());        
        }

        d->iptcMetadata = iptcData;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set IPTC Keywords into image using Exiv2 ", e);
    }        
    
    return false;
}

QStringList KExiv2::getImageSubjects() const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {
            QStringList subjects;          
            Exiv2::IptcData iptcData(d->iptcMetadata);

            for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it)
            {
                QString key = QString::fromLocal8Bit(it->key().c_str());
                
                if (key == QString("Iptc.Application2.Subject"))
                {
                    QString val(it->toString().c_str());
                    subjects.append(val);
                }
            }
            
            return subjects;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get IPTC Subjects from image using Exiv2 ", e);
    }        
    
    return QStringList();
}

bool KExiv2::setImageSubjects(const QStringList& oldSubjects, const QStringList& newSubjects, 
                              bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        QStringList oldDef = oldSubjects;
        QStringList newDef = newSubjects;
        
        // Remove all old subjects.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());
            
            if (key == QString("Iptc.Application2.Subject") && oldDef.contains(val))
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new subjects. Note that Keywords IPTC tag is limited to 236 char but can be redondant.

        Exiv2::IptcKey iptcTag("Iptc.Application2.Subject");

        for (QStringList::iterator it = newDef.begin(); it != newDef.end(); ++it)
        {
            QString key = *it;
            key.truncate(236);
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::asciiString);
            val->read(key.latin1());
            iptcData.add(iptcTag, val.get());        
        }

        d->iptcMetadata = iptcData;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set IPTC Subjects into image using Exiv2 ", e);
    }        
    
    return false;
}

QStringList KExiv2::getImageSubCategories() const
{
    try
    {    
        if (!d->iptcMetadata.empty())
        {
            QStringList subCategories;          
            Exiv2::IptcData iptcData(d->iptcMetadata);

            for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it)
            {
                QString key = QString::fromLocal8Bit(it->key().c_str());
                
                if (key == QString("Iptc.Application2.SuppCategory"))
                {
                    QString val(it->toString().c_str());
                    subCategories.append(val);
                }
            }
            
            return subCategories;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get IPTC Sub Categories from image using Exiv2 ", e);
    }        
    
    return QStringList();
}

bool KExiv2::setImageSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories, 
                                   bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        QStringList oldkeys = oldSubCategories;
        QStringList newkeys = newSubCategories;
        
        // Remove all old Sub Categories.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());
            
            if (key == QString("Iptc.Application2.SuppCategory") && oldSubCategories.contains(val))
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new Sub Categories. Note that SubCategories IPTC tag is limited to 32 
        // characters but can be redondant.

        Exiv2::IptcKey iptcTag("Iptc.Application2.SuppCategory");

        for (QStringList::iterator it = newkeys.begin(); it != newkeys.end(); ++it)
        {
            QString key = *it;
            key.truncate(32);
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::asciiString);
            val->read(key.latin1());
            iptcData.add(iptcTag, val.get());        
        }

        d->iptcMetadata = iptcData;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set IPTC Sub Categories into image using Exiv2 ", e);
    }        
    
    return false;
}

QString KExiv2::getExifComment() const
{
    try
    {
        if (!d->exifMetadata.empty())
        {
            Exiv2::ExifKey key("Exif.Photo.UserComment");
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifData::iterator it = exifData.findKey(key);

            if (it != exifData.end())
            {
                QString exifComment = convertCommentValue(*it);

                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !exifComment.stripWhiteSpace().isEmpty())
                    return exifComment;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot find Exif User Comment using Exiv2 ", e);
    }

    return QString();
}

bool KExiv2::setExifComment(const QString& comment, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        if (comment.isEmpty())
            return false;

    // Write as Unicode only when necessary.
    QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
    if (latin1Codec->canEncode(comment))
    {
        // write as ASCII
        std::string exifComment("charset=\"Ascii\" ");
        exifComment += comment.latin1();
        d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
    }
    else
    {
        // write as Unicode (UCS-2)

        // Be aware that we are dealing with a UCS-2 string.
        // Null termination means \0\0, strlen does not work,
        // do not use any const-char*-only methods,
        // pass a std::string and not a const char * to ExifDatum::operator=().
        const unsigned short *ucs2 = comment.ucs2();
        std::string exifComment("charset=\"Unicode\" ");
        exifComment.append((const char*)ucs2, sizeof(unsigned short) * comment.length());
        d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
    }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif Comment using Exiv2 ", e);
    }

    return false;
}

QString KExiv2::convertCommentValue(const Exiv2::Exifdatum &exifDatum)
{
    try
    {
        std::string comment;
        std::string charset;
#ifdef EXIV2_CHECK_VERSION
        if (EXIV2_CHECK_VERSION(0,11,0))
        {
            comment = exifDatum.toString();
        }
        else
        {
            // workaround for bug in TIFF parser: CommentValue is loaded as DataValue
            const Exiv2::Value &value = exifDatum.value();
            Exiv2::byte *data = new Exiv2::byte[value.size()];
            value.copy(data, Exiv2::invalidByteOrder);
            Exiv2::CommentValue commentValue;
            // this read method is hidden in CommentValue
            static_cast<Exiv2::Value &>(commentValue).read(data, value.size(), Exiv2::invalidByteOrder);
            comment = commentValue.toString();
            delete [] data;
        }
#else
        comment = exifDatum.toString();
#endif

        // libexiv2 will prepend "charset=\"SomeCharset\" " if charset is specified
        // Before conversion to QString, we must know the charset, so we stay with std::string for a while
        if (comment.length() > 8 && comment.substr(0, 8) == "charset=")
        {
            // the prepended charset specification is followed by a blank
            std::string::size_type pos = comment.find_first_of(' ');
            if (pos != std::string::npos)
            {
                // extract string between the = and the blank
                charset = comment.substr(8, pos-8);
                // get the rest of the string after the charset specification
                comment = comment.substr(pos+1);
            }
        }

        if (charset == "\"Unicode\"")
        {
            // QString expects a null-terminated UCS-2 string.
            // Is it already null terminated? In any case, add termination "\0\0" for safety.
            comment.resize(comment.length() + 2, '\0');
            return QString::fromUcs2((unsigned short *)comment.data());
        }
        else if (charset == "\"Jis\"")
        {
            QTextCodec *codec = QTextCodec::codecForName("JIS7");
            return codec->toUnicode(comment.c_str());
        }
        else if (charset == "\"Ascii\"")
        {
            return QString::fromLatin1(comment.c_str());
        }
        else
        {
            return detectEncodingAndDecode(comment);
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot convert Comment using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2::detectEncodingAndDecode(const std::string &value)
{
    // For charset autodetection, we could use sophisticated code
    // (Mozilla chardet, KHTML's autodetection, QTextCodec::codecForContent),
    // but that is probably too much.
    // We check for UTF8, Local encoding and ASCII.

    if (value.empty())
        return QString();

#if KDE_IS_VERSION(3,2,0)
    if (KStringHandler::isUtf8(value.c_str()))
    {
        return QString::fromUtf8(value.c_str());
    }
#else
    // anyone who is still running KDE 3.0 or 3.1 is missing so many features
    // that he will have to accept this missing feature.
    return QString::fromUtf8(value.c_str());
#endif

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // We try if QTextCodec can decide here, otherwise we use Latin1.
    // Or use local8Bit as default?

    // load QTextCodecs
    QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
    //QTextCodec *utf8Codec   = QTextCodec::codecForName("utf8");
    QTextCodec *localCodec  = QTextCodec::codecForLocale();

    // make heuristic match
    int latin1Score = latin1Codec->heuristicContentMatch(value.c_str(), value.length());
    int localScore  = localCodec->heuristicContentMatch(value.c_str(), value.length());

    // convert string:
    // Use whatever has the larger score, local or ASCII
    if (localScore >= 0 && localScore >= latin1Score)
    {
        // workaround for bug #134999:
        // The QLatin15Codec may crash if strlen < value.length()
        int length = value.length();
        if (localCodec->name() == QString::fromLatin1("ISO 8859-15"))
            length = strlen(value.c_str());
        return localCodec->toUnicode(value.c_str(), length);
    }
    else
        return QString::fromLatin1(value.c_str());
}

bool KExiv2::setProgramId(bool /*on*/)
{
    return true; 
}

}  // NameSpace KExiv2Iface
