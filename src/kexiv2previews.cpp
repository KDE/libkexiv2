/*
    SPDX-FileCopyrightText: 2009-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2009-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Local includes

#include "kexiv2previews.h"
#include "kexiv2_p.h"
#include "kexiv2.h"
#include "libkexiv2_debug.h"

namespace KExiv2Iface
{

class KExiv2PreviewsPrivate
{
public:

    KExiv2PreviewsPrivate()
    {
        manager = nullptr;
    }

    ~KExiv2PreviewsPrivate()
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
    : d(new KExiv2PreviewsPrivate)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((const char*)(QFile::encodeName(filePath).constData()));
        d->load(image);
    }
    catch( Exiv2::Error& e )
    {
        KExiv2Private::printExiv2ExceptionError(QString::fromLatin1("Cannot load metadata using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }
}

KExiv2Previews::KExiv2Previews(const QByteArray& imgData)
    : d(new KExiv2PreviewsPrivate)
{
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((Exiv2::byte*)imgData.data(), imgData.size());
        d->load(image);
    }
    catch( Exiv2::Error& e )
    {
        KExiv2Private::printExiv2ExceptionError(QString::fromLatin1("Cannot load metadata using Exiv2 "), e);
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
    }
}

KExiv2Previews::~KExiv2Previews() = default;

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
        return QString::fromLatin1(d->image->mimeType().c_str());

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

    qCDebug(LIBKEXIV2_LOG) << "index: "         << index;
    qCDebug(LIBKEXIV2_LOG) << "d->properties: " << count();

    try
    {
        Exiv2::PreviewImage image = d->manager->getPreviewImage(d->properties[index]);
        return QByteArray((const char*)image.pData(), image.size());
    }
    catch( Exiv2::Error& e )
    {
        KExiv2Private::printExiv2ExceptionError(QString::fromLatin1("Cannot load metadata using Exiv2 "), e);
        return QByteArray();
    }
    catch(...)
    {
        qCCritical(LIBKEXIV2_LOG) << "Default exception from Exiv2";
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
