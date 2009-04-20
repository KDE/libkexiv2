/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Common metadata image information manipulation methods
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

// Local includes.

#include "kexiv2_p.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

bool KExiv2::setImageProgramId(const QString& program, const QString& version) const
{
    try
    {
        QString software(program);
        software.append("-");
        software.append(version);

        // Set program info into Exif.Image.ProcessingSoftware tag (only available with Exiv2 >= 0.14.0).

#if (EXIV2_TEST_VERSION(0,14,0))
        d->exifMetadata["Exif.Image.ProcessingSoftware"] = std::string(software.toAscii().constData());
#endif

        // See B.K.O #142564: Check if Exif.Image.Software already exist. If yes, do not touch this tag.

        if (!d->exifMetadata.empty())
        {
            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifKey key("Exif.Image.Software");
            Exiv2::ExifData::iterator it = exifData.findKey(key);
            if (it == exifData.end())
                d->exifMetadata["Exif.Image.Software"] = std::string(software.toAscii().constData());
        }

        // set program info into XMP tags.

#ifdef _XMP_SUPPORT_

        if (!d->xmpMetadata.empty())
        {
            // Only create Xmp.xmp.CreatorTool if it do not exist.
            Exiv2::XmpData xmpData(d->xmpMetadata);
            Exiv2::XmpKey key("Xmp.xmp.CreatorTool");
            Exiv2::XmpData::iterator it = xmpData.findKey(key);
            if (it == xmpData.end())
                setXmpTagString("Xmp.xmp.CreatorTool", software, false);
        }

        setXmpTagString("Xmp.tiff.Software", software, false);

#endif // _XMP_SUPPORT_

        // Set program info into IPTC tags.

        d->iptcMetadata["Iptc.Application2.Program"]        = std::string(program.toAscii().constData());
        d->iptcMetadata["Iptc.Application2.ProgramVersion"] = std::string(version.toAscii().constData());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Program identity into image using Exiv2 ", e);
    }

    return false;
}

QSize KExiv2::getImageDimensions() const
{
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

#ifdef _XMP_SUPPORT_

        // Try to get Xmp.tiff tags

        width    = -1;
        height   = -1;
        bool wOk = false;
        bool hOk = false;

        QString str = getXmpTagString("Xmp.tiff.ImageWidth");
        if (!str.isEmpty())
            width = str.toInt(&wOk);

        str = getXmpTagString("Xmp.tiff.ImageLength");
        if (!str.isEmpty())
            height = str.toInt(&hOk);

        if (wOk && hOk)
            return QSize(width, height);

        // Try to get Xmp.exif tags

        width  = -1;
        height = -1;
        wOk    = false;
        hOk    = false;

        str = getXmpTagString("Xmp.exif.PixelXDimension");
        if (!str.isEmpty())
            width = str.toInt(&wOk);

        str = getXmpTagString("Xmp.exif.PixelYDimension");
        if (!str.isEmpty())
            height = str.toInt(&hOk);

        if (wOk && hOk)
            return QSize(width, height);

#endif // _XMP_SUPPORT_

    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot parse image dimensions tag using Exiv2 ", e);
    }

    return QSize();
}

bool KExiv2::setImageDimensions(const QSize& size, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // Set Exif values.

        // NOTE: see B.K.O #144604: need to cast to record an unsigned integer value.
        d->exifMetadata["Exif.Image.ImageWidth"]      = static_cast<uint32_t>(size.width());
        d->exifMetadata["Exif.Image.ImageLength"]     = static_cast<uint32_t>(size.height());
        d->exifMetadata["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(size.width());
        d->exifMetadata["Exif.Photo.PixelYDimension"] = static_cast<uint32_t>(size.height());

        // Set Xmp values.

#ifdef _XMP_SUPPORT_

        setXmpTagString("Xmp.tiff.ImageWidth",      QString::number(size.width()),  false);
        setXmpTagString("Xmp.tiff.ImageLength",     QString::number(size.height()), false);
        setXmpTagString("Xmp.exif.PixelXDimension", QString::number(size.width()),  false);
        setXmpTagString("Xmp.exif.PixelYDimension", QString::number(size.height()), false);

#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set image dimensions using Exiv2 ", e);
    }

    return false;
}

KExiv2::ImageOrientation KExiv2::getImageOrientation() const
{
    try
    {
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it;
        long orientation;
        ImageOrientation imageOrient = ORIENTATION_NORMAL;

        // Because some camera set a wrong standard exif orientation tag, 
        // We need to check makernote tags in first!

        // -- Minolta Cameras ----------------------------------

        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        it = exifData.findKey(minoltaKey1);

        if (it != exifData.end())
        {
            orientation = it->toLong();
            kDebug(51003) << "Orientation => Exif.MinoltaCs7D.Rotation => " << (int)orientation << endl;
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
            kDebug(51003) << "Orientation => Exif.MinoltaCs5D.Rotation => " << (int)orientation << endl;
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

        // -- Standard Exif tag --------------------------------

        Exiv2::ExifKey keyStd("Exif.Image.Orientation");
        it = exifData.findKey(keyStd);

        if (it != exifData.end())
        {
            orientation = it->toLong();
            kDebug(51003) << "Orientation => Exif.Image.Orientation => " << (int)orientation << endl;
            return (ImageOrientation)orientation;
        }

        // -- Standard Xmp tag --------------------------------

#ifdef _XMP_SUPPORT_

        bool ok = false;
        QString str = getXmpTagString("Xmp.tiff.Orientation");
        if (!str.isEmpty())
        {
            orientation = str.toLong(&ok);
            if (ok)
            {
                kDebug(51003) << "Orientation => Xmp.tiff.Orientation => " << (int)orientation << endl;
                return (ImageOrientation)orientation;
            }
        }

#endif // _XMP_SUPPORT_

    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot parse Exif Orientation tag using Exiv2 ", e);
    }

    return ORIENTATION_UNSPECIFIED;
}

bool KExiv2::setImageOrientation(ImageOrientation orientation, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        if (orientation < ORIENTATION_UNSPECIFIED || orientation > ORIENTATION_ROT_270)
        {
            kDebug(51003) << "Image orientation value is not correct!" << endl;
            return false;
        }

        // Set Exif values.

        d->exifMetadata["Exif.Image.Orientation"] = static_cast<uint16_t>(orientation);
        kDebug(51003) << "Exif.Image.Orientation tag set to: " << (int)orientation << endl;

        // Set Xmp values.

#ifdef _XMP_SUPPORT_

        setXmpTagString("Xmp.tiff.Orientation", QString::number((int)orientation), false);

#endif // _XMP_SUPPORT_

        // -- Minolta Cameras ----------------------------------

        // Minolta camera store image rotation in Makernote.
        // We remove these information to prevent duplicate values. 

        Exiv2::ExifData::iterator it;

        Exiv2::ExifKey minoltaKey1("Exif.MinoltaCs7D.Rotation");
        it = d->exifMetadata.findKey(minoltaKey1);
        if (it != d->exifMetadata.end())
        {
            d->exifMetadata.erase(it);
            kDebug(51003) << "Removing Exif.MinoltaCs7D.Rotation tag" << endl;
        }

        Exiv2::ExifKey minoltaKey2("Exif.MinoltaCs5D.Rotation");
        it = d->exifMetadata.findKey(minoltaKey2);
        if (it != d->exifMetadata.end())
        {
            d->exifMetadata.erase(it);
            kDebug(51003) << "Removing Exif.MinoltaCs5D.Rotation tag" << endl;
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Exif Orientation tag using Exiv2 ", e);
    }

    return false;
}

KExiv2::ImageColorWorkSpace KExiv2::getImageColorWorkSpace() const
{
    // Check Exif values.

    long colorSpace = 0;

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
        }
    }

    // Check Xmp values.

#ifdef _XMP_SUPPORT_

    colorSpace  = 0;
    bool ok     = false;
    QString str = getXmpTagString("Xmp.exif.ColorSpace");
    if (!str.isEmpty())
    {
        colorSpace = str.toLong(&ok);
        if (ok)
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
                    return WORKSPACE_UNCALIBRATED;
                    break;
                }
            }
        }
    }

#endif // _XMP_SUPPORT_

    return WORKSPACE_UNSPECIFIED;
}

bool KExiv2::setImageColorWorkSpace(ImageColorWorkSpace workspace, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // Set Exif value.

        d->exifMetadata["Exif.Photo.ColorSpace"] = static_cast<uint16_t>(workspace);

        // Set Xmp value.

#ifdef _XMP_SUPPORT_

        setXmpTagString("Xmp.exif.ColorSpace", QString::number((int)workspace), false);

#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Exif color workspace tag using Exiv2 ", e);
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
            Exiv2::ExifData exifData(d->exifMetadata);
            {
                Exiv2::ExifKey key("Exif.Photo.DateTimeOriginal");
                Exiv2::ExifData::iterator it = exifData.findKey(key);
                if (it != exifData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Exif.Photo.DateTimeOriginal => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::ExifKey key("Exif.Photo.DateTimeDigitized");
                Exiv2::ExifData::iterator it = exifData.findKey(key);
                if (it != exifData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Exif.Photo.DateTimeDigitized => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::ExifKey key("Exif.Image.DateTime");
                Exiv2::ExifData::iterator it = exifData.findKey(key);
                if (it != exifData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Exif.Image.DateTime => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
        }

        // In second, trying to get Date & time from Xmp tags.

#ifdef _XMP_SUPPORT_

        if (!d->xmpMetadata.empty())
        {
            Exiv2::XmpData xmpData(d->xmpMetadata);
            {
                Exiv2::XmpKey key("Xmp.exif.DateTimeOriginal");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.exif.DateTimeOriginal => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.exif.DateTimeDigitized");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.exif.DateTimeDigitized => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.photoshop.DateCreated");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.photoshop.DateCreated => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.xmp.CreateDate");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.xmp.CreateDate => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.tiff.DateTime");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.tiff.DateTime => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.xmp.ModifyDate");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.xmp.ModifyDate => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
            {
                Exiv2::XmpKey key("Xmp.xmp.MetadataDate");
                Exiv2::XmpData::iterator it = xmpData.findKey(key);
                if (it != xmpData.end())
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "DateTime => Xmp.xmp.MetadataDate => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
        }

#endif // _XMP_SUPPORT_

        // In third, trying to get Date & time from Iptc tags.

        if (!d->iptcMetadata.empty())
        {
            Exiv2::IptcData iptcData(d->iptcMetadata);

            // Try creation Iptc date & time entries.

            Exiv2::IptcKey keyDateCreated("Iptc.Application2.DateCreated");
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
                        kDebug(51003) << "DateTime => Iptc.Application2.DateCreated => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }

            // Try digitization Iptc date & time entries.

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
                        kDebug(51003) << "DateTime => Iptc.Application2.DigitizationDate => " << dateTime << endl;
                        return dateTime;
                    }
                }
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot parse Exif date & time tag using Exiv2 ", e);
    }

    return QDateTime();
}

bool KExiv2::setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized, bool setProgramName) const
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

#ifdef _XMP_SUPPORT_

        // In second we write date & time into Xmp.

        const std::string &xmpdatetime(dateTime.toString(Qt::ISODate).toAscii().constData());

        Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::xmpText);
        xmpTxtVal->read(xmpdatetime);
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.exif.DateTimeOriginal"), xmpTxtVal.get());
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.photoshop.DateCreated"), xmpTxtVal.get());
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.tiff.DateTime"),         xmpTxtVal.get());
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.xmp.CreateDate"),        xmpTxtVal.get());
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.xmp.MetadataDate"),      xmpTxtVal.get());
        d->xmpMetadata.add(Exiv2::XmpKey("Xmp.xmp.ModifyDate"),        xmpTxtVal.get());
        if(setDateTimeDigitized)
            d->xmpMetadata.add(Exiv2::XmpKey("Xmp.exif.DateTimeDigitized"), xmpTxtVal.get());

        // Tag not updated:
        // "Xmp.dc.DateTime" is a sequence of date relevant of dublin core change. 
        //                   This is not the picture date as well

#endif // _XMP_SUPPORT_

        // In third we write date & time into Iptc.

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
        d->printExiv2ExceptionError("Cannot set Date & Time into image using Exiv2 ", e);
    }

    return false;
}

QDateTime KExiv2::getDigitizationDateTime(bool fallbackToCreationTime) const
{
    try
    {
        // In first, trying to get Date & time from Exif tags.

        if (!d->exifMetadata.empty())
        {
            // Try Exif date time digitized.

            Exiv2::ExifData exifData(d->exifMetadata);
            Exiv2::ExifKey key("Exif.Photo.DateTimeDigitized");
            Exiv2::ExifData::iterator it = exifData.findKey(key);

            if (it != exifData.end())
            {
                QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);

                if (dateTime.isValid())
                {
                    kDebug(51003) << "DateTime (Exif digitalized): " << dateTime.toString().toAscii().constData() << endl;
                    return dateTime;
                }
            }

        }

        // In second, trying to get Date & time from Iptc tags.

        if (!d->iptcMetadata.empty())
        {

            // Try digitization Iptc date time entries.

            Exiv2::IptcData iptcData(d->iptcMetadata);
            Exiv2::IptcKey keyDigitizationDate("Iptc.Application2.DigitizationDate");
            Exiv2::IptcData::iterator it = iptcData.findKey(keyDigitizationDate);

            if (it != iptcData.end())
            {
                QString IptcDateDigitization(it->toString().c_str());

                Exiv2::IptcKey keyDigitizationTime("Iptc.Application2.DigitizationTime");
                Exiv2::IptcData::iterator it2 = iptcData.findKey(keyDigitizationTime);

                if (it2 != iptcData.end())
                {
                    QString IptcTimeDigitization(it2->toString().c_str());

                    QDate date = QDate::fromString(IptcDateDigitization, Qt::ISODate);
                    QTime time = QTime::fromString(IptcTimeDigitization, Qt::ISODate);
                    QDateTime dateTime = QDateTime(date, time);

                    if (dateTime.isValid())
                    {
                        kDebug(51003) << "Date (IPTC digitalized): " << dateTime.toString().toAscii().constData() << endl;
                        return dateTime;
                    }
                }
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot parse Exif digitization date & time tag using Exiv2 ", e);
    }

    if (fallbackToCreationTime)
        return getImageDateTime();
    else
        return QDateTime();
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
        d->printExiv2ExceptionError("Cannot get image preview using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setImagePreview(const QImage& preview, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);

        // A little bit compressed preview jpeg image to limit IPTC size.
        preview.save(&buffer, "JPEG");
        kDebug(51003) << "JPEG image preview size: (" << preview.width() << " x " 
                      << preview.height() << ") pixels - " << data.size() << " bytes" << endl;

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
        d->printExiv2ExceptionError("Cannot get image preview using Exiv2 ", e);
    }

    return false;
}

}  // NameSpace KExiv2Iface
