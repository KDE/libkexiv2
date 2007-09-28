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

bool KExiv2::hasXmp()
{
#ifdef _XMP_SUPPORT_
    return !d->xmpMetadata.empty();
#else
    return false;
#endif // _XMP_SUPPORT_
}

bool KExiv2::clearXmp()
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

bool KExiv2::setXmp(const QByteArray& data)
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

KExiv2::MetaDataMap KExiv2::getXmpTagsDataList(const QStringList &xmpKeysFilter, bool invertSelection)
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

            // If the tag is a language alternative type, parse content to detect encoding.
            if (md->typeId() == Exiv2::langAlt)
            {
                QString lang;
                qDebug() << "Language Alternative string to parse : " << value << endl;
                value = detectLanguageAlt(value, lang);
                qDebug() << "Language= " << lang << endl;
                qDebug() << "String Extracted= " << value << endl;
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
        printExiv2ExceptionError("Cannot parse XMP metadata using Exiv2 ", e);
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
        printExiv2ExceptionError("Cannot get metadata tag title using Exiv2 ", e);
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
        printExiv2ExceptionError("Cannot get metadata tag description using Exiv2 ", e);
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
            QString tagValue = QString::fromLocal8Bit(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError(QString("Cannot find XMp key '%1' into image using Exiv2 ")
                                 .arg(xmpTagName), e);
    }
#endif // _XMP_SUPPORT_

    return QString();
}

bool KExiv2::setXmpTagString(const char *xmpTagName, const QString& value, bool setProgramName)
{
#ifdef _XMP_SUPPORT_
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        const std::string &txt(value.toAscii().constData());
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

}  // NameSpace KExiv2Iface
