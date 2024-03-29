/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kexiv2.h"
#include "kexiv2_p.h"

// C++ includes

#include <cctype>

// Qt includes

#include <QBuffer>

// Local includes

#include "rotationmatrix.h"
#include "libkexiv2_debug.h"

namespace KExiv2Iface
{

bool KExiv2::canWriteExif(const QString& filePath)
{
    try
    {
#if EXIV2_TEST_VERSION(0,28,0)
        Exiv2::Image::UniquePtr image =
#else
        Exiv2::Image::AutoPtr image =
#endif
                                      Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath).constData()));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdExif);

        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error& e )
    {
        std::string s(e.what());
        qCCritical(LIBKEXIV2_LOG) << "Cannot check Exif access mode using Exiv2 (Error #"
#if EXIV2_TEST_VERSION(0,28,0)
                    << Exiv2::Error(e.code()).what()
#else
                    << e.code() << ": " << s.c_str()
#endif
                    << ")";
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::hasExif() const
{
    return !d->exifMetadata().empty();
}

bool KExiv2::clearExif() const
{
    try
    {
        d->exifMetadata().clear();
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot clear Exif data using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

QByteArray KExiv2::getExifEncoded(bool addExifHeader) const
{
    try
    {
        if (!d->exifMetadata().empty())
        {
            QByteArray data;
            Exiv2::ExifData& exif = d->exifMetadata();
            Exiv2::Blob blob;
            Exiv2::ExifParser::encode(blob, Exiv2::bigEndian, exif);
            QByteArray ba((const char*)&blob[0], blob.size());

            if (addExifHeader)
            {
                const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
                data.resize(ba.size() + sizeof(ExifHeader));
                memcpy(data.data(), ExifHeader, sizeof(ExifHeader));
                memcpy(data.data() + sizeof(ExifHeader), ba.data(), ba.size());
            }
            else
            {
                data = ba;
            }
            return data;
        }
    }
    catch( Exiv2::Error& e )
    {
        if (!d->filePath.isEmpty())
            qCDebug(LIBKEXIV2_LOG) << "From file " << d->filePath.toLatin1().constData();

        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get Exif data using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QByteArray();
}

bool KExiv2::setExif(const QByteArray& data) const
{
    try
    {
        if (!data.isEmpty())
        {
            Exiv2::ExifParser::decode(d->exifMetadata(), (const Exiv2::byte*)data.data(), data.size());
            return (!d->exifMetadata().empty());
        }
    }
    catch( Exiv2::Error& e )
    {
        if (!d->filePath.isEmpty())
            qCCritical(LIBKEXIV2_LOG) << "From file " << d->filePath.toLatin1().constData();

        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif data using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

KExiv2::MetaDataMap KExiv2::getExifTagsDataList(const QStringList& exifKeysFilter, bool invertSelection) const
{
    if (d->exifMetadata().empty())
       return MetaDataMap();

    try
    {
        Exiv2::ExifData exifData = d->exifMetadata();
        exifData.sortByKey();

        QString     ifDItemName;
        MetaDataMap metaDataMap;

        for (Exiv2::ExifData::iterator md = exifData.begin(); md != exifData.end(); ++md)
        {
            QString key = QString::fromLatin1(md->key().c_str());

            // Decode the tag value with a user friendly output.
            QString tagValue;

            if (key == QString::fromLatin1("Exif.Photo.UserComment"))
            {
                tagValue = d->convertCommentValue(*md);
            }
            else if (key == QString::fromLatin1("Exif.Image.0x935c"))
            {
                tagValue = QString::number(md->value().size());
            }
            else
            {
                std::ostringstream os;
                os << *md;

                // Exif tag contents can be an translated strings, no only simple ascii.
                tagValue = QString::fromLocal8Bit(os.str().c_str());
            }

            tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));

            // We apply a filter to get only the Exif tags that we need.

            if (!exifKeysFilter.isEmpty())
            {
                if (!invertSelection)
                {
                    if (exifKeysFilter.contains(key.section(QString::fromLatin1("."), 1, 1)))
                        metaDataMap.insert(key, tagValue);
                }
                else
                {
                    if (!exifKeysFilter.contains(key.section(QString::fromLatin1("."), 1, 1)))
                        metaDataMap.insert(key, tagValue);
                }
            }
            else // else no filter at all.
            {
                metaDataMap.insert(key, tagValue);
            }
        }

        return metaDataMap;
    }
    catch (Exiv2::Error& e)
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot parse EXIF metadata using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return MetaDataMap();
}

QString KExiv2::getExifComment() const
{
    try
    {
        if (!d->exifMetadata().empty())
        {
            Exiv2::ExifData exifData(d->exifMetadata());
            Exiv2::ExifKey key("Exif.Photo.UserComment");
            Exiv2::ExifData::iterator it = exifData.findKey(key);

            if (it != exifData.end())
            {
                QString exifComment = d->convertCommentValue(*it);

                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !exifComment.trimmed().isEmpty())
                    return exifComment;
            }

            Exiv2::ExifKey key2("Exif.Image.ImageDescription");
            Exiv2::ExifData::iterator it2 = exifData.findKey(key2);

            if (it2 != exifData.end())
            {
                QString exifComment = d->convertCommentValue(*it2);

                // Some cameras fill in nonsense default values
                QStringList blackList;
                blackList << QString::fromLatin1("SONY DSC"); // + whitespace
                blackList << QString::fromLatin1("OLYMPUS DIGITAL CAMERA");
                blackList << QString::fromLatin1("MINOLTA DIGITAL CAMERA");

                QString trimmedComment = exifComment.trimmed();

                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !trimmedComment.isEmpty() && !blackList.contains(trimmedComment))
                    return exifComment;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif User Comment using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QString();
}

static bool is7BitAscii(const QString& s)
{
    return std::all_of(s.begin(), s.end(), [](QChar c) { return c.unicode() <= 0x7f; });
}

bool KExiv2::setExifComment(const QString& comment, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        removeExifTag("Exif.Image.ImageDescription");
        removeExifTag("Exif.Photo.UserComment");

        if (!comment.isNull())
        {
            setExifTagString("Exif.Image.ImageDescription", comment, setProgramName);

            // Write as Unicode only when necessary.
            if (is7BitAscii(comment))
            {
                // write as ASCII
                std::string exifComment("charset=\"Ascii\" ");
                exifComment += comment.toLatin1().constData();
                d->exifMetadata()["Exif.Photo.UserComment"] = exifComment;
                return true;
            }
            // write as Unicode (UCS-2)
            std::string exifComment("charset=\"Unicode\" ");
            exifComment += comment.toUtf8().constData();
            d->exifMetadata()["Exif.Photo.UserComment"] = exifComment;
        }

        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif Comment using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

QString KExiv2::getExifTagTitle(const char* exifTagName)
{
    try
    {
        std::string exifkey(exifTagName);
        Exiv2::ExifKey ek(exifkey);

        return QString::fromLocal8Bit( ek.tagLabel().c_str() );
    }
    catch (Exiv2::Error& e)
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get metadata tag title using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QString();
}

QString KExiv2::getExifTagDescription(const char* exifTagName)
{
    try
    {
        std::string exifkey(exifTagName);
        Exiv2::ExifKey ek(exifkey);

        return QString::fromLocal8Bit( ek.tagDesc().c_str() );
    }
    catch (Exiv2::Error& e)
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get metadata tag description using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QString();
}

bool KExiv2::removeExifTag(const char* exifTagName, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData::iterator it = d->exifMetadata().findKey(exifKey);

        if (it != d->exifMetadata().end())
        {
            d->exifMetadata().erase(it);
            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot remove Exif tag using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::getExifTagRational(const char* exifTagName, long int& num, long int& den, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata());
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);

        if (it != exifData.end())
        {
            num = (*it).toRational(component).first;
            den = (*it).toRational(component).second;

            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif Rational value from key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::setExifTagLong(const char* exifTagName, long val, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata()[exifTagName] = static_cast<int32_t>(val);
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif tag long value into image using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::setExifTagRational(const char* exifTagName, long int num, long int den, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata()[exifTagName] = Exiv2::Rational(num, den);
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif tag rational value into image using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::setExifTagData(const char* exifTagName, const QByteArray& data, bool setProgramName) const
{
    if (data.isEmpty())
        return false;

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        Exiv2::DataValue val((Exiv2::byte*)data.data(), data.size());
        d->exifMetadata()[exifTagName] = val;
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif tag data into image using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::setExifTagVariant(const char* exifTagName, const QVariant& val,
                               bool rationalWantSmallDenominator, bool setProgramName) const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    switch (val.metaType().id())
#else
    switch (val.type())
#endif
    {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Bool:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return setExifTagLong(exifTagName, val.toInt(), setProgramName);

        case QMetaType::Double:
        {
            long num, den;

            if (rationalWantSmallDenominator)
                convertToRationalSmallDenominator(val.toDouble(), &num, &den);
            else
                convertToRational(val.toDouble(), &num, &den, 4);

            return setExifTagRational(exifTagName, num, den, setProgramName);
        }
        case QMetaType::QVariantList:
        {
            long num = 0, den = 1;
            QList<QVariant> list = val.toList();

            if (list.size() >= 1)
                num = list[0].toInt();

            if (list.size() >= 2)
                den = list[1].toInt();

            return setExifTagRational(exifTagName, num, den, setProgramName);
        }

        case QMetaType::QDate:
        case QMetaType::QDateTime:
        {
            QDateTime dateTime = val.toDateTime();

            if(!dateTime.isValid())
                return false;

            if (!setProgramId(setProgramName))
                return false;

            try
            {
                const std::string &exifdatetime(dateTime.toString(QString::fromLatin1("yyyy:MM:dd hh:mm:ss")).toLatin1().constData());
                d->exifMetadata()[exifTagName] = exifdatetime;
            }
            catch( Exiv2::Error &e )
            {
                d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Date & Time in image using Exiv2 "), e);
            }
            catch(...)
            {
                qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
            }

            return false;
        }

        case QMetaType::QString:
        case QMetaType::QChar:
            return setExifTagString(exifTagName, val.toString(), setProgramName);

        case QMetaType::QByteArray:
            return setExifTagData(exifTagName, val.toByteArray(), setProgramName);
        default:
            break;
    }
    return false;
}

QString KExiv2::createExifUserStringFromValue(const char* exifTagName, const QVariant& val, bool escapeCR)
{
    try
    {
        Exiv2::ExifKey key(exifTagName);
        Exiv2::Exifdatum datum(key);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        switch (val.metaType().id())
#else
        switch (val.type())
#endif
        {
            case QMetaType::Int:
            case QMetaType::Bool:
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
                datum = (int32_t)val.toInt();
                break;
            case QMetaType::UInt:
                datum = (uint32_t)val.toUInt();
                break;

            case QMetaType::Double:
            {
                long num, den;
                convertToRationalSmallDenominator(val.toDouble(), &num, &den);
                Exiv2::Rational rational;
                rational.first  = num;
                rational.second = den;
                datum = rational;
                break;
            }
            case QMetaType::QVariantList:
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

            case QMetaType::QDate:
            case QMetaType::QDateTime:
            {
                QDateTime dateTime = val.toDateTime();
                if(!dateTime.isValid())
                    break;

                const std::string &exifdatetime(dateTime.toString(QString::fromLatin1("yyyy:MM:dd hh:mm:ss")).toLatin1().constData());
                datum = exifdatetime;
                break;
            }

            case QMetaType::QString:
            case QMetaType::QChar:
                datum = (std::string)val.toString().toLatin1().constData();
                break;
            default:
                break;
        }

        std::ostringstream os;
        os << datum;
        QString tagValue = QString::fromLocal8Bit(os.str().c_str());

        if (escapeCR)
            tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));

        return tagValue;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Iptc tag string into image using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QString();
}

bool KExiv2::getExifTagLong(const char* exifTagName, long& val) const
{
    return getExifTagLong(exifTagName, val, 0);
}

bool KExiv2::getExifTagLong(const char* exifTagName, long& val, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata());
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);

        if (it != exifData.end() && it->count() > 0)
        {
#if EXIV2_TEST_VERSION(0,28,0)
            val = it->toUint32(component);
#else
            val = it->toLong(component);
#endif
            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

QByteArray KExiv2::getExifTagData(const char* exifTagName) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata());
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);

        if (it != exifData.end())
        {
            char* const s = new char[(*it).size()];
            (*it).copy((Exiv2::byte*)s, Exiv2::bigEndian);
            QByteArray data(s, (*it).size());
            delete[] s;

            return data;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QByteArray();
}

QVariant KExiv2::getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts, bool stringEscapeCR, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata());
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
#if EXIV2_TEST_VERSION(0,28,0)
                        return QVariant((int)it->toUint32(component));
#else
                        return QVariant((int)it->toLong(component));
#endif
                    else
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                        return QVariant(QMetaType(QMetaType::Int));
#else
                        return QVariant(QVariant::Int);
#endif
                case Exiv2::unsignedRational:
                case Exiv2::signedRational:

                    if (rationalAsListOfInts)
                    {
                        if (it->count() <= component)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                            return QVariant(QMetaType(QMetaType::QVariantList));
#else
                            return QVariant(QVariant::List);
#endif

                        QList<QVariant> list;
                        list << (*it).toRational(component).first;
                        list << (*it).toRational(component).second;

                        return QVariant(list);
                    }
                    else
                    {
                        if (it->count() <= component)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                            return QVariant(QMetaType(QMetaType::Double));
#else
                            return QVariant(QVariant::Double);
#endif

                        // prefer double precision
                        double num = (*it).toRational(component).first;
                        double den = (*it).toRational(component).second;

                        if (den == 0.0)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                            return QVariant(QMetaType(QMetaType::Double));
#else
                            return QVariant(QVariant::Double);
#endif

                        return QVariant(num / den);
                    }
                case Exiv2::date:
                case Exiv2::time:
                {
                    QDateTime dateTime = QDateTime::fromString(QString::fromLatin1(it->toString().c_str()), Qt::ISODate);
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
                        tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));

                    return QVariant(tagValue);
                }
                default:
                    break;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif key '%1' in the image using Exiv2 ").arg(QString::fromLatin1(exifTagName)), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QVariant();
}

QString KExiv2::getExifTagString(const char* exifTagName, bool escapeCR) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData exifData(d->exifMetadata());
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);

        if (it != exifData.end())
        {
            // See B.K.O #184156 comment #13
            std::string val  = it->print(&exifData);
            QString tagValue = QString::fromLocal8Bit(val.c_str());

            if (escapeCR)
                tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));

            return tagValue;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot find Exif key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return QString();
}

bool KExiv2::setExifTagString(const char* exifTagName, const QString& value, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        d->exifMetadata()[exifTagName] = std::string(value.toLatin1().constData());
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif tag string into image using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

QImage KExiv2::getExifThumbnail(bool fixOrientation) const
{
    QImage thumbnail;

    if (d->exifMetadata().empty())
       return thumbnail;

    try
    {
        Exiv2::ExifThumbC thumb(d->exifMetadata());
        Exiv2::DataBuf const c1 = thumb.copy();
#if EXIV2_TEST_VERSION(0,28,0)
        thumbnail.loadFromData(c1.c_data(), c1.size());
#else
        thumbnail.loadFromData(c1.pData_, c1.size_);
#endif

        if (!thumbnail.isNull())
        {
            if (fixOrientation)
            {
                Exiv2::ExifKey key1("Exif.Thumbnail.Orientation");
                Exiv2::ExifKey key2("Exif.Image.Orientation");
                Exiv2::ExifData exifData(d->exifMetadata());
                Exiv2::ExifData::iterator it = exifData.findKey(key1);

                if (it == exifData.end())
                    it = exifData.findKey(key2);

                if (it != exifData.end() && it->count())
                {
#if EXIV2_TEST_VERSION(0,28,0)
                    long orientation = it->toUint32();
#else
                    long orientation = it->toLong();
#endif
                    qCDebug(LIBKEXIV2_LOG) << "Exif Thumbnail Orientation: " << (int)orientation;
                    rotateExifQImage(thumbnail, (ImageOrientation)orientation);
                }

                return thumbnail;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get Exif Thumbnail using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return thumbnail;
}

bool KExiv2::rotateExifQImage(QImage& image, ImageOrientation orientation) const
{
    QTransform matrix = RotationMatrix::toTransform(orientation);

    if ((orientation != ORIENTATION_NORMAL) && (orientation != ORIENTATION_UNSPECIFIED))
    {
        image = image.transformed(matrix);
        return true;
    }

    return false;
}

bool KExiv2::setExifThumbnail(const QImage& thumbImage, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    if (thumbImage.isNull())
    {
        return removeExifThumbnail();
    }

    try
    {
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        thumbImage.save(&buffer, "JPEG");
        Exiv2::ExifThumb thumb(d->exifMetadata());
        thumb.setJpegThumbnail((Exiv2::byte *)data.data(), data.size());
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set Exif Thumbnail using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::setTiffThumbnail(const QImage& thumbImage, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    removeExifThumbnail();

    try
    {
        // Make sure IFD0 is explicitly marked as a main image
        Exiv2::ExifData::const_iterator pos = d->exifMetadata().findKey(Exiv2::ExifKey("Exif.Image.NewSubfileType"));

#if EXIV2_TEST_VERSION(0,28,0)
        if (pos == d->exifMetadata().end() || pos->count() != 1 || pos->toUint32() != 0)
#else
        if (pos == d->exifMetadata().end() || pos->count() != 1 || pos->toLong() != 0)
#endif
        {
#if EXIV2_TEST_VERSION(0,28,0)
            throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, "Exif.Image.NewSubfileType missing or not set as main image");
#elif EXIV2_TEST_VERSION(0,27,0)
            throw Exiv2::Error(Exiv2::kerErrorMessage, "Exif.Image.NewSubfileType missing or not set as main image");
#else
            throw Exiv2::Error(1, "Exif.Image.NewSubfileType missing or not set as main image");
#endif
        }

        // Remove sub-IFD tags
        std::string subImage1("SubImage1");

        for (Exiv2::ExifData::iterator md = d->exifMetadata().begin(); md != d->exifMetadata().end();)
        {
            if (md->groupName() == subImage1)
                md = d->exifMetadata().erase(md);
            else
                ++md;
        }

        if (!thumbImage.isNull())
        {
            // Set thumbnail tags
            QByteArray data;
            QBuffer buffer(&data);
            buffer.open(QIODevice::WriteOnly);
            thumbImage.save(&buffer, "JPEG");

            Exiv2::DataBuf buf((Exiv2::byte *)data.data(), data.size());
            Exiv2::ULongValue val;
            val.read("0");
#if EXIV2_TEST_VERSION(0,28,0)
            val.setDataArea(buf.c_data(), buf.size());
#else
            val.setDataArea(buf.pData_, buf.size_);
#endif
            d->exifMetadata()["Exif.SubImage1.JPEGInterchangeFormat"]       = val;
#if EXIV2_TEST_VERSION(0,28,0)
            d->exifMetadata()["Exif.SubImage1.JPEGInterchangeFormatLength"] = uint32_t(buf.size());
#else
            d->exifMetadata()["Exif.SubImage1.JPEGInterchangeFormatLength"] = uint32_t(buf.size_);
#endif
            d->exifMetadata()["Exif.SubImage1.Compression"]                 = uint16_t(6); // JPEG (old-style)
            d->exifMetadata()["Exif.SubImage1.NewSubfileType"]              = uint32_t(1); // Thumbnail image
            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot set TIFF Thumbnail using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::removeExifThumbnail() const
{
    try
    {
        // Remove all IFD0 subimages.
        Exiv2::ExifThumb thumb(d->exifMetadata());
        thumb.erase();
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot remove Exif Thumbnail using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

KExiv2::TagsMap KExiv2::getStdExifTagsList() const
{
    try
    {
        QList<const Exiv2::TagInfo*> tags;
        TagsMap                      tagsMap;

        const Exiv2::GroupInfo* gi = Exiv2::ExifTags::groupList();

        while (gi->tagList_ != nullptr)
        {
            if (QString::fromLatin1(gi->ifdName_) != QString::fromLatin1("Makernote"))
            {
                Exiv2::TagListFct tl     = gi->tagList_;
                const Exiv2::TagInfo* ti = tl();

                while (ti->tag_ != 0xFFFF)
                {
                    tags << ti;
                    ++ti;
                }
            }
            ++gi;
        }

        for (QList<const Exiv2::TagInfo*>::iterator it = tags.begin(); it != tags.end(); ++it)
        {
            do
            {
                const Exiv2::TagInfo* const ti = *it;
                QString key                    = QLatin1String(Exiv2::ExifKey(*ti).key().c_str());
                QStringList values;
                values << QString::fromLatin1(ti->name_) << QString::fromLatin1(ti->title_) << QString::fromLatin1(ti->desc_);
                tagsMap.insert(key, values);
                ++(*it);
            }
            while((*it)->tag_ != 0xffff);
        }
        return tagsMap;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get Exif Tags list using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return TagsMap();
}

KExiv2::TagsMap KExiv2::getMakernoteTagsList() const
{
    try
    {
        QList<const Exiv2::TagInfo*> tags;
        TagsMap                      tagsMap;

        const Exiv2::GroupInfo* gi = Exiv2::ExifTags::groupList();

        while (gi->tagList_ != nullptr)
        {
            if (QString::fromLatin1(gi->ifdName_) == QString::fromLatin1("Makernote"))
            {
                Exiv2::TagListFct tl     = gi->tagList_;
                const Exiv2::TagInfo* ti = tl();

                while (ti->tag_ != 0xFFFF)
                {
                    tags << ti;
                    ++ti;
                }
            }
            ++gi;
        }

        for (QList<const Exiv2::TagInfo*>::iterator it = tags.begin(); it != tags.end(); ++it)
        {
            do
            {
                const Exiv2::TagInfo* const ti = *it;
                QString key                    = QLatin1String(Exiv2::ExifKey(*ti).key().c_str());
                QStringList values;
                values << QString::fromLatin1(ti->name_) << QString::fromLatin1(ti->title_) << QString::fromLatin1(ti->desc_);
                tagsMap.insert(key, values);
                ++(*it);
            }
            while((*it)->tag_ != 0xffff);
        }

        return tagsMap;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError(QString::fromLatin1("Cannot get Makernote Tags list using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return TagsMap();
}

}  // NameSpace KExiv2Iface
