/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-14
 * @brief  Embedded preview loading
 *
 * @author Copyright (C) 2009-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2009-2012 by Marcel Wiesweg
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

#include "kexiv2previews.h"

// Local includes

#include "kexiv2_p.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

class KExiv2Previews::Private
{
public:

    Private()
    {
        manager = 0;
    }

    ~Private()
    {
        delete manager;
    }

    void load(Exiv2::Image::AutoPtr image_)
    {
        image                              = image_;

        image->readMetadata();

        manager                            = new Exiv2::PreviewManager(*image);
        Exiv2::PreviewPropertiesList props = manager->getPreviewProperties();

        // reverse order of list, which is smallest-first
        Exiv2::PreviewPropertiesList::reverse_iterator it;

        for (it = props.rbegin() ; it != props.rend() ; ++it)
        {
            properties << *it;
        }
    }

public:

    Exiv2::Image::AutoPtr           image;
    Exiv2::PreviewManager*          manager;
    QList<Exiv2::PreviewProperties> properties;
};

KExiv2Previews::KExiv2Previews(const QString& filePath)
    : d(new Private)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)(QFile::encodeName(filePath)));
        d->load(image);
    }
    catch( Exiv2::Error& e )
    {
        KExiv2::Private::printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }
}

KExiv2Previews::KExiv2Previews(const QByteArray& imgData)
    : d(new Private)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((Exiv2::byte*)imgData.data(), imgData.size());
        d->load(image);
    }
    catch( Exiv2::Error& e )
    {
        KExiv2::Private::printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
    }
}

KExiv2Previews::~KExiv2Previews()
{
    delete d;
}

bool KExiv2Previews::isEmpty()
{
    return d->properties.isEmpty();
}

QSize KExiv2Previews::originalSize() const
{
    if (d->image.get())
        return QSize(d->image->pixelWidth(), d->image->pixelHeight());

    return QSize();
}

QString KExiv2Previews::originalMimeType() const
{
    if (d->image.get())
        return d->image->mimeType().c_str();

    return QString();
}

int KExiv2Previews::count()
{
    return d->properties.size();
}

int KExiv2Previews::dataSize(int index)
{
    if (index < 0 || index >= size()) return 0;

    return d->properties[index].size_;
}

int KExiv2Previews::width(int index)
{
    if (index < 0 || index >= size()) return 0;

    return d->properties[index].width_;
}

int KExiv2Previews::height(int index)
{
    if (index < 0 || index >= size()) return 0;

    return d->properties[index].height_;
}

QString KExiv2Previews::mimeType(int index)
{
    if (index < 0 || index >= size()) return QString();

    return QString::fromLatin1(d->properties[index].mimeType_.c_str());
}

QString KExiv2Previews::fileExtension(int index)
{
    if (index < 0 || index >= size()) return QString();

    return QString::fromLatin1(d->properties[index].extension_.c_str());
}

QByteArray KExiv2Previews::data(int index)
{
    if (index < 0 || index >= size()) return QByteArray();

    kDebug() << "index: "         << index;
    kDebug() << "d->properties: " << count();

    try
    {
        Exiv2::PreviewImage image = d->manager->getPreviewImage(d->properties[index]);
        return QByteArray((const char*)image.pData(), image.size());
    }
    catch( Exiv2::Error& e )
    {
        KExiv2::Private::printExiv2ExceptionError("Cannot load metadata using Exiv2 ", e);
        return QByteArray();
    }
    catch(...)
    {
        kError() << "Default exception from Exiv2";
        return QByteArray();
    }
}

QImage KExiv2Previews::image(int index)
{
    QByteArray previewData = data(index);
    QImage     image;

    if (!image.loadFromData(previewData))
        return QImage();

    return image;
}

} // namespace KExiv2Iface
