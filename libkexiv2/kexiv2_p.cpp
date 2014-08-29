/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2007-09-03
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2006-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2012 by Marcel Wiesweg
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

#include "kexiv2_p.h"

// C ANSI includes

extern "C"
{
#include <sys/stat.h>
#include <utime.h>
}

namespace KExiv2Iface
{

KExiv2::Private::Private()
    : data(new KExiv2Data::Private)
{
    writeRawFiles         = false;
    updateFileTimeStamp   = false;
    useXMPSidecar4Reading = false;
    metadataWritingMode   = WRITETOIMAGEONLY;
    loadedFromSidecar     = false;
    Exiv2::LogMsg::setHandler(KExiv2::Private::printExiv2MessageHandler);
}

KExiv2::Private::~Private()
{
}

void KExiv2::Private::copyPrivateData(const Private* const other)
{
    data                  = other->data;
    filePath              = other->filePath;
    writeRawFiles         = other->writeRawFiles;
    updateFileTimeStamp   = other->updateFileTimeStamp;
    useXMPSidecar4Reading = other->useXMPSidecar4Reading;
    metadataWritingMode   = other->metadataWritingMode;
}

bool KExiv2::Private::saveToXMPSidecar(const QFileInfo& finfo) const
{
    QString filePath = KExiv2::sidecarFilePathForFile(finfo.filePath());

    if (filePath.isEmpty())
    {
        return false;
    }

    try
    {
        Exiv2::Image::AutoPtr image;
        image = Exiv2::ImageFactory::create(Exiv2::ImageType::xmp, (const char*)(QFile::encodeName(filePath)));
        return saveOperations(finfo, image);
    }
    catch( Exiv2::Error& e )
    {
        printExiv2ExceptionError("Cannot save metadata to XMP sidecar using Exiv2 ", e);
        return false;
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
        return false;
    }
}

bool KExiv2::Private::saveToFile(const QFileInfo& finfo) const
{
    if (!finfo.isWritable())
    {
        kDebug() << "File '" << finfo.fileName().toAscii().constData() << "' is read only. Metadata not written.";
        return false;
    }

    QStringList rawTiffBasedSupported, rawTiffBasedNotSupported;

    // Raw files supported by Exiv2 0.21
    rawTiffBasedSupported << "dng" << "nef" << "pef" << "orf" << "srw";

    if (Exiv2::testVersion(0,23,0))
    {
        rawTiffBasedSupported << "cr2";
    }

    // Raw files not supported by Exiv2 0.21
    rawTiffBasedNotSupported
        << "3fr" << "arw" << "dcr" << "erf" << "k25" << "kdc" 
        << "mos" << "raw" << "sr2" << "srf" << "rw2";

    if (!Exiv2::testVersion(0,23,0))
    {
        rawTiffBasedNotSupported << "cr2";
    }

    QString ext = finfo.suffix().toLower();

    if (!writeRawFiles && (rawTiffBasedSupported.contains(ext) || rawTiffBasedNotSupported.contains(ext)) )
    {
        kDebug() << finfo.fileName()
                 << "is a TIFF based RAW file, writing to such a file is disabled by current settings.";
        return false;
    }

/*
    if (rawTiffBasedNotSupported.contains(ext))
    {
        kDebug() << finfo.fileName()
                 << "is TIFF based RAW file not yet supported. Metadata not saved.";
        return false;
    }

    if (rawTiffBasedSupported.contains(ext) && !writeRawFiles)
    {
        kDebug() << finfo.fileName()
                 << "is TIFF based RAW file supported but writing mode is disabled. "
                 << "Metadata not saved.";
        return false;
    }

    kDebug() << "File Extension: " << ext << " is supported for writing mode";

    bool ret = false;
*/

    try
    {
        Exiv2::Image::AutoPtr image;
        image = Exiv2::ImageFactory::open((const char*)(QFile::encodeName(finfo.filePath())));
        return saveOperations(finfo, image);
    }
    catch( Exiv2::Error& e )
    {
        printExiv2ExceptionError("Cannot save metadata to image using Exiv2 ", e);
        return false;
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
        return false;
    }
}

bool KExiv2::Private::saveOperations(const QFileInfo& finfo, Exiv2::Image::AutoPtr image) const
{
    try
    {
        Exiv2::AccessMode mode;
        bool wroteComment = false, wroteEXIF = false, wroteIPTC = false, wroteXMP = false;
        
        // We need to load target file metadata to merge with new one. It's mandatory with TIFF format:
        // like all tiff file structure is based on Exif.
        image->readMetadata();

        // Image Comments ---------------------------------

        mode = image->checkMode(Exiv2::mdComment);

        if ((mode == Exiv2::amWrite) || (mode == Exiv2::amReadWrite))
        {
            image->setComment(imageComments());
            wroteComment = true;
        }

        kDebug() << "wroteComment: " << wroteComment;

        // Exif metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdExif);

        if ((mode == Exiv2::amWrite) || (mode == Exiv2::amReadWrite))
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

                Exiv2::ExifData readedExif = exifMetadata();

                for (Exiv2::ExifData::iterator it = readedExif.begin(); it != readedExif.end(); ++it)
                {
                    if (!untouchedTags.contains(it->key().c_str()))
                    {
                        newExif[it->key().c_str()] = readedExif[it->key().c_str()];
                    }
                }

                image->setExifData(newExif);
            }
            else
            {
                image->setExifData(exifMetadata());
            }

            wroteEXIF = true;
        }

        kDebug() << "wroteEXIF: " << wroteEXIF;

        // Iptc metadata ----------------------------------

        mode = image->checkMode(Exiv2::mdIptc);

        if ((mode == Exiv2::amWrite) || (mode == Exiv2::amReadWrite))
        {
            image->setIptcData(iptcMetadata());
            wroteIPTC = true;
        }

        kDebug() << "wroteIPTC: " << wroteIPTC;

        // Xmp metadata -----------------------------------

        mode = image->checkMode(Exiv2::mdXmp);

        if ((mode == Exiv2::amWrite) || (mode == Exiv2::amReadWrite))
        {
#ifdef _XMP_SUPPORT_
            image->setXmpData(xmpMetadata());
            wroteXMP = true;
#endif
        }

        kDebug() << "wroteXMP: " << wroteXMP;

        if (!wroteComment && !wroteEXIF && !wroteIPTC && !wroteXMP)
        {
            kDebug() << "Writing metadata is not supported for file" << finfo.fileName();
            return false;
        }
        else if (!wroteEXIF || !wroteIPTC || !wroteXMP)
        {
            kDebug() << "Support for writing metadata is limited for file" << finfo.fileName();
        }

        if (!updateFileTimeStamp)
        {
            // Don't touch access and modification timestamp of file.
            struct stat    st;
            struct utimbuf ut;
            int ret = ::stat(QFile::encodeName(filePath), &st);

            if (ret == 0)
            {
                ut.modtime = st.st_mtime;
                ut.actime  = st.st_atime;
            }

            image->writeMetadata();

            if (ret == 0)
            {
                ::utime(QFile::encodeName(filePath), &ut);
            }
            
            kDebug() << "File time stamp restored";
        }
        else
        {
            image->writeMetadata();
        }

        return true;
    }
    catch( Exiv2::Error& e )
    {
        printExiv2ExceptionError("Cannot save metadata using Exiv2 ", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

    return false;
}

void KExiv2Data::Private::clear()
{
    imageComments.clear();
    exifMetadata.clear();
    iptcMetadata.clear();
#ifdef _XMP_SUPPORT_
    xmpMetadata.clear();
#endif
}

void KExiv2::Private::printExiv2ExceptionError(const QString& msg, Exiv2::Error& e)
{
    std::string s(e.what());
    kError() << msg.toAscii().constData() << " (Error #"
             << e.code() << ": " << s.c_str();
}

void KExiv2::Private::printExiv2MessageHandler(int lvl, const char* msg)
{
    kDebug() << "Exiv2 (" << lvl << ") : " << msg;
}

QString KExiv2::Private::convertCommentValue(const Exiv2::Exifdatum& exifDatum) const
{
    try
    {
        std::string comment;
        std::string charset;

        comment = exifDatum.toString();

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
            return QString::fromUtf8(comment.data());
        }
        else if (charset == "\"Jis\"")
        {
            QTextCodec* codec = QTextCodec::codecForName("JIS7");
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
    catch( Exiv2::Error& e )
    {
        printExiv2ExceptionError("Cannot convert Comment using Exiv2 ", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }

    return QString();
}

QString KExiv2::Private::detectEncodingAndDecode(const std::string& value) const
{
    // For charset autodetection, we could use sophisticated code
    // (Mozilla chardet, KHTML's autodetection, QTextCodec::codecForContent),
    // but that is probably too much.
    // We check for UTF8, Local encoding and ASCII.
    // TODO: Gilles ==> Marcel : Look like KEncodingDetector class can provide a full implementation for encoding detection.

    if (value.empty())
    {
        return QString();
    }

    if (KStringHandler::isUtf8(value.c_str()))
    {
        return QString::fromUtf8(value.c_str());
    }

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // So we can use either local encoding, or latin1.

    //TODO: KDE4PORT: check for regression of #134999 (very probably no regression!)
    return QString::fromLocal8Bit(value.c_str());
    //return QString::fromLatin1(value.c_str());
}

int KExiv2::Private::getXMPTagsListFromPrefix(const QString& pf, KExiv2::TagsMap& tagsMap) const
{
    QList<const Exiv2::XmpPropertyInfo*> tags;
    tags << Exiv2::XmpProperties::propertyList(pf.toAscii().data());
    int i = 0;

    for (QList<const Exiv2::XmpPropertyInfo*>::iterator it = tags.begin(); it != tags.end(); ++it)
    {
        while ( (*it) && !QString((*it)->name_).isNull() )
        {
            QString     key = QLatin1String( Exiv2::XmpKey( pf.toAscii().data(), (*it)->name_ ).key().c_str() );
            QStringList values;
            values << (*it)->name_ << (*it)->title_ << (*it)->desc_;
            tagsMap.insert(key, values);
            ++(*it);
            i++;
        }
    }

    return i;
}

#ifdef _XMP_SUPPORT_
void KExiv2::Private::loadSidecarData(Exiv2::Image::AutoPtr xmpsidecar)
{
    // Having a sidecar is a special situation.
    // The sidecar data often "dominates", see in particular bug 309058 for important aspects:
    // If a field is removed from the sidecar, we must ignore (older) data for this field in the file.

    // First: Ignore file XMP, only use sidecar XMP
    xmpMetadata() = xmpsidecar->xmpData();
    loadedFromSidecar = true;

    // EXIF
    // Four groups of properties are mapped between EXIF and XMP:
    // Date/Time, Description, Copyright, Creator
    // A few more tags are defined "writeback" tags in the XMP specification, the sidecar value therefore overrides the Exif value.
    // The rest is kept side-by-side.
    // (to understand, remember that the xmpsidecar's Exif data is actually XMP data mapped back to Exif)

    // Description, Copyright and Creator is dominated by the sidecar: Remove file Exif fields, if field not in XMP.
    ExifMergeHelper exifDominatedHelper;
    exifDominatedHelper << QLatin1String("Exif.Image.ImageDescription")
                        << QLatin1String("Exif.Photo.UserComment")
                        << QLatin1String("Exif.Image.Copyright")
                        << QLatin1String("Exif.Image.Artist");
    exifDominatedHelper.exclusiveMerge(xmpsidecar->exifData(), exifMetadata());
    // Date/Time and "the few more" from the XMP spec are handled as writeback
    // Note that Date/Time mapping is slightly contradictory in latest specs.
    ExifMergeHelper exifWritebackHelper;
    exifWritebackHelper << QLatin1String("Exif.Image.DateTime")
                        << QLatin1String("Exif.Image.DateTime")
                        << QLatin1String("Exif.Photo.DateTimeOriginal")
                        << QLatin1String("Exif.Photo.DateTimeDigitized")
                        << QLatin1String("Exif.Image.Orientation")
                        << QLatin1String("Exif.Image.XResolution")
                        << QLatin1String("Exif.Image.YResolution")
                        << QLatin1String("Exif.Image.ResolutionUnit")
                        << QLatin1String("Exif.Image.Software")
                        << QLatin1String("Exif.Photo.RelatedSoundFile");
    exifWritebackHelper.mergeFields(xmpsidecar->exifData(), exifMetadata());

    // IPTC
    // These fields cover almost all relevant IPTC data and are defined in the XMP specification for reconciliation.
    IptcMergeHelper iptcDominatedHelper;
    iptcDominatedHelper << QLatin1String("Iptc.Application2.ObjectName")
                        << QLatin1String("Iptc.Application2.Urgency")
                        << QLatin1String("Iptc.Application2.Category")
                        << QLatin1String("Iptc.Application2.SuppCategory")
                        << QLatin1String("Iptc.Application2.Keywords")
                        << QLatin1String("Iptc.Application2.SubLocation")
                        << QLatin1String("Iptc.Application2.SpecialInstructions")
                        << QLatin1String("Iptc.Application2.Byline")
                        << QLatin1String("Iptc.Application2.BylineTitle")
                        << QLatin1String("Iptc.Application2.City")
                        << QLatin1String("Iptc.Application2.ProvinceState")
                        << QLatin1String("Iptc.Application2.CountryCode")
                        << QLatin1String("Iptc.Application2.CountryName")
                        << QLatin1String("Iptc.Application2.TransmissionReference")
                        << QLatin1String("Iptc.Application2.Headline")
                        << QLatin1String("Iptc.Application2.Credit")
                        << QLatin1String("Iptc.Application2.Source")
                        << QLatin1String("Iptc.Application2.Copyright")
                        << QLatin1String("Iptc.Application2.Caption")
                        << QLatin1String("Iptc.Application2.Writer");
    iptcDominatedHelper.exclusiveMerge(xmpsidecar->iptcData(), iptcMetadata());

    IptcMergeHelper iptcWritebackHelper;
    iptcWritebackHelper << QLatin1String("Iptc.Application2.DateCreated")
                        << QLatin1String("Iptc.Application2.TimeCreated")
                        << QLatin1String("Iptc.Application2.DigitizationDate")
                        << QLatin1String("Iptc.Application2.DigitizationTime");
    iptcWritebackHelper.mergeFields(xmpsidecar->iptcData(), iptcMetadata());

    /*
     * TODO: Exiv2 (referring to 0.23) does not correctly synchronize all times values as given below.
     * Time values and their synchronization:
     * Original Date/Time – Creation date of the intellectual content (e.g. the photograph),
       rather than the creatio*n date of the content being shown
        Exif DateTimeOriginal (36867, 0x9003) and SubSecTimeOriginal (37521, 0x9291)
        IPTC DateCreated (IIM 2:55, 0x0237) and TimeCreated (IIM 2:60, 0x023C)
        XMP (photoshop:DateCreated)
     * Digitized Date/Time – Creation date of the digital representation
        Exif DateTimeDigitized (36868, 0x9004) and SubSecTimeDigitized (37522, 0x9292)
        IPTC DigitalCreationDate (IIM 2:62, 0x023E) and DigitalCreationTime (IIM 2:63, 0x023F)
        XMP (xmp:CreateDate)
     * Modification Date/Time – Modification date of the digital image file
        Exif DateTime (306, 0x132) and SubSecTime (37520, 0x9290)
        XMP (xmp:ModifyDate)
     */
}
#endif // _XMP_SUPPORT_

}  // NameSpace KExiv2Iface
