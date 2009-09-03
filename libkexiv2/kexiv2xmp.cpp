/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Xmp manipulation methods
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

bool KExiv2::canWriteXmp(const QString& filePath)
{
#ifdef _XMP_SUPPORT_
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdXmp);
        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error &e )
    {
        std::string s(e.what());
        kDebug(51003) << "Cannot check Xmp access mode using Exiv2 (Error #" 
                      << e.code() << ": " << s.c_str() << ")" << endl;
    }

    return false;

#else

    return false;

#endif // _XMP_SUPPORT_
}

bool KExiv2::hasXmp() const
{
#ifdef _XMP_SUPPORT_

    return !d->xmpMetadata.empty();

#else

    return false;

#endif // _XMP_SUPPORT_
}

bool KExiv2::clearXmp() const
{
#ifdef _XMP_SUPPORT_

    try
    {
        d->xmpMetadata.clear();
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot clear Xmp data using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QByteArray KExiv2::getXmp() const
{
#ifdef _XMP_SUPPORT_

    try
    {
        if (!d->xmpMetadata.empty())
        {

            std::string xmpPacket;
            Exiv2::XmpParser::encode(xmpPacket, d->xmpMetadata);
            QByteArray data(xmpPacket.data(), xmpPacket.size());
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())


        d->printExiv2ExceptionError("Cannot get Xmp data using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return QByteArray();
}

bool KExiv2::setXmp(const QByteArray& data) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        if (!data.isEmpty())
        {

            std::string xmpPacket;
            xmpPacket.assign(data.data(), data.size());
            if (Exiv2::XmpParser::decode(d->xmpMetadata, xmpPacket) != 0)
                return false;
            else
                return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            kDebug(51003) << "From file " << d->filePath.toAscii().constData() << endl;

        d->printExiv2ExceptionError("Cannot set Xmp data using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

KExiv2::MetaDataMap KExiv2::getXmpTagsDataList(const QStringList &xmpKeysFilter, bool invertSelection) const
{
#ifdef _XMP_SUPPORT_

    if (d->xmpMetadata.empty())
       return MetaDataMap();

    try
    {
        Exiv2::XmpData xmpData = d->xmpMetadata;
        xmpData.sortByKey();

        QString     ifDItemName;
        MetaDataMap metaDataMap;

        for (Exiv2::XmpData::iterator md = xmpData.begin(); md != xmpData.end(); ++md)
        {
            QString key = QString::fromAscii(md->key().c_str());

            // Decode the tag value with a user friendly output.
            std::ostringstream os;
            os << *md;
            QString value = QString::fromUtf8(os.str().c_str());

            // If the tag is a language alternative type, parse content to detect language.
            if (md->typeId() == Exiv2::langAlt)
            {
                QString lang;
                value = detectLanguageAlt(value, lang);
            }
            else
            {
                value = QString::fromUtf8(os.str().c_str());
            }

            // To make a string just on one line.
            value.replace("\n", " ");

            // Some XMP key are redondancy. check if already one exist...
            MetaDataMap::iterator it = metaDataMap.find(key);

            // We apply a filter to get only the XMP tags that we need.

            if (!invertSelection)
            {
                if (xmpKeysFilter.contains(key.section(".", 1, 1)))
                {
                    if (it == metaDataMap.end())
                        metaDataMap.insert(key, value);
                    else
                    {
                        QString v = *it;
                        v.append(", ");
                        v.append(value);
                        metaDataMap.insert(key, v);
                    }
                }
            }
            else
            {
                if (!xmpKeysFilter.contains(key.section(".", 1, 1)))
                {
                    if (it == metaDataMap.end())
                        metaDataMap.insert(key, value);
                    else
                    {
                        QString v = *it;
                        v.append(", ");
                        v.append(value);
                        metaDataMap.insert(key, v);
                    }
                }
            }
        }

        return metaDataMap;
    }
    catch (Exiv2::Error& e)
    {
        d->printExiv2ExceptionError("Cannot parse Xmp metadata using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return MetaDataMap();
}

QString KExiv2::getXmpTagTitle(const char *xmpTagName)
{
#ifdef _XMP_SUPPORT_

    try 
    {
        std::string xmpkey(xmpTagName);
        Exiv2::XmpKey xk(xmpkey);
        return QString::fromLocal8Bit( Exiv2::XmpProperties::propertyTitle(xk) );
    }
    catch (Exiv2::Error& e) 
    {
        d->printExiv2ExceptionError("Cannot get Xmp metadata tag title using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return QString();
}

QString KExiv2::getXmpTagDescription(const char *xmpTagName)
{
#ifdef _XMP_SUPPORT_
    try 
    {
        std::string xmpkey(xmpTagName);
        Exiv2::XmpKey xk(xmpkey);
        return QString::fromLocal8Bit( Exiv2::XmpProperties::propertyDesc(xk) );
    }
    catch (Exiv2::Error& e) 
    {
        d->printExiv2ExceptionError("Cannot get Xmp metadata tag description using Exiv2 ", e);
    }
#endif // _XMP_SUPPORT_

    return QString();
}

QString KExiv2::getXmpTagString(const char* xmpTagName, bool escapeCR) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        Exiv2::XmpData xmpData(d->xmpMetadata);
        Exiv2::XmpKey key(xmpTagName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if (it != xmpData.end())
        {
            std::ostringstream os;
            os << *it;
            QString tagValue = QString::fromUtf8(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QString();
}

bool KExiv2::setXmpTagString(const char *xmpTagName, const QString& value, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        const std::string &txt(value.toUtf8().constData());
        Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::xmpText);
        xmpTxtVal->read(txt);

        d->xmpMetadata.add(Exiv2::XmpKey(xmpTagName), xmpTxtVal.get());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Xmp tag string into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

KExiv2::AltLangMap KExiv2::getXmpTagStringListLangAlt(const char* xmpTagName, bool escapeCR) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        Exiv2::XmpData xmpData = d->xmpMetadata;

        for (Exiv2::XmpData::iterator it = xmpData.begin(); it != xmpData.end(); ++it)
        {
            if (it->key() == xmpTagName && it->typeId() == Exiv2::langAlt)
            {
                AltLangMap map;
                const Exiv2::LangAltValue &value = static_cast<const Exiv2::LangAltValue &>(it->value());

                for (Exiv2::LangAltValue::ValueType::const_iterator it2 = value.value_.begin(); 
                     it2 != value.value_.end(); ++it2) 
                {
                    QString lang = QString::fromUtf8(it2->first.c_str());
                    QString text = QString::fromUtf8(it2->second.c_str());
                    if (escapeCR)
                        text.replace("\n", " ");

                    map.insert(lang, text);
                }

                return map;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return AltLangMap();
}

bool KExiv2::setXmpTagStringListLangAlt(const char *xmpTagName, const KExiv2::AltLangMap& values, 
                                        bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // Remove old XMP alternative Language tag.
        removeXmpTag(xmpTagName);

        if (!values.isEmpty())
        {
            Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::langAlt);

            for (AltLangMap::const_iterator it = values.begin(); it != values.end(); ++it)
            {
                QString lang = it.key();
                QString text = it.value();
                QString txtLangAlt = QString("lang=%1 %2").arg(lang).arg(text);
                const std::string &txt(txtLangAlt.toUtf8().constData());
                xmpTxtVal->read(txt);
            }

            // ...and add the new one instead.
            d->xmpMetadata.add(Exiv2::XmpKey(xmpTagName), xmpTxtVal.get());
        }
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Xmp tag string lang-alt into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QString KExiv2::getXmpTagStringLangAlt(const char* xmpTagName, const QString& langAlt, bool escapeCR) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        Exiv2::XmpData xmpData(d->xmpMetadata);
        Exiv2::XmpKey key(xmpTagName);
        for (Exiv2::XmpData::iterator it = xmpData.begin(); it != xmpData.end(); ++it)
        {
            if (it->key() == xmpTagName && it->typeId() == Exiv2::langAlt)
            {
                for (int i = 0; i < it->count(); i++)
                {
                    std::ostringstream os;
                    os << it->toString(i);
                    QString lang;
                    QString tagValue = QString::fromUtf8(os.str().c_str());
                    tagValue = detectLanguageAlt(tagValue, lang);
                    if (langAlt == lang)
                    {
                        if (escapeCR)
                            tagValue.replace("\n", " ");

                        return tagValue;
                    }
                }
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QString();
}

bool KExiv2::setXmpTagStringLangAlt(const char *xmpTagName, const QString& value, 
                                    const QString& langAlt, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QString language("x-default"); // default alternative language.

        if (!langAlt.isEmpty()) 
            language = langAlt;

        QString txtLangAlt = QString("lang=%1 %2").arg(language).arg(value);

        const std::string &txt(txtLangAlt.toUtf8().constData());
        Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::langAlt);

        // Search if an Xmp tag already exist.

        AltLangMap map = getXmpTagStringListLangAlt(xmpTagName, false);
        if (!map.isEmpty())
        {
            for (AltLangMap::iterator it = map.begin(); it != map.end(); ++it)
            {
                if (it.key() != langAlt)
                {
                    const std::string &val((*it).toUtf8().constData());
                    xmpTxtVal->read(val);
                    kDebug(51003) << *it << endl;
                }
            }
        }

        xmpTxtVal->read(txt);
        removeXmpTag(xmpTagName);
        d->xmpMetadata.add(Exiv2::XmpKey(xmpTagName), xmpTxtVal.get());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Xmp tag string lang-alt into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QStringList KExiv2::getXmpTagStringSeq(const char* xmpTagName, bool escapeCR) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        Exiv2::XmpData xmpData(d->xmpMetadata);
        Exiv2::XmpKey key(xmpTagName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if (it != xmpData.end())
        {
            if (it->typeId() == Exiv2::xmpSeq)
            {
                QStringList seq;

                for (int i = 0; i < it->count(); i++)
                {
                    std::ostringstream os;
                    os << it->toString(i);
                    QString seqValue = QString::fromUtf8(os.str().c_str());

                    if (escapeCR)
                        seqValue.replace("\n", " ");

                    seq.append(seqValue);
                }
                kDebug(51003) << "XMP String Seq (" << xmpTagName << "): " << seq << endl;

                return seq;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QStringList();
}

bool KExiv2::setXmpTagStringSeq(const char *xmpTagName, const QStringList& seq,
                                bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        if (seq.isEmpty())
        {
            removeXmpTag(xmpTagName);
        }
        else
        {
            const QStringList list = seq;
            Exiv2::Value::AutoPtr xmpTxtSeq = Exiv2::Value::create(Exiv2::xmpSeq);

            for (QStringList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
            {
                const std::string &txt((*it).toUtf8().constData());
                xmpTxtSeq->read(txt);
            }
            d->xmpMetadata[xmpTagName].setValue(xmpTxtSeq.get());
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Xmp tag string Seq into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QStringList KExiv2::getXmpTagStringBag(const char* xmpTagName, bool escapeCR) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        Exiv2::XmpData xmpData(d->xmpMetadata);
        Exiv2::XmpKey key(xmpTagName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if (it != xmpData.end())
        {
            if (it->typeId() == Exiv2::xmpBag)
            {
                QStringList bag;

                for (int i = 0; i < it->count(); i++)
                {
                    std::ostringstream os;
                    os << it->toString(i);
                    QString bagValue = QString::fromUtf8(os.str().c_str());

                    if (escapeCR)
                        bagValue.replace("\n", " ");

                    bag.append(bagValue);
                }

                return bag;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QStringList();
}

bool KExiv2::setXmpTagStringBag(const char *xmpTagName, const QStringList& bag,
                                bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        if (bag.isEmpty())
        {
            removeXmpTag(xmpTagName);
        }
        else
        {
            QStringList list = bag;
            Exiv2::Value::AutoPtr xmpTxtBag = Exiv2::Value::create(Exiv2::xmpBag);

            for (QStringList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
            {
                const std::string &txt((*it).toUtf8().constData());
                xmpTxtBag->read(txt);
            }
            d->xmpMetadata[xmpTagName].setValue(xmpTxtBag.get());
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Xmp tag string Bag into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QVariant KExiv2::getXmpTagVariant(const char *xmpTagName, bool rationalAsListOfInts, bool stringEscapeCR) const
{
#ifdef _XMP_SUPPORT_
    try
    {
        Exiv2::XmpData xmpData(d->xmpMetadata);
        Exiv2::XmpKey key(xmpTagName);
        Exiv2::XmpData::iterator it = xmpData.findKey(key);
        if (it != xmpData.end())
        {
            switch (it->typeId())
            {
                case Exiv2::unsignedByte:
                case Exiv2::unsignedShort:
                case Exiv2::unsignedLong:
                case Exiv2::signedShort:
                case Exiv2::signedLong:
                    return QVariant((int)it->toLong());
                case Exiv2::unsignedRational:
                case Exiv2::signedRational:
                    if (rationalAsListOfInts)
                    {
                        QList<QVariant> list;
                        list << (*it).toRational().first;
                        list << (*it).toRational().second;
                        return QVariant(list);
                    }
                    else
                    {
                        // prefer double precision
                        double num = (*it).toRational().first;
                        double den = (*it).toRational().second;
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
                case Exiv2::xmpText:
                {
                    std::ostringstream os;
                    os << *it;
                    QString tagValue = QString::fromUtf8(os.str().c_str());

                    if (stringEscapeCR)
                        tagValue.replace("\n", " ");

                    return tagValue;
                }
                case Exiv2::xmpBag:
                case Exiv2::xmpSeq:
                case Exiv2::xmpAlt:
                {
                    QStringList list;
                    for (int i=0; i < it->count(); i++)
                    {
                        list << QString::fromUtf8(it->toString(i).c_str());
                    }
                    return list;
                }
                case Exiv2::langAlt:
                {
                    // access the value directly
                    const Exiv2::LangAltValue &value = static_cast<const Exiv2::LangAltValue &>(it->value());
                    QMap<QString, QVariant> map;
                    // access the ValueType std::map< std::string, std::string>
                    Exiv2::LangAltValue::ValueType::const_iterator i;
                    for (i = value.value_.begin(); i != value.value_.end(); ++i) 
                    {
                        map[QString::fromUtf8(i->first.c_str())] = QString::fromUtf8(i->second.c_str());
                    }
                    return map;
                }
                default:
                    break;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                    .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QVariant();
}

bool KExiv2::registerXmpNameSpace(const QString& uri, const QString& prefix) const
{
#ifdef _XMP_SUPPORT_

    try
    {
        QString ns = uri;
        if (!uri.endsWith("/")) ns.append("/");
        Exiv2::XmpProperties::registerNs(ns.toAscii().constData(), prefix.toAscii().constData());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot register a new Xmp namespace using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

bool KExiv2::removeXmpTag(const char *xmpTagName, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        Exiv2::XmpKey xmpKey(xmpTagName);
        Exiv2::XmpData::iterator it = d->xmpMetadata.findKey(xmpKey);
        if (it != d->xmpMetadata.end())
        {
            d->xmpMetadata.erase(it);
            return true;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot remove Xmp tag using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
}

QStringList KExiv2::getXmpKeywords() const
{
    return (getXmpTagStringBag("Xmp.dc.subject", false));
}

bool KExiv2::setXmpKeywords(const QStringList& newKeywords, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    QStringList oldkeys = getXmpKeywords();
    QStringList newkeys = newKeywords;

    // Create a list of keywords including old one which already exists.
    for (QStringList::const_iterator it = oldkeys.constBegin(); it != oldkeys.constEnd(); ++it )
    {
        if (!newkeys.contains(*it))
            newkeys.append(*it);
    }

    if (setXmpTagStringBag("Xmp.dc.subject", newkeys, false))
        return true;

#endif // _XMP_SUPPORT_

    return false;
}

QStringList KExiv2::getXmpSubCategories() const
{
    return (getXmpTagStringBag("Xmp.photoshop.SupplementalCategories", false));
}

bool KExiv2::setXmpSubCategories(const QStringList& newSubCategories, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    const QStringList oldSubCat = getXmpSubCategories();
    QStringList newSubCat = newSubCategories;

    // Create a list of sub-categories including old one which already exists.
    for (QStringList::const_iterator it = oldSubCat.constBegin(); it != oldSubCat.constEnd(); ++it )
    {
        if (!newSubCat.contains(*it))
            newSubCat.append(*it);
    }

    if (setXmpTagStringBag("Xmp.photoshop.SupplementalCategories", newSubCat, false))
        return true;

#endif // _XMP_SUPPORT_

    return false;
}

QStringList KExiv2::getXmpSubjects() const
{
    return (getXmpTagStringBag("Xmp.iptc.SubjectCode", false));
}

bool KExiv2::setXmpSubjects(const QStringList& newSubjects, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    QStringList oldSubjectCodes = getXmpSubjects();
    QStringList newSubjectCodes = newSubjects;

    // Create a list of sub-categories including old one which already exists.
    for (QStringList::const_iterator it = oldSubjectCodes.constBegin(); it != oldSubjectCodes.constEnd(); ++it )
    {
        if (!newSubjectCodes.contains(*it))
            newSubjectCodes.append(*it);
    }

    if (setXmpTagStringBag("Xmp.iptc.SubjectCode", newSubjectCodes, false))
        return true;

#endif // _XMP_SUPPORT_

    return false;
}

}  // NameSpace KExiv2Iface
