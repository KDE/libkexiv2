/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Iptc manipulation methods
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

bool KExiv2::hasIptc() const
{
    return !d->iptcMetadata.empty();
}

bool KExiv2::clearIptc() const
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
#if (EXIV2_TEST_VERSION(0,10,0))
                c2 = Exiv2::Photoshop::setIptcIrb(0, 0, iptc);
#else
                qDebug("Exiv2 version is to old. Cannot add Irb header to IPTC metadata");
                return QByteArray();
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

bool KExiv2::setIptc(const QByteArray& data) const
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

KExiv2::MetaDataMap KExiv2::getIptcTagsDataList(const QStringList &iptcKeysFilter, bool invertSelection)
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

            // We apply a filter to get only the IPTC tags that we need.

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
                        metaDataMap.insert(key, v);
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
                        metaDataMap.insert(key, v);
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

bool KExiv2::removeIptcTag(const char *iptcTagName, bool setProgramName) const
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

bool KExiv2::setIptcTagData(const char *iptcTagName, const QByteArray& data, bool setProgramName) const
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

QByteArray KExiv2::getIptcTagData(const char *iptcTagName) const
{
    try
    {
        Exiv2::IptcKey iptcKey(iptcTagName);
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.findKey(iptcKey);
        if (it != iptcData.end())
        {
            char *s = new char[(*it).size()];
            (*it).copy((Exiv2::byte*)s, Exiv2::bigEndian);
            QByteArray data = QByteArray::fromRawData(s, (*it).size());
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

bool KExiv2::setIptcTagString(const char *iptcTagName, const QString& value, bool setProgramName) const
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

}  // NameSpace KExiv2Iface
