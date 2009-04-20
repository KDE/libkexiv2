/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-09-03
 * Description : Exiv2 library interface for KDE
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

namespace KExiv2Iface
{

KExiv2Priv::KExiv2Priv()
{
    writeRawFiles       = false;
    updateFileTimeStamp = false;
    imageComments       = std::string();
}

KExiv2Priv::~KExiv2Priv()
{
}

void KExiv2Priv::printExiv2ExceptionError(const QString& msg, Exiv2::Error& e)
{
    std::string s(e.what());
    kDebug(51003) << msg.toAscii().constData() << " (Error #" 
                  << e.code() << ": " << s.c_str() << endl;
}

QString KExiv2Priv::convertCommentValue(const Exiv2::Exifdatum &exifDatum)
{
    try
    {
        std::string comment;
        std::string charset;

#if (EXIV2_TEST_VERSION(0,11,0))
        comment = exifDatum.toString();
#else
        // workaround for bug in TIFF parser: CommentValue is loaded as DataValue
        const Exiv2::Value &value = exifDatum.value();
        Exiv2::byte *data = new Exiv2::byte[value.size()];
        value.copy(data, Exiv2::invalidByteOrder);
        Exiv2::CommentValue commentValue;
        // this read method is hidden in CommentValue
        static_cast<Exiv2::Value &>(commentValue).read(data, value.size(),
                                                    Exiv2::invalidByteOrder);
        comment = commentValue.toString();
        delete [] data;
#endif

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
            // QString expects a null-terminated UCS-2 string.
            // Is it already null terminated? In any case, add termination "\0\0" for safety.
            comment.resize(comment.length() + 2, '\0');
            return QString::fromUtf16((unsigned short *)comment.data());
        }
        else if (charset == "\"Jis\"")
        {
            QTextCodec *codec = QTextCodec::codecForName("JIS7");
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
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot convert Comment using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2Priv::detectEncodingAndDecode(const std::string &value)
{
    // For charset autodetection, we could use sophisticated code
    // (Mozilla chardet, KHTML's autodetection, QTextCodec::codecForContent),
    // but that is probably too much.
    // We check for UTF8, Local encoding and ASCII.
    // TODO: Gilles ==> Marcel : Look like KEncodingDetector class can provide a full implementation for encoding detection.

    if (value.empty())
        return QString();

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

}  // NameSpace KExiv2Iface
