/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2007-09-03
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2006-2012 by Gilles Caulier
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

#include <QBuffer>
#include <QFile>
#include <QImage>
#include <QSize>
#include <QTextCodec>
#include <QMatrix>
#include <QFileInfo>
#include <QDataStream>
#include <QSharedData>

// KDE includes

#include <ktemporaryfile.h>
#include <kencodingdetector.h>
#include <kstringhandler.h>
#include <kdeversion.h>
#include <kdebug.h>

// Exiv2 includes -------------------------------------------------------

// NOTE: All Exiv2 header must be stay there to not expose external source code to Exiv2 API
//       and reduce Exiv2 dependency to client code.

// The pragmas are required to be able to catch exceptions thrown by libexiv2:
// See http://gcc.gnu.org/wiki/Visibility, the section about c++ exceptions.
// They are needed for all libexiv2 versions that do not care about visibility.
#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

#include <exiv2/exv_conf.h>
#include <exiv2/error.hpp>
#include <exiv2/image.hpp>
#include <exiv2/jpgimage.hpp>
#include <exiv2/datasets.hpp>
#include <exiv2/tags.hpp>
#include <exiv2/preview.hpp>
#include <exiv2/properties.hpp>
#include <exiv2/types.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/xmpsidecar.hpp>

// Check if Exiv2 support XMP

#ifdef EXV_HAVE_XMP_TOOLKIT
#   define _XMP_SUPPORT_ 1
#endif

// Make sure an EXIV2_TEST_VERSION macro exists:

#ifdef EXIV2_VERSION
#    ifndef EXIV2_TEST_VERSION
#        define EXIV2_TEST_VERSION(major,minor,patch) \
         ( EXIV2_VERSION >= EXIV2_MAKE_VERSION(major,minor,patch) )
#    endif
#else
#    define EXIV2_TEST_VERSION(major,minor,patch) (false)
#endif

// With exiv2 > 0.20.0, all makernote header files have been removed to increase binary compatibility.
// See Exiv2 bugzilla entry http://dev.exiv2.org/issues/719
// and wiki topic           http://dev.exiv2.org/boards/3/topics/583

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

// End of Exiv2 headers ------------------------------------------------------

namespace KExiv2Iface
{

class KExiv2Data::Private : public QSharedData
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

class KExiv2::Private
{
public:

    Private();
    ~Private();

    void copyPrivateData(const Private* const other);

    bool saveToXMPSidecar(const QFileInfo& finfo)    const;
    bool saveToFile(const QFileInfo& finfo)          const;
    bool saveOperations(const QFileInfo& finfo, Exiv2::Image::AutoPtr image) const;

    /** Wrapper method to convert a Comments content to a QString.
     */
    QString convertCommentValue(const Exiv2::Exifdatum& exifDatum) const;

    /** Charset autodetection to convert a string to a QString.
     */
    QString detectEncodingAndDecode(const std::string& value)      const;

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

    /**
     * Merge two XmpData packages, where the result is stored in dest
     * and fields from src take precedence over existing data from dest.
     */
    void mergeXmpData(const Exiv2::XmpData& src, Exiv2::XmpData& dest);
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

    QString                                        filePath;
    QSize                                          pixelSize;
    QString                                        mimeType;

    QSharedDataPointer<KExiv2Data::Private> data;
};

}  // NameSpace KExiv2Iface

#endif // KEXIV2PRIVATE_H
