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

// Local includes.

#include "kexiv2private.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

//-- Common metadata image information manipulation methods ----------------

bool KExiv2::setImageProgramId(const QString& program, const QString& version)
{
    try
    {
        // Record program info in Exif.Image.ProcessingSoftware tag (only available with Exiv2 >= 0.14.0).

#if (EXIV2_TEST_VERSION(0,14,0))
        QString software(program);
        software.append("-");
        software.append(version);
        d->exifMetadata["Exif.Image.ProcessingSoftware"] = software.toAscii().constData();
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
    if (!d->exifMetadata.empty())
    {
        long colorSpace;

        if (getExifTagLong("Exif.Photo.ColorSpace", colorSpace))
        {
            switch (colorSpace)
            {
                case 1:
                {
                    return WORKSPACE_SRGB;
                    break;
                }
                case 2:
                {
                    return WORKSPACE_ADOBERGB;
                    break;
                }
                case 65535:
                {
                    // Nikon camera set Exif.Photo.ColorSpace to uncalibrated and 
                    // Exif.Nikon3.ColorMode to "MODE2" when users work in AdobRGB color space.
                    if (getExifTagString("Exif.Nikon3.ColorMode").contains("MODE2"))
                        return WORKSPACE_ADOBERGB;

                    // TODO : add more Makernote parsing here ...

                    return WORKSPACE_UNCALIBRATED;
                    break;
                }
                default:
                {
                    return WORKSPACE_UNSPECIFIED;
                    break;
                }
            }
        }
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
        KTemporaryFile previewFile;
        previewFile.setSuffix("KExiv2ImagePreview");
        previewFile.setAutoRemove(true);
        // A little bit compressed preview jpeg image to limit IPTC size.
        preview.save(previewFile.fileName(), "JPEG");

        QFile file(previewFile.fileName());
        if ( !file.open(QIODevice::ReadOnly) ) 
            return false;

        qDebug("(%i x %i) JPEG image preview size: %i bytes", 
               preview.width(), preview.height(), (int)file.size());
        
        char *s;
        uint  l;
        QDataStream stream( &file );
        stream.readBytes(s, l);
        QByteArray data(s, l);
        delete [] s;
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
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::string);
            val->read(key.toLatin1().constData());
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
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::string);
            val->read(key.toLatin1().constData());
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
            
            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::string);
            val->read(key.toLatin1().constData());
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

}  // NameSpace KExiv2Iface
