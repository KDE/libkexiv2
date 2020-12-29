/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Local includes

#include "kexiv2_p.h"
#include "kexiv2.h"
#include "libkexiv2_debug.h"

namespace KExiv2Iface
{

bool KExiv2::canWriteComment(const QString& filePath)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)
                                      (QFile::encodeName(filePath).constData()));

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdComment);
        return (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    }
    catch( Exiv2::Error& e )
    {
        std::string s(e.what());
        qCCritical(LIBKEXIV2_LOG) << "Cannot check Comment access mode using Exiv2 (Error #"
                                  << e.code() << ": " << s.c_str() << ")";
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool KExiv2::hasComments() const
{
    return !d->imageComments().empty();
}

bool KExiv2::clearComments() const
{
    return setComments(QByteArray());
}

QByteArray KExiv2::getComments() const
{
    return QByteArray(d->imageComments().data(), d->imageComments().size());
}

QString KExiv2::getCommentsDecoded() const
{
    return d->detectEncodingAndDecode(d->imageComments());
}

bool KExiv2::setComments(const QByteArray& data) const
{
    d->imageComments() = std::string(data.data(), data.size());
    return true;
}

QString KExiv2::detectLanguageAlt(const QString& value, QString& lang)
{
    // Ex. from an Xmp tag Xmp.tiff.copyright: "lang="x-default" (c) Gilles Caulier 2007"

    if (value.size() > 6 && value.startsWith(QString::fromLatin1("lang=\"")))
    {
        int pos = value.indexOf(QString::fromLatin1("\""), 6);

        if (pos != -1)
        {
            lang = value.mid(6, pos-6);
            return (value.mid(pos+2));
        }
    }

    lang.clear();
    return value;
}

}  // NameSpace KExiv2Iface
