/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KEXIV2PRIVATE_H
#define KEXIV2PRIVATE_H

#include "kexiv2.h"

 // C++ includes

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <iomanip>
#include <string>

// Qt includes

#include <QFile>
#include <QSize>
#include <QLatin1String>
#include <QFileInfo>
#include <QSharedData>

// Exiv2 includes -------------------------------------------------------

// NOTE: All Exiv2 header must be stay there to not expose external source code to Exiv2 API
//       and reduce Exiv2 dependency to client code.

// The pragmas are required to be able to catch exceptions thrown by libexiv2:
// See https://gcc.gnu.org/wiki/Visibility, the section about c++ exceptions.
// They are needed for all libexiv2 versions that do not care about visibility.
#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

#include <exiv2/exiv2.hpp>

// Check if Exiv2 support XMP

#ifdef EXV_HAVE_XMP_TOOLKIT
#   define _XMP_SUPPORT_ 1
#endif

// With exiv2 > 0.20.0, all makernote header files have been removed to increase binary compatibility.
// See Exiv2 bugzilla entry https://dev.exiv2.org/issues/719
// and wiki topic           https://dev.exiv2.org/boards/3/topics/583

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

// End of Exiv2 headers ------------------------------------------------------

namespace KExiv2Iface
{

class KExiv2DataPrivate : public QSharedData
{
public:

    void clear();

public:

    std::string     imageComments;

    Exiv2::ExifData exifMetadata;

    Exiv2::IptcData iptcMetadata;

#ifdef _XMP_SUPPORT_
    Exiv2::XmpData  xmpMetadata;
#endif
};

// --------------------------------------------------------------------------

class KExiv2Private
{
public:

    KExiv2Private();
    ~KExiv2Private();

    void copyPrivateData(const KExiv2Private* const other);

    bool saveToXMPSidecar(const QFileInfo& finfo)                            const;
    bool saveToFile(const QFileInfo& finfo)                                  const;
#if EXIV2_TEST_VERSION(0,28,0)
    bool saveOperations(const QFileInfo& finfo, Exiv2::Image::UniquePtr image) const;
#else
    bool saveOperations(const QFileInfo& finfo, Exiv2::Image::AutoPtr image) const;
#endif

    /** Wrapper method to convert a Comments content to a QString.
     */
    QString convertCommentValue(const Exiv2::Exifdatum& exifDatum) const;

    /** Charset autodetection to convert a string to a QString.
     */
    QString detectEncodingAndDecode(const std::string& value)      const;

    /** UTF8 autodetection from a string.
     */
    bool isUtf8(const char* const buffer)                          const;

    int getXMPTagsListFromPrefix(const QString& pf, KExiv2::TagsMap& tagsMap) const;

    const Exiv2::ExifData& exifMetadata()  const { return data.constData()->exifMetadata;  }
    const Exiv2::IptcData& iptcMetadata()  const { return data.constData()->iptcMetadata;  }
    const std::string&     imageComments() const { return data.constData()->imageComments; }

#ifdef _XMP_SUPPORT_
    const Exiv2::XmpData&  xmpMetadata()   const { return data.constData()->xmpMetadata;   }
#endif

    Exiv2::ExifData&       exifMetadata()        { return data.data()->exifMetadata;       }
    Exiv2::IptcData&       iptcMetadata()        { return data.data()->iptcMetadata;       }
    std::string&           imageComments()       { return data.data()->imageComments;      }

#ifdef _XMP_SUPPORT_
    Exiv2::XmpData&        xmpMetadata()         { return data.data()->xmpMetadata;        }

#if EXIV2_TEST_VERSION(0,28,0)
    void loadSidecarData(Exiv2::Image::UniquePtr xmpsidecar);
#else
    void loadSidecarData(Exiv2::Image::AutoPtr xmpsidecar);
#endif
#endif

public:

    /** Generic method to print the Exiv2 C++ Exception error message from 'e'.
     *  'msg' string is printed using kDebug rules..
     */
    static void printExiv2ExceptionError(const QString& msg, Exiv2::Error& e);

    /** Generic method to print debug message from Exiv2.
     *  'msg' string is printed using kDebug rules. 'lvl' is the debug level of Exiv2 message.
     */
    static void printExiv2MessageHandler(int lvl, const char* msg);

public:

    bool                                           writeRawFiles;
    bool                                           updateFileTimeStamp;

    bool                                           useXMPSidecar4Reading;

    /// A mode from #MetadataWritingMode enum.
    int                                            metadataWritingMode;

    /// XMP, and parts of EXIF/IPTC, were loaded from an XMP sidecar file
    bool                                           loadedFromSidecar;

    QString                                        filePath;
    QSize                                          pixelSize;
    QString                                        mimeType;

    QSharedDataPointer<KExiv2DataPrivate> data;
};

// --------------------------------------------------------------------------------------------

template <class Data, class Key, class KeyString, class KeyStringList = QList<KeyString> >

class MergeHelper
{
public:

    KeyStringList keys;

    MergeHelper& operator<<(const KeyString& key)
    {
        keys << key;
        return *this;
    }

    /**
      * Merge two (Exif,IPTC,Xmp)Data packages, where the result is stored in dest
      * and fields from src take precedence over existing data from dest.
      */
    void mergeAll(const Data& src, Data& dest)
    {
        for (typename Data::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            typename Data::iterator destIt = dest.findKey(Key(it->key()));

            if (destIt == dest.end())
            {
                dest.add(*it);
            }
            else
            {
                *destIt = *it;
            }
        }
    }

    /**
     * Merge two (Exif,IPTC,Xmp)Data packages, the result is stored in dest.
     * Only keys in keys are considered for merging.
     * Fields from src take precedence over existing data from dest.
     */
    void mergeFields(const Data& src, Data& dest) const
    {
        for (const KeyString& keyString : keys)
        {
            Key key(keyString.latin1());
            typename Data::const_iterator it = src.findKey(key);

            if (it == src.end())
            {
                continue;
            }

            typename Data::iterator destIt = dest.findKey(key);

            if (destIt == dest.end())
            {
                dest.add(*it);
            }
            else
            {
                *destIt = *it;
            }
        }
    }

    /**
     * Merge two (Exif,IPTC,Xmp)Data packages, the result is stored in dest.
     * The following steps apply only to keys in "keys":
     * The result is determined by src.
     * Keys must exist in src to kept in dest.
     * Fields from src take precedence over existing data from dest.
     */
    void exclusiveMerge(const Data& src, Data& dest) const
    {
        for (const KeyString& keyString : keys)
        {
            Key key(keyString.latin1());
            typename Data::const_iterator it = src.findKey(key);
            typename Data::iterator destIt = dest.findKey(key);

            if (destIt == dest.end())
            {
                if (it != src.end())
                {
                    dest.add(*it);
                }
            }
            else
            {
                if (it == src.end())
                {
                    dest.erase(destIt);
                }
                else
                {
                    *destIt = *it;
                }
            }
        }
    }
};

class ExifMergeHelper : public MergeHelper<Exiv2::ExifData, Exiv2::ExifKey, QLatin1String>
{
};

class IptcMergeHelper : public MergeHelper<Exiv2::IptcData, Exiv2::IptcKey, QLatin1String>
{
};

#ifdef _XMP_SUPPORT_
class XmpMergeHelper : public MergeHelper<Exiv2::XmpData, Exiv2::XmpKey, QLatin1String>
{
};
#endif

}  // NameSpace KExiv2Iface

#endif // KEXIV2PRIVATE_H
