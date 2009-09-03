/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Exif manipulation methods
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

bool KExiv2::canWriteExif(const QString& filePath)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdExif);
        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error &e )
    {
        std::string s(e.what());
        kDebug(51003) << "Cannot check Exif access mode using Exiv2 (Error #" 
                      << e.code() << ": " << s.c_str() << ")" << endl;
    }

    return false;
}

bool KExiv2::hasExif() const
{
    return !d->exifMetadata.empty();
}

bool KExiv2::clearExif() const
{
    try
    {
        d->exifMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot clear Exif data using Exiv2 ", e);
    }

    return false;
}

QByteArray KExiv2::getExif(bool addExifHeader) const
{
    try
    {
        if (!d->exifMetadata.empty())
        {
            QByteArray data;
            Exiv2::ExifData& exif = d->exifMetadata;
#if (EXIV2_TEST_VERSION(0,17,91))
            Exiv2::Blob blob;
            Exiv2::ExifParser::encode(blob, Exiv2::bigEndian, exif);
            QByteArray ba((const char*)&blob[0], blob.size());
#else
            Exiv2::DataBuf c2 = exif.copy();
            QByteArray ba((const char*)c2.pData_, c2.size_);
#endif
            if (addExifHeader)
            {
                const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
                data.resize(ba.size() + sizeof(ExifHeader));
                memcpy(data.data(), ExifHeader, sizeof(ExifHeader));
                memcpy(data.data()+sizeof(ExifHeader), ba.data(), ba.size());
            }
            else
            {
                data = ba;
            }
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            kDebug(51003) << "From file " << d->filePath.toAscii().constData() << endl;

        d->printExiv2ExceptionError("Cannot get Exif data using Exiv2 ", e);
    }

    return QByteArray();
}

bool KExiv2::setExif(const QByteArray& data) const
{
    try
    {
        if (!data.isEmpty())
        {
#if (EXIV2_TEST_VERSION(0,17,91))
            Exiv2::ExifParser::decode(d->exifMetadata, (const Exiv2::byte*)data.data(), data.size());
            return (!d->exifMetadata.empty());
#else
            if (d->exifMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
                return false;
            else
                return true;
#endif
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            kDebug(51003) << "From file " << d->filePath.toAscii().constData() << endl;

        d->printExiv2ExceptionError("Cannot set Exif data using Exiv2 ", e);
    }

    return false;
}

KExiv2::MetaDataMap KExiv2::getExifTagsDataList(const QStringList &exifKeysFilter, bool invertSelection) const
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
                tagValue = d->convertCommentValue(*md);
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
        d->printExiv2ExceptionError("Cannot parse EXIF metadata using Exiv2 ", e);
    }

    return MetaDataMap();
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
                QString exifComment = d->convertCommentValue(*it);

                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !exifComment.trimmed().isEmpty())
                    return exifComment;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot find Exif User Comment using Exiv2 ", e);
    }

    return QString();
}

bool KExiv2::setExifComment(const QString& comment, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        removeExifTag("Exif.Photo.UserComment");

        if (!comment.isNull())
        {
            // Write as Unicode only when necessary.
            QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
            if (latin1Codec->canEncode(comment))
            {
                // write as ASCII
                std::string exifComment("charset=\"Ascii\" ");
                exifComment += comment.toLatin1().constData();
                d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
            }
            else
            {
                // write as Unicode (UCS-2)

                // Be aware that we are dealing with a UCS-2 string.
                // Null termination means \0\0, strlen does not work,
                // do not use any const-char*-only methods,
                // pass a std::string and not a const char * to ExifDatum::operator=().
                const unsigned short *ucs2 = comment.utf16();
                std::string exifComment("charset=\"Unicode\" ");
                exifComment.append((const char*)ucs2, sizeof(unsigned short) * comment.length());
                d->exifMetadata["Exif.Photo.UserComment"] = exifComment;
            }
        }
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Exif Comment using Exiv2 ", e);
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
        d->printExiv2ExceptionError("Cannot get metadata tag title using Exiv2 ", e);
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
        d->printExiv2ExceptionError("Cannot get metadata tag description using Exiv2 ", e);
    }

    return QString();
}

bool KExiv2::removeExifTag(const char *exifTagName, bool setProgramName) const
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
        d->printExiv2ExceptionError("Cannot remove Exif tag using Exiv2 ", e);
    }

    return false;
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
        d->printExiv2ExceptionError(QString("Cannot find Exif Rational value from key '%1' " 
                                         "into image using Exiv2 ").arg(exifTagName), e);
    }

    return false;
}

bool KExiv2::setExifTagLong(const char *exifTagName, long val, bool setProgramName) const
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
        d->printExiv2ExceptionError("Cannot set Exif tag long value into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setExifTagRational(const char *exifTagName, long int num, long int den, bool setProgramName) const
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
        d->printExiv2ExceptionError("Cannot set Exif tag rational value into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setExifTagData(const char *exifTagName, const QByteArray& data, bool setProgramName) const
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
        d->printExiv2ExceptionError("Cannot set Exif tag data into image using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setExifTagVariant(const char *exifTagName, const QVariant& val, 
                               bool rationalWantSmallDenominator, bool setProgramName) const
{
    switch (val.type())
    {
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::Bool:
        case QVariant::LongLong:
        case QVariant::ULongLong:
            return setExifTagLong(exifTagName, val.toInt(), setProgramName);

        case QVariant::Double:
        {
            long num, den;
            if (rationalWantSmallDenominator)
                convertToRationalSmallDenominator(val.toDouble(), &num, &den);
            else
                convertToRational(val.toDouble(), &num, &den, 4);
            return setExifTagRational(exifTagName, num, den, setProgramName);
        }
        case QVariant::List:
        {
            long num = 0, den = 1;
            QList<QVariant> list = val.toList();
            if (list.size() >= 1)
                num = list[0].toInt();
            if (list.size() >= 2)
                den = list[1].toInt();
            return setExifTagRational(exifTagName, num, den, setProgramName);
        }

        case QVariant::Date:
        case QVariant::DateTime:
        {
            QDateTime dateTime = val.toDateTime();
            if(!dateTime.isValid())
                return false;

            if (!setProgramId(setProgramName))
                return false;

            try
            {
                const std::string &exifdatetime(dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii().constData());
                d->exifMetadata[exifTagName] = exifdatetime;
            }
            catch( Exiv2::Error &e )
            {
                d->printExiv2ExceptionError("Cannot set Date & Time in image using Exiv2 ", e);
            }
            return false;
        }

        case QVariant::String:
        case QVariant::Char:
            return setExifTagString(exifTagName, val.toString(), setProgramName);

        case QVariant::ByteArray:
            return setExifTagData(exifTagName, val.toByteArray(), setProgramName);
        default:
            break;
    }
    return false;
}

QString KExiv2::createExifUserStringFromValue(const char *exifTagName, const QVariant &val, bool escapeCR)
{
    try
    {
        Exiv2::ExifKey key(exifTagName);
        Exiv2::Exifdatum datum(key);
        switch (val.type())
        {
            case QVariant::Int:
            case QVariant::Bool:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                datum = (int32_t)val.toInt();
                break;
            case QVariant::UInt:
                datum = (uint32_t)val.toUInt();
                break;

            case QVariant::Double:
            {
                long num, den;
                convertToRationalSmallDenominator(val.toDouble(), &num, &den);
                Exiv2::Rational rational;
                rational.first  = num;
                rational.second = den;
                datum = rational;
                break;
            }
            case QVariant::List:
            {
                long num = 0, den = 1;
                QList<QVariant> list = val.toList();
                if (list.size() >= 1)
                    num = list[0].toInt();
                if (list.size() >= 2)
                    den = list[1].toInt();
                Exiv2::Rational rational;
                rational.first  = num;
                rational.second = den;
                datum = rational;
                break;
            }

            case QVariant::Date:
            case QVariant::DateTime:
            {
                QDateTime dateTime = val.toDateTime();
                if(!dateTime.isValid())
                    break;

                const std::string &exifdatetime(dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii().constData());
                datum = exifdatetime;
                break;
            }

            case QVariant::String:
            case QVariant::Char:
                datum = (std::string)val.toString().toAscii().constData();
                break;
            default:
                break;
        }

        std::ostringstream os;
        os << datum;
        QString tagValue = QString::fromLocal8Bit(os.str().c_str());

        if (escapeCR)
            tagValue.replace("\n", " ");

        return tagValue;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Iptc tag string into image using Exiv2 ", e);
    }

    return QString();
}

bool KExiv2::getExifTagLong(const char* exifTagName, long &val) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end() && it->count() > 0)
        {
            val = it->toLong();
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
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
            char *s = new char[(*it).size()];
#if (EXIV2_TEST_VERSION(0,17,91))
            (*it).copy((Exiv2::byte*)s, Exiv2::bigEndian);
#else
            (*it).copy((Exiv2::byte*)s, exifData.byteOrder());
#endif
            QByteArray data(s, (*it).size());
            delete s;
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
                                 .arg(exifTagName), e);
    }

    return QByteArray();
}

QVariant KExiv2::getExifTagVariant(const char *exifTagName, bool rationalAsListOfInts, bool stringEscapeCR, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            switch (it->typeId())
            {
                case Exiv2::unsignedByte:
                case Exiv2::unsignedShort:
                case Exiv2::unsignedLong:
                case Exiv2::signedShort:
                case Exiv2::signedLong:
                    if (it->count() > component)
                        return QVariant((int)it->toLong(component));
                    else
                        return QVariant(QVariant::Int);
                case Exiv2::unsignedRational:
                case Exiv2::signedRational:
                    if (rationalAsListOfInts)
                    {
                        if (it->count() <= component)
                            return QVariant(QVariant::List);
                        QList<QVariant> list;
                        list << (*it).toRational(component).first;
                        list << (*it).toRational(component).second;
                        return QVariant(list);
                    }
                    else
                    {
                        if (it->count() <= component)
                            return QVariant(QVariant::Double);
                        // prefer double precision
                        double num = (*it).toRational(component).first;
                        double den = (*it).toRational(component).second;
                        if (den == 0.0)
                            return QVariant(QVariant::Double);
                        return QVariant(num / den);
                    }
                case Exiv2::date:
                case Exiv2::time:
                {
                    QDateTime dateTime = QDateTime::fromString(it->toString().c_str(), Qt::ISODate);
                    return QVariant(dateTime);
                }
                case Exiv2::asciiString:
                case Exiv2::comment:
                case Exiv2::string:
                {
                    std::ostringstream os;
                    os << *it;
                    QString tagValue = QString::fromLocal8Bit(os.str().c_str());

                    if (stringEscapeCR)
                        tagValue.replace("\n", " ");

                    return QVariant(tagValue);
                }
                default:
                    break;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Exif key '%1' in the image using Exiv2 ")
                                 .arg(exifTagName), e);
    }

    return QVariant();
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
#if (EXIV2_TEST_VERSION(0,17,91))
            // See B.K.O #184156 comment #13
            std::string val  = it->print(&exifData);
            QString tagValue = QString::fromLocal8Bit(val.c_str());
#else
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());
#endif
            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Exif key '%1' into image using Exiv2 ")
                                 .arg(exifTagName), e);
    }

    return QString();
}

bool KExiv2::setExifTagString(const char *exifTagName, const QString& value, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata[exifTagName] = std::string(value.toAscii().constData());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Exif tag string into image using Exiv2 ", e);
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
#if (EXIV2_TEST_VERSION(0,17,91))
        Exiv2::ExifThumbC thumb(d->exifMetadata);
        Exiv2::DataBuf const c1 = thumb.copy();
#else
        Exiv2::DataBuf const c1(d->exifMetadata.copyThumbnail());
#endif
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
                    long orientation = it->toLong();
                    kDebug(51003) << "Exif Thumbnail Orientation: " << (int)orientation << endl;
                    rotateExifQImage(thumbnail, (ImageOrientation)orientation);
                }

                return thumbnail;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot get Exif Thumbnail using Exiv2 ", e);
    }

    return thumbnail;
}

bool KExiv2::rotateExifQImage(QImage &image, ImageOrientation orientation) const
{
    QMatrix matrix;

    switch(orientation)
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

    if (orientation != ORIENTATION_NORMAL)
    {
        image = image.transformed(matrix);
        return true;
    }

    return false;
}

bool KExiv2::setExifThumbnail(const QImage& thumb, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
#if (EXIV2_TEST_VERSION(0,17,91))
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        thumb.save(&buffer, "JPEG");
        Exiv2::ExifThumb thumb(d->exifMetadata);
        thumb.setJpegThumbnail((Exiv2::byte *)data.data(), data.size());
#else
        KTemporaryFile thumbFile;
        thumbFile.setSuffix("KExiv2ExifThumbnail");
        thumbFile.setAutoRemove(true);
        thumbFile.open();
        thumb.save(thumbFile.fileName(), "JPEG");
        kDebug(51003) << "Thumbnail temp file: " << thumbFile.fileName().toAscii().data() << endl;
        const std::string &fileName((const char*)(QFile::encodeName(thumbFile.fileName())));
        d->exifMetadata.setJpegThumbnail( fileName );
#endif
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Exif Thumbnail using Exiv2 ", e);
    }

    return false;
}

}  // NameSpace KExiv2Iface
