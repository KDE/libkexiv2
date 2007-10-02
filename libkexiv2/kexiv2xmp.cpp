/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Xmp manipulation methods
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
        printExiv2ExceptionError("Cannot clear Xmp data using Exiv2 ", e);
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
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot get Xmp data using Exiv2 ", e);
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
            qDebug ("From file %s", d->filePath.toAscii().constData());

        printExiv2ExceptionError("Cannot set Xmp data using Exiv2 ", e);
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

            qDebug() << key << " = " << value << endl; 

            // If the tag is a language alternative type, parse content to detect encoding.
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
        printExiv2ExceptionError("Cannot parse Xmp metadata using Exiv2 ", e);
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
        printExiv2ExceptionError("Cannot get Xmp metadata tag title using Exiv2 ", e);
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
        printExiv2ExceptionError("Cannot get Xmp metadata tag description using Exiv2 ", e);
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
        printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
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
        printExiv2ExceptionError("Cannot set Xmp tag string into image using Exiv2 ", e);
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
                std::ostringstream os;
                os << *it;
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
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
                                 .arg(xmpTagName), e);
    }

#endif // _XMP_SUPPORT_

    return QString();
}

bool KExiv2::setXmpTagStringLangAlt(const char *xmpTagName, const QString& value, 
                                    const QString& altLang, bool setProgramName) const
{
#ifdef _XMP_SUPPORT_

    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QString lang("x-default"); // default alternative language.

        if (!altLang.isEmpty()) 
            lang = altLang;

        QString txtLangAlt = QString("lang=%1 %2").arg(lang).arg(value);

        Exiv2::Value::AutoPtr xmpTxtVal = Exiv2::Value::create(Exiv2::langAlt);
        const std::string &txt(txtLangAlt.toUtf8().constData());
        xmpTxtVal->read(txt);
        d->xmpMetadata.add(Exiv2::XmpKey(xmpTagName), xmpTxtVal.get());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Xmp tag string lang-alt into image using Exiv2 ", e);
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

                for (int i = 0; i < it->count(); ++i)
                {
                    std::ostringstream os;
                    os << it->toString(i);
                    QString seqValue = QString::fromUtf8(os.str().c_str());
    
                    if (escapeCR)
                        seqValue.replace("\n", " ");

                    seq.append(seqValue);
                }
                qDebug() << "XMP String Seq (" << xmpTagName << "): " << seq << endl;  

                return seq;
            }
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find Xmp key '%1' into image using Exiv2 ")
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
        QStringList list = seq;
        Exiv2::Value::AutoPtr xmpTxtSeq = Exiv2::Value::create(Exiv2::xmpSeq);

        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        {
            const std::string &txt((*it).toUtf8().constData());
            xmpTxtSeq->read(txt);
        }
        d->xmpMetadata.add(Exiv2::XmpKey(xmpTagName), xmpTxtSeq.get());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Xmp tag string lang-alt into image using Exiv2 ", e);
    }

#endif // _XMP_SUPPORT_

    return false;
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
        printExiv2ExceptionError("Cannot register a new Xmp namespace using Exiv2 ", e);
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
        printExiv2ExceptionError("Cannot remove Xmp tag using Exiv2 ", e);
    }        

#endif // _XMP_SUPPORT_

    return false;
}

}  // NameSpace KExiv2Iface
