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

namespace KExiv2Iface
{

KExiv2Priv::KExiv2Priv()
{
    imageComments = std::string();
}

KExiv2Priv::~KExiv2Priv()
{
#ifdef _XMP_SUPPORT_
    // Fix memory leak if Exiv2 support XMP.
    Exiv2::XmpParser::terminate();
#endif // _XMP_SUPPORT_
}

bool KExiv2Priv::setExif(Exiv2::DataBuf const data)
{
    try
    {
        if (data.size_ != 0)
        {
#if (EXIV2_TEST_VERSION(0,17,91))
            Exiv2::ExifParser::decode(exifMetadata, data.pData_, data.size_);
            return (!exifMetadata.empty());
#else
            if (exifMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
#endif
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!filePath.isEmpty())
            qDebug ("From file %s", filePath.ascii());

        printExiv2ExceptionError("Cannot set Exif data using Exiv2 ", e);
    }

    return false;
}

bool KExiv2Priv::setIptc(Exiv2::DataBuf const data)
{
    try
    {
        if (data.size_ != 0)
        {
#if (EXIV2_TEST_VERSION(0,17,91))
            Exiv2::IptcParser::decode(iptcMetadata, data.pData_, data.size_);
            return (!iptcMetadata.empty());
#else
            if (iptcMetadata.load(data.pData_, data.size_) != 0)
                return false;
            else
                return true;
#endif
        }
    }
    catch( Exiv2::Error &e )
    {
        if (!filePath.isEmpty())
            qDebug ("From file %s", filePath.ascii());

        printExiv2ExceptionError("Cannot set Iptc data using Exiv2 ", e);
    }

    return false;
}

void KExiv2Priv::printExiv2ExceptionError(const QString& msg, Exiv2::Error& e)
{
    std::string s(e.what());
    qDebug("%s (Error #%i: %s)", msg.ascii(), e.code(), s.c_str());
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
        static_cast<Exiv2::Value &>(commentValue).read(data, value.size(), Exiv2::invalidByteOrder);
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
            return QString::fromUcs2((unsigned short *)comment.data());
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

    if (value.empty())
        return QString();

#if KDE_IS_VERSION(3,2,0)
    if (KStringHandler::isUtf8(value.c_str()))
    {
        return QString::fromUtf8(value.c_str());
    }
#else
    // anyone who is still running KDE 3.0 or 3.1 is missing so many features
    // that he will have to accept this missing feature.
    return QString::fromUtf8(value.c_str());
#endif

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // We try if QTextCodec can decide here, otherwise we use Latin1.
    // Or use local8Bit as default?

    // load QTextCodecs
    QTextCodec *latin1Codec = QTextCodec::codecForName("iso8859-1");
    //QTextCodec *utf8Codec   = QTextCodec::codecForName("utf8");
    QTextCodec *localCodec  = QTextCodec::codecForLocale();

    // make heuristic match
    int latin1Score = latin1Codec->heuristicContentMatch(value.c_str(), value.length());
    int localScore  = localCodec->heuristicContentMatch(value.c_str(), value.length());

    // convert string:
    // Use whatever has the larger score, local or ASCII
    if (localScore >= 0 && localScore >= latin1Score)
    {
        // workaround for bug #134999:
        // The QLatin15Codec may crash if strlen < value.length()
        int length = value.length();
        if (localCodec->name() == QString::fromLatin1("ISO 8859-15"))
            length = strlen(value.c_str());
        return localCodec->toUnicode(value.c_str(), length);
    }
    else
        return QString::fromLatin1(value.c_str());
}

}  // NameSpace KExiv2Iface
