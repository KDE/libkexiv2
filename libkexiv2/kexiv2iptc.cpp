/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               Iptc manipulation methods
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

bool KExiv2::canWriteIptc(const QString& filePath)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath)));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdIptc);
        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error &e )
    {
        std::string s(e.what());
        kDebug(51003) << "Cannot check Iptc access mode using Exiv2 (Error #" 
                      << e.code() << ": " << s.c_str() << ")" << endl;
    }

    return false;
}

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
        d->printExiv2ExceptionError("Cannot clear Iptc data using Exiv2 ", e);
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
                kDebug(51003) << "Exiv2 version is to old. Cannot add Irb header to Iptc metadata" << endl;
                return QByteArray();
#endif
            }
            else 
            {
#if (EXIV2_TEST_VERSION(0,17,91))
                c2 = Exiv2::IptcParser::encode(d->iptcMetadata);
#else
                c2 = iptc.copy();
#endif
            }

            QByteArray data((const char*)c2.pData_, c2.size_);
            return data;

        }
    }
    catch( Exiv2::Error &e )
    {
        if (!d->filePath.isEmpty())
            kDebug(51003) << "From file " << d->filePath.toAscii().constData() << endl;

        d->printExiv2ExceptionError("Cannot get Iptc data using Exiv2 ",e);
    }

    return QByteArray();
}

bool KExiv2::setIptc(const QByteArray& data) const
{
    try
    {
        if (!data.isEmpty())
        {
#if (EXIV2_TEST_VERSION(0,17,91))
            Exiv2::IptcParser::decode(d->iptcMetadata, (const Exiv2::byte*)data.data(), data.size());
            return (!d->iptcMetadata.empty());
#else
            if (d->iptcMetadata.load((const Exiv2::byte*)data.data(), data.size()) != 0)
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

        d->printExiv2ExceptionError("Cannot set Iptc data using Exiv2 ", e);
    }

    return false;
}

KExiv2::MetaDataMap KExiv2::getIptcTagsDataList(const QStringList &iptcKeysFilter, bool invertSelection) const
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
            QString key = QString::fromLocal8Bit(md->key().c_str());

            // Decode the tag value with a user friendly output.
            std::ostringstream os;
            os << *md;
            QString value = QString(os.str().c_str());
            // To make a string just on one line.
            value.replace("\n", " ");

            // Some Iptc key are redondancy. check if already one exist...
            MetaDataMap::iterator it = metaDataMap.find(key);

            // We apply a filter to get only the Iptc tags that we need.

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
        d->printExiv2ExceptionError("Cannot parse Iptc metadata using Exiv2 ", e);
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
        d->printExiv2ExceptionError("Cannot get metadata tag title using Exiv2 ", e);
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
        d->printExiv2ExceptionError("Cannot get metadata tag description using Exiv2 ", e);
    }

    return QString();
}

bool KExiv2::removeIptcTag(const char *iptcTagName, bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        Exiv2::IptcData::iterator it = d->iptcMetadata.begin();
        int i = 0;
        while(it != d->iptcMetadata.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key == QString(iptcTagName))
            {
                it = d->iptcMetadata.erase(it);
                ++i;
            }
            else
            {
                ++it;
            }
        };

        if (i > 0)
            return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot remove Iptc tag using Exiv2 ", e);
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
        d->printExiv2ExceptionError("Cannot set Iptc tag data into image using Exiv2 ", e);
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
            QByteArray data(s, (*it).size());
            delete s;
            return data;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Iptc key '%1' into image using Exiv2 ")
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
            QString tagValue(os.str().c_str());

            if (escapeCR)
                tagValue.replace("\n", " ");

            return tagValue;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Iptc key '%1' into image using Exiv2 ")
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
        d->iptcMetadata[iptcTagName] = std::string(value.toAscii().constData());
        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError("Cannot set Iptc tag string into image using Exiv2 ", e);
    }

    return false;
}

QStringList KExiv2::getIptcTagsStringList(const char* iptcTagName, bool escapeCR) const
{
    try
    {
        if (!d->iptcMetadata.empty())
        {
            QStringList values;
            Exiv2::IptcData iptcData(d->iptcMetadata);

            for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it)
            {
                QString key = QString::fromLocal8Bit(it->key().c_str());

                if (key == QString(iptcTagName))
                {
                    QString tagValue(it->toString().c_str());

                    if (escapeCR)
                        tagValue.replace("\n", " ");

                    values.append(tagValue);
                }
            }

            return values;
        }
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot find Iptc key '%1' into image using Exiv2 ")
                                    .arg(iptcTagName), e);
    }

    return QStringList();
}

bool KExiv2::setIptcTagsStringList(const char* iptcTagName, int maxSize,
                                   const QStringList& oldValues, const QStringList& newValues, 
                                   bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QStringList oldvals = oldValues;
        QStringList newvals = newValues;

        kDebug(51003) << d->filePath.toAscii().constData() << " : " << iptcTagName 
                      << " => " << newvals.join(",").toAscii().constData() << endl;

        // Remove all old values.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());

            // Also remove new values to avoid duplicates. They will be added again below.
            if ( key == QString(iptcTagName) &&
                 (oldvals.contains(val) || newvals.contains(val))
               )
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new values.

        Exiv2::IptcKey iptcTag(iptcTagName);

        for (QStringList::iterator it = newvals.begin(); it != newvals.end(); ++it)
        {
            QString key = *it;
            key.truncate(maxSize);

            Exiv2::Value::AutoPtr val = Exiv2::Value::create(Exiv2::string);
            val->read(key.toLatin1().constData());
            iptcData.add(iptcTag, val.get());
        }

        d->iptcMetadata = iptcData;

        return true;
    }
    catch( Exiv2::Error &e )
    {
        d->printExiv2ExceptionError(QString("Cannot set Iptc key '%1' into image using Exiv2 ")
                                    .arg(iptcTagName), e);
    }

    return false;
}

QStringList KExiv2::getIptcKeywords() const
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
        d->printExiv2ExceptionError("Cannot get Iptc Keywords from image using Exiv2 ", e);
    }

    return QStringList();
}

bool KExiv2::setIptcKeywords(const QStringList& oldKeywords, const QStringList& newKeywords, 
                             bool setProgramName) const
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QStringList oldkeys = oldKeywords;
        QStringList newkeys = newKeywords;

        kDebug(51003) << d->filePath.toAscii().constData() 
                      << " ==> Iptc Keywords: " << newkeys.join(",").toAscii().constData() << endl;

        // Remove all old keywords.
        Exiv2::IptcData iptcData(d->iptcMetadata);
        Exiv2::IptcData::iterator it = iptcData.begin();

        while(it != iptcData.end())
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());
            QString val(it->toString().c_str());

            // Also remove new keywords to avoid duplicates. They will be added again below.
            if ( key == QString("Iptc.Application2.Keywords") &&
                 (oldKeywords.contains(val) || newKeywords.contains(val)) )
                it = iptcData.erase(it);
            else 
                ++it;
        };

        // Add new keywords. Note that Keywords Iptc tag is limited to 64 char but can be redondant.

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
        d->printExiv2ExceptionError("Cannot set Iptc Keywords into image using Exiv2 ", e);
    }

    return false;
}

QStringList KExiv2::getIptcSubjects() const
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
        d->printExiv2ExceptionError("Cannot get Iptc Subjects from image using Exiv2 ", e);
    }

    return QStringList();
}

bool KExiv2::setIptcSubjects(const QStringList& oldSubjects, const QStringList& newSubjects, 
                             bool setProgramName) const
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

        // Add new subjects. Note that Keywords Iptc tag is limited to 236 char but can be redondant.

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
        d->printExiv2ExceptionError("Cannot set Iptc Subjects into image using Exiv2 ", e);
    }

    return false;
}

QStringList KExiv2::getIptcSubCategories() const
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
        d->printExiv2ExceptionError("Cannot get Iptc Sub Categories from image using Exiv2 ", e);
    }

    return QStringList();
}

bool KExiv2::setIptcSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories, 
                                  bool setProgramName) const
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

        // Add new Sub Categories. Note that SubCategories Iptc tag is limited to 32 
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
        d->printExiv2ExceptionError("Cannot set Iptc Sub Categories into image using Exiv2 ", e);
    }

    return false;
}

}  // NameSpace KExiv2Iface
