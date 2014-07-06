/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2013-02-21
 * @brief  a command line tool to set faces in Picassa format
 *
 * @author Copyright (C) 2013 by Munteanu Veaceslav
 *         <a href="mailto:slavuttici at gmail dot com">slavuttici at gmail dot com</a>
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

// Qt includes

#include <QString>
#include <QFile>

// KDE includes

#include "kdebug.h"

// Local includes

#include "kexiv2.h"

using namespace KExiv2Iface;

bool setFaceTags(KExiv2& meta,const char* xmpTagName,const QMap<QString,QRectF>& faces,
                     bool setProgramName)
{

        Q_UNUSED(setProgramName);
        meta.setXmpTagString(xmpTagName,QString(),KExiv2::XmpTagType(1),false);

        QString qxmpTagName(xmpTagName);
        QString nameTagKey = qxmpTagName + QString("[%1]/mwg-rs:Name");
        QString typeTagKey = qxmpTagName + QString("[%1]/mwg-rs:Type");
        QString areaTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area");
        QString areaxTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:x");
        QString areayTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:y");
        QString areawTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:w");
        QString areahTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:h");
        QString areanormTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:unit");

        QMap<QString,QRectF>::const_iterator it = faces.constBegin();
        int i =1;
        while(it != faces.constEnd())
        {
            qreal x,y,w,h;
            it.value().getRect(&x,&y,&w,&h);
            /** Set tag name **/
            meta.setXmpTagString(nameTagKey.arg(i).toLatin1(),it.key(),
                                 KExiv2::XmpTagType(0),false);
            /** Set tag type as Face **/
            meta.setXmpTagString(typeTagKey.arg(i).toLatin1(),QString("Face"),
                                 KExiv2::XmpTagType(0),false);
            /** Set tag Area, with xmp type struct **/
            meta.setXmpTagString(areaTagKey.arg(i).toLatin1(),QString(),
                                 KExiv2::XmpTagType(2),false);
            /** Set stArea:x inside Area structure **/
            meta.setXmpTagString(areaxTagKey.arg(i).toLatin1(),QString::number(x),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:y inside Area structure **/
            meta.setXmpTagString(areayTagKey.arg(i).toLatin1(),QString::number(y),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:w inside Area structure **/
            meta.setXmpTagString(areawTagKey.arg(i).toLatin1(),QString::number(w),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:h inside Area structure **/
            meta.setXmpTagString(areahTagKey.arg(i).toLatin1(),QString::number(h),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:unit inside Area structure  as normalized **/
            meta.setXmpTagString(areanormTagKey.arg(i).toLatin1(),QString("normalized"),
                                 KExiv2::XmpTagType(0),false);

            ++it;
            ++i;
        }

    return true;

}

void removeFaceTags(KExiv2& meta,const char* xmpTagName)
{
        QString qxmpTagName(xmpTagName);
        QString regionTagKey = qxmpTagName + QString("[%1]");
        QString nameTagKey = qxmpTagName + QString("[%1]/mwg-rs:Name");
        QString typeTagKey = qxmpTagName + QString("[%1]/mwg-rs:Type");
        QString areaTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area");
        QString areaxTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:x");
        QString areayTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:y");
        QString areawTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:w");
        QString areahTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:h");
        QString areanormTagKey = qxmpTagName + QString("[%1]/mwg-rs:Area/stArea:unit");

        meta.removeXmpTag(xmpTagName,false);
        bool dirty= true;
        int i=1;
        while(dirty)
        {
            dirty = false;
            dirty |=meta.removeXmpTag(regionTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(nameTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(typeTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areaTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areaxTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areayTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areawTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areahTagKey.arg(i).toLatin1(),false);
            dirty |=meta.removeXmpTag(areanormTagKey.arg(i).toLatin1(),false);
            i++;
        }
}
int main (int argc, char **argv)
{
    if(argc != 3)
    {
        kDebug() << "Adding a face rectangle to image";
        kDebug() << "Usage: <add/remove> <image>";
        return -1;
    }

    QString filePath(argv[2]);

    KExiv2Iface::KExiv2::initializeExiv2();
    KExiv2 meta;
    meta.load(filePath);
    meta.setWriteRawFiles(true);

    /** Add a random rectangle with facetag Bob **/
    QString name = "Bob Marley";
    float x =0.5;
    float y =0.5;
    float w = 60;
    float h = 60;

    QRectF rect(x,y,w,h);

    QMap<QString, QRectF> faces;

    faces[name] = rect;

    QString name2 = "Hello Kitty!";
    QRectF rect2(0.4,0.4,30,30);

    faces[name2] = rect2;

    bool g = meta.supportXmp();

    kDebug() << "Image support XMP" << g;

    const QString bag = "Xmp.mwg-rs.Regions/mwg-rs:RegionList";

    QString op(argv[1]);

    if(op == "add")
        setFaceTags(meta,bag.toLatin1(),faces,false);
    else
        removeFaceTags(meta,bag.toLatin1());

        meta.applyChanges();

    QString recoverName = "Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Name";

    KExiv2 meta2;
    meta2.load(filePath);
    meta2.setWriteRawFiles(true);

    QString nameR = meta2.getXmpTagString(recoverName.toLatin1(),false);

    kDebug() << "Saved name is:" << nameR;

    KExiv2Iface::KExiv2::cleanupExiv2();
    return 0;
}