/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2007-09-03
 * @brief  Exiv2 library interface for KDE
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2010 by Marcel Wiesweg
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

// Local includes.

#include "kexiv2_p.h"

namespace KExiv2Iface
{

void KExiv2DataPriv::clear()
{
    imageComments.clear();
    exifMetadata.clear();
    iptcMetadata.clear();
    xmpMetadata.clear();
}

KExiv2Priv::KExiv2Priv()
          : data(new KExiv2DataPriv)
{
    writeRawFiles       = false;
    updateFileTimeStamp = false;
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

QString KExiv2Priv::convertCommentValue(const Exiv2::Exifdatum& exifDatum)
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
#if (EXIV2_TEST_VERSION(0,20,0))
            return QString::fromUtf8(comment.data());
#else
            // Older versions give a UCS2-String, see bug #205824

            // QString expects a null-terminated UCS-2 string.
            // Is it already null terminated? In any case, add termination "\0\0" for safety.
            comment.resize(comment.length() + 2, '\0');
            return QString::fromUtf16((unsigned short *)comment.data());
#endif
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
    catch( Exiv2::Error& e )
    {
        printExiv2ExceptionError("Cannot convert Comment using Exiv2 ", e);
    }

    return QString();
}

QString KExiv2Priv::detectEncodingAndDecode(const std::string& value)
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

int KExiv2Priv::getXMPTagsListFromPrefix(const QString& pf, KExiv2::TagsMap& tagsMap)
{
    QList<const Exiv2::XmpPropertyInfo*> tags;
    tags << Exiv2::XmpProperties::propertyList(pf.toAscii().data());
    int i = 0;

    for (QList<const Exiv2::XmpPropertyInfo*>::iterator it = tags.begin(); it != tags.end(); ++it)
    {
        do
        {
            QString     key = QLatin1String( Exiv2::XmpKey( pf.toAscii().data(), (*it)->name_ ).key().c_str() );
            QStringList values;
            values << (*it)->name_ << (*it)->title_ << (*it)->desc_;
            tagsMap.insert(key, values);
            ++(*it);
            i++;
        }
        while( !QString((*it)->name_).isNull() );
    }
    return i;
}

}  // NameSpace KExiv2Iface
