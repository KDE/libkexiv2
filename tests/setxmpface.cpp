/*
    A command line tool to set faces in Picassa format

    SPDX-FileCopyrightText: 2013 Munteanu Veaceslav <slavuttici at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt includes

#include <QString>
#include <QFile>
#include <QDebug>

// Local includes

#include "kexiv2.h"

using namespace KExiv2Iface;

bool setFaceTags(KExiv2& meta,const char* xmpTagName,const QMap<QString,QRectF>& faces,
                     bool setProgramName)
{

        Q_UNUSED(setProgramName);
        meta.setXmpTagString(xmpTagName,QString(),KExiv2::XmpTagType(1),false);

        QString qxmpTagName(QString::fromLatin1(xmpTagName));
        QString nameTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Name");
        QString typeTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Type");
        QString areaTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area");
        QString areaxTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:x");
        QString areayTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:y");
        QString areawTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:w");
        QString areahTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:h");
        QString areanormTagKey = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:unit");

        QMap<QString,QRectF>::const_iterator it = faces.constBegin();
        int i =1;
        while(it != faces.constEnd())
        {
            qreal x,y,w,h;
            it.value().getRect(&x,&y,&w,&h);
            /** Set tag name **/
            meta.setXmpTagString(nameTagKey.arg(i).toLatin1().constData(),it.key(),
                                 KExiv2::XmpTagType(0),false);
            /** Set tag type as Face **/
            meta.setXmpTagString(typeTagKey.arg(i).toLatin1().constData(),QString::fromLatin1("Face"),
                                 KExiv2::XmpTagType(0),false);
            /** Set tag Area, with xmp type struct **/
            meta.setXmpTagString(areaTagKey.arg(i).toLatin1().constData(),QString(),
                                 KExiv2::XmpTagType(2),false);
            /** Set stArea:x inside Area structure **/
            meta.setXmpTagString(areaxTagKey.arg(i).toLatin1().constData(),QString::number(x),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:y inside Area structure **/
            meta.setXmpTagString(areayTagKey.arg(i).toLatin1().constData(),QString::number(y),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:w inside Area structure **/
            meta.setXmpTagString(areawTagKey.arg(i).toLatin1().constData(),QString::number(w),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:h inside Area structure **/
            meta.setXmpTagString(areahTagKey.arg(i).toLatin1().constData(),QString::number(h),
                                 KExiv2::XmpTagType(0),false);
            /** Set stArea:unit inside Area structure  as normalized **/
            meta.setXmpTagString(areanormTagKey.arg(i).toLatin1().constData(),QString::fromLatin1("normalized"),
                                 KExiv2::XmpTagType(0),false);

            ++it;
            ++i;
        }

    return true;

}

void removeFaceTags(KExiv2& meta,const char* xmpTagName)
{
        QString qxmpTagName(QString::fromLatin1(xmpTagName));
        QString regionTagKey   = qxmpTagName + QString::fromLatin1("[%1]");
        QString nameTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Name");
        QString typeTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Type");
        QString areaTagKey     = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area");
        QString areaxTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:x");
        QString areayTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:y");
        QString areawTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:w");
        QString areahTagKey    = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:h");
        QString areanormTagKey = qxmpTagName + QString::fromLatin1("[%1]/mwg-rs:Area/stArea:unit");

        meta.removeXmpTag(xmpTagName,false);
        bool dirty = true;
        int i      =1;

        while(dirty)
        {
            dirty = false;
            dirty |=meta.removeXmpTag(regionTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(nameTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(typeTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areaTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areaxTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areayTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areawTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areahTagKey.arg(i).toLatin1().constData(),false);
            dirty |=meta.removeXmpTag(areanormTagKey.arg(i).toLatin1().constData(),false);
            i++;
        }
}

int main (int argc, char **argv)
{
    if (argc != 3)
    {
        qDebug() << "Adding a face rectangle to image";
        qDebug() << "Usage: <add/remove> <image>";
        return -1;
    }

    QString filePath(QString::fromLatin1(argv[2]));

    KExiv2Iface::KExiv2::initializeExiv2();
    KExiv2 meta;
    meta.load(filePath);
    meta.setWriteRawFiles(true);

    /** Add a random rectangle with facetag Bob **/
    QString name = QString::fromLatin1("Bob Marley");
    float x =0.5;
    float y =0.5;
    float w = 60;
    float h = 60;

    QRectF rect(x,y,w,h);

    QMap<QString, QRectF> faces;

    faces[name] = rect;

    QString name2 = QString::fromLatin1("Hello Kitty!");
    QRectF rect2(0.4,0.4,30,30);

    faces[name2] = rect2;

    bool g = meta.supportXmp();

    qDebug() << "Image support XMP" << g;

    const QString bag = QString::fromLatin1("Xmp.mwg-rs.Regions/mwg-rs:RegionList");

    QString op(QString::fromLatin1(argv[1]));

    if (op == QString::fromLatin1("add"))
        setFaceTags(meta,bag.toLatin1().constData(),faces,false);
    else
        removeFaceTags(meta,bag.toLatin1().constData());

    meta.applyChanges();

    QString recoverName = QString::fromLatin1("Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Name");

    KExiv2 meta2;
    meta2.load(filePath);
    meta2.setWriteRawFiles(true);

    QString nameR = meta2.getXmpTagString(recoverName.toLatin1().constData(),false);

    qDebug() << "Saved name is:" << nameR;

    KExiv2Iface::KExiv2::cleanupExiv2();
    return 0;
}
