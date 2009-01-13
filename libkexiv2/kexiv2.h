/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * Exiv2: http://www.exiv2.org
 * Exif : http://www.exif.org/Exif2-2.PDF 
 * Iptc : http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
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

#ifndef KEXIV2_H
#define KEXIV2_H

// QT includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qmap.h>

// Local includes.

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class KExiv2Priv;

class LIBKEXIV2_EXPORT KExiv2
{

public:

    /** The image color workspace values given by Exif metadata. 
     */
    enum ImageColorWorkSpace
    {
        WORKSPACE_UNSPECIFIED  = 0,
        WORKSPACE_SRGB         = 1,
        WORKSPACE_ADOBERGB     = 2,
        WORKSPACE_UNCALIBRATED = 65535
    };

    /** The image orientation values given by Exif metadata. 
     */
    enum ImageOrientation
    {
        ORIENTATION_UNSPECIFIED  = 0,
        ORIENTATION_NORMAL       = 1,
        ORIENTATION_HFLIP        = 2,
        ORIENTATION_ROT_180      = 3,
        ORIENTATION_VFLIP        = 4,
        ORIENTATION_ROT_90_HFLIP = 5,
        ORIENTATION_ROT_90       = 6,
        ORIENTATION_ROT_90_VFLIP = 7,
        ORIENTATION_ROT_270      = 8
    };

    /** A map used by decodeExifMetadata() decodeIptcMetadata() methods
        to store Tags Key and Tags Value. 
     */
    typedef QMap<QString, QString>  MetaDataMap;

public:

    /** Standard constructor. 
     */
    KExiv2();

    /** Contructor to Load Metadata from image file. 
     */
    KExiv2(const QString& filePath);

    /** Standard destructor 
     */
    virtual ~KExiv2();

    /** Return true if library can writte metadata to typeMime file format.
     */
    static bool supportMetadataWritting(const QString& typeMime);

    /** Return a string version of Exiv2 release in format "major.minor.patch" 
     */
    static QString Exiv2Version();

    /** Return a string version of libkexiv2 release 
     */
    static QString version();

    //-- Metadata manipulation methods ----------------------------------------------

    /** Clear the Comments metadata container in memory. 
     */
    bool clearComments();

    /** Clear the Exif metadata container in memory. 
     */
    bool clearExif();

    /** Clear the Iptc metadata container in memory. 
     */
    bool clearIptc();

    /** Return the file path open with the current instance of interface. 
     */
    QString getFilePath() const;

    /** Return a Qt byte array copy of Comments container get from current image. 
        Comments are JFIF section of JPEG images. Look Exiv2 API for more information.
        Return a null Qt byte array if there is no Comments metadata in memory. 
     */ 
    QByteArray getComments() const;

    /** Return a Qt string object of Comments from current image decoded using 
        the 'detectEncodingAndDecode()' method. Return a null string if there is no 
        Comments metadata available. 
     */ 
    QString getCommentsDecoded() const;

    /** Return a Qt byte array copy of Exif container get from current image. 
        Return a null Qt byte array if there is no Exif metadata in memory. 
     */
    QByteArray getExif() const;

    /** Return a Qt byte array copy of Iptc container get from current image. 
        Set true 'addIrbHeader' parameter to add an Irb header to IPTC metadata. 
        Return a null Qt byte array if there is no Iptc metadata in memory. 
     */
    QByteArray  getIptc(bool addIrbHeader=false) const;

    /** Set the Comments data using a Qt byte array. Return true if Comments metadata
        have been changed in memory. 
     */
    bool setComments(const QByteArray& data);

    /** Set the Exif data using a Qt byte array. Return true if Exif metadata
        have been changed in memory. 
     */
    bool setExif(const QByteArray& data);

    /** Set the Iptc data using a Qt byte array. Return true if Iptc metadata
        have been changed in memory. 
     */
    bool setIptc(const QByteArray& data);

    //-- File access methods ----------------------------------------------

    /** Load all metadata (EXIF, IPTC and JFIF Comments) from a byte array. 
        Return true if metadata have been loaded successfully from image data.
     */
    bool load(const QByteArray& imgData);

    /** Load all metadata (EXIF, IPTC and JFIF Comments) from a picture (JPEG, RAW, TIFF, PNG, 
        DNG, etc...). Return true if metadata have been loaded successfully from file. 
     */
    virtual bool load(const QString& filePath);

    /** Save all metadata to a file. This one can be different than original picture to perform 
        transfert operation Return true if metadata have been saved into file. 
     */
    bool save(const QString& filePath);

    /** The same than save() method, but it apply on current image. Return true if metadata 
        have been saved into file. 
     */
    bool applyChanges();

    /** return true is the file metadata cannot be written by Exiv2. 
     */
    static bool isReadOnly(const QString& filePath);

    /** Return 'true' if Comments can be written in file.
     */
    static bool canWriteComment(const QString& filePath);

    /** Return 'true' if Exif can be written in file.
     */
    static bool canWriteExif(const QString& filePath);

    /** Return 'true' if Iptc can be written in file.
     */
    static bool canWriteIptc(const QString& filePath);

    //-- Metadata Image Information manipulation methods ----------------

    /** Set Program name and program version in Exif and Iptc Metadata. Return true if information
        have been changed in metadata. 
     */
    bool setImageProgramId(const QString& program, const QString& version);

    /** Return the size of image in pixels using Exif tags. Return a null dimmension if size cannot 
        be found. 
     */
    QSize getImageDimensions() const;

    /** Set the size of image in pixels in Exif tags. Return true if size have been changed 
        in metadata. 
     */
    bool setImageDimensions(const QSize& size, bool setProgramName=true);

    /** Return a QImage copy of Exif thumbnail image. Return a null image if thumbnail cannot 
        be found. The 'fixOrientation' parameter will rotate automatically the thumbnail if Exif 
        orientation tags information are attached with thumbnail. 
     */
    QImage getExifThumbnail(bool fixOrientation) const;

    /** Set the Exif Thumbnail image. The thumbnail image must have the right dimensions before. 
        Look Exif specification for details. Return true if thumbnail have been changed in metadata. 
     */
    bool setExifThumbnail(const QImage& thumb, bool setProgramName=true);

    /** Return the image orientation set in Exif metadata. The makernotes of image are also parsed to 
        get this information. See ImageOrientation values for details. 
     */
    KExiv2::ImageOrientation getImageOrientation() const;

    /** Set the Exif orientation tag of image. See ImageOrientation values for details 
        Return true if orientation have been changed in metadata. 
     */
    bool setImageOrientation(ImageOrientation orientation, bool setProgramName=true);

    /** Return the image color-space set in Exif metadata. The makernotes of image are also parsed to 
        get this information. See ImageColorWorkSpace values for details. 
     */
    KExiv2::ImageColorWorkSpace getImageColorWorkSpace() const;

    /** Set the Exif color-space tag of image. See ImageColorWorkSpace values for details 
        Return true if work-space have been changed in metadata. 
     */
    bool setImageColorWorkSpace(ImageColorWorkSpace workspace, bool setProgramName=true);

    /** Return the time stamp of image. Exif information are check in first, IPTC in second 
        if image don't have Exif information. If no time stamp is found, a null date is returned. 
     */
    QDateTime getImageDateTime() const;

    /** Set the Exif and Iptc time stamp. If 'setDateTimeDigitized' parameter is true, the 'Digitalized'
        time stamp is set, else only 'Created' time stamp is set. 
     */
    bool setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized = false, 
                          bool setProgramName=true);

    /** Return a QImage copy of Iptc preview image. Return a null image if preview cannot 
        be found. 
     */
    bool getImagePreview(QImage& preview) const;

    /** Set the Iptc preview image. The thumbnail image must have the right size before (64Kb max 
        with JPEG file, else 256Kb). Look Iptc specification for details. Return true if preview 
        have been changed in metadata. 
        Re-implemente this method if you want to use another image file format than JPEG to 
        save preview.
    */
    virtual bool setImagePreview(const QImage& preview, bool setProgramName=true);

    /** Return a strings list of Iptc keywords from image. Return an empty list if no keyword are set. */
    QStringList getImageKeywords() const;

    /** Set Iptc keywords using a list of strings defined by 'newKeywords' parameter. Use 'getImageKeywords()' 
        method to set 'oldKeywords' parameter with existing keywords from image. The method will compare 
        all new keywords with all old keywords to prevent duplicate entries in image. Return true if keywords
        have been changed in metadata. 
     */
    bool setImageKeywords(const QStringList& oldKeywords, const QStringList& newKeywords, 
                          bool setProgramName=true);

    /** Return a strings list of Iptc subjects from image. Return an empty list if no subject are set. */
    QStringList getImageSubjects() const;

    /** Set Iptc subjects using a list of strings defined by 'newSubjects' parameter. Use 'getImageSubjects()' 
        method to set 'oldSubjects' parameter with existing subjects from image. The method will compare 
        all new subjects with all old subjects to prevent duplicate entries in image. Return true if subjects
        have been changed in metadata. 
     */
    bool setImageSubjects(const QStringList& oldSubjects, const QStringList& newSubjects, 
                          bool setProgramName=true);

    /** Return a strings list of Iptc sub-categories from image. Return an empty list if no sub-category 
        are set. 
     */
    QStringList getImageSubCategories() const;

    /** Set Iptc sub-categories using a list of strings defined by 'newSubCategories' parameter. Use
        'getImageSubCategories()' method to set 'oldSubCategories' parameter with existing sub-categories
        from image. The method will compare all new sub-categories with all old sub-categories to prevent
        duplicate entries in image. Return true if sub-categories have been changed in metadata. 
     */
    bool setImageSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories, 
                               bool setProgramName=true);

    /** Return a QString copy of Exif user comments. Return a null string if user comments cannot 
        be found. 
     */
    QString getExifComment() const;

    /** Set the Exif user comments from image. Look Exif specification for more details about this tag. 
        Return true if Exif user comments have been changed in metadata. 
     */
    bool setExifComment(const QString& comment, bool setProgramName=true);

    /** Get all GPS location information set in image. Return true if all information can be found. 
     */
    bool getGPSInfo(double& altitude, double& latitude, double& longitude) const;

    /** Set all GPS location information into image. Return true if all information have been 
        changed in metadata. 
     */
    bool setGPSInfo(double altitude, double latitude, double longitude, bool setProgramName=true);

    /** Remove all Exif tags relevant of GPS location information. Return true if all tags have been 
        removed successfully in metadata. 
     */
    bool removeGPSInfo(bool setProgramName=true);

    //-- Metadata Tags manipulation methods ----------------------------------------

    /** Get an Exif tags content like a string. If 'escapeCR' parameter is true, the CR characters
        will be removed. If Exif tag cannot be found a null string is returned. 
     */
    QString getExifTagString(const char *exifTagName, bool escapeCR=true) const;

    /** Set an Exif tag content using a string. Return true if tag is set successfully. 
     */
    bool setExifTagString(const char *exifTagName, const QString& value, bool setProgramName=true);

    /** Get an Exif tags content like a long value. Return true if Exif tag be found.
     */
    bool getExifTagLong(const char* exifTagName, long &val) const;

    /** Set an Exif tag content using a long value. Return true if tag is set successfully. 
     */
    bool setExifTagLong(const char *exifTagName, long val, bool setProgramName=true);

    /** Get the 'component' index of an Exif tags content like a rational value. 
        'num' and 'den' are the numerator and the denominator of the rational value. 
        Return true if Exif tag be found. 
     */
    bool getExifTagRational(const char *exifTagName, long int &num, long int &den, int component=0) const;

    /** Set an Exif tags content using a rational value. 
        'num' and 'den' are the numerator and the denominator of the rational value. 
        Return true if tag is set successfully. 
     */
    bool setExifTagRational(const char *exifTagName, long int num, long int den, bool setProgramName=true);

    /** Get an Exif tags content like a bytes array. Return an empty bytes array if Exif 
        tag cannot be found. 
     */
    QByteArray getExifTagData(const char *exifTagName) const;

    /** Set an Exif tag content using a bytes array. Return true if tag is set successfully. 
     */
    bool setExifTagData(const char *exifTagName, const QByteArray& data, bool setProgramName=true);

    /** Get an Iptc tags content like a string. If 'escapeCR' parameter is true, the CR characters
        will be removed. If Iptc tag cannot be found a null string is returned. 
     */
    QString getIptcTagString(const char* iptcTagName, bool escapeCR=true) const;

    /** Set an Iptc tag content using a string. Return true if tag is set successfully. 
     */
    bool setIptcTagString(const char *iptcTagName, const QString& value, bool setProgramName=true);

    /** Get an Iptc tags content like a bytes array. Return an empty bytes array if Iptc 
        tag cannot be found. 
     */
    QByteArray getIptcTagData(const char *iptcTagName) const;

    /** Set an Iptc tag content using a bytes array. Return true if tag is set successfully. 
     */
    bool setIptcTagData(const char *iptcTagName, const QByteArray& data, bool setProgramName=true);

    /** Remove the Exif tag 'exifTagName' from Exif metadata. Return true if tag is 
        removed successfully. 
     */
    bool removeExifTag(const char *exifTagName, bool setProgramName=true);

    /** Remove the all instance of Iptc tags 'iptcTagName' from Iptc metadata. Return true if all 
        tags have been removed successfully. 
     */
    bool removeIptcTag(const char *iptcTagName, bool setProgramName=true);

    /** Return the Exif Tag title or a null string. 
     */
    QString getExifTagTitle(const char *exifTagName);

    /** Return the Exif Tag description or a null string. 
     */ 
    QString getExifTagDescription(const char *exifTagName);

    /** Return the Iptc Tag title or a null string. 
     */ 
    QString getIptcTagTitle(const char *iptcTagName);

    /** Return the Iptc Tag description or a null string. 
     */ 
    QString getIptcTagDescription(const char *iptcTagName);

    /** Return a map of Exif tags name/value found in metadata sorted by 
        Exif keys given by 'exifKeysFilter'. 

        'exifKeysFilter' is a QStringList of Exif keys. 
        For example, if you use the string list given below:

        "Iop"
        "Thumbnail"
        "Image"
        "Photo"

        ... this method will return a map of all Exif tags witch :

        - include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys 
          if 'inverSelection' is false.
        - not include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys 
          if 'inverSelection' is true.
     */ 
    KExiv2::MetaDataMap getExifTagsDataList(const QStringList &exifKeysFilter, bool invertSelection=false);

    /** Return a map of Iptc tags name/value found in metadata sorted by 
        Iptc keys given by 'iptcKeysFilter'. 

        'iptcKeysFilter' is a QStringList of Iptc keys. 
        For example, if you use the string list given below:

        "Envelope"
        "Application2"

        ... this method will return a map of all Iptc tags witch :

        - include "Envelope", or "Application2" in the Iptc tag keys 
          if 'inverSelection' is false.
        - not include "Envelope", or "Application2" in the Iptc tag keys 
          if 'inverSelection' is true.
     */ 
    KExiv2::MetaDataMap getIptcTagsDataList(const QStringList &iptcKeysFilter, bool invertSelection=false);

    //-- Advanced methods to convert and decode data -------------------------

    /** This method convert 'number' like a rational value, returned in 'numerator' and 
        'denominator' parameters. Set the precision using 'rounding' parameter. 
     */
    static void convertToRational(double number, long int* numerator, 
                                  long int* denominator, int rounding);

protected:

    /** Re-implemente this method to set automatically the Program Name and Program Version 
        information in Exif and Iptc metadata if 'on' argument is true. This method is called by all methods witch
        change tags in metadata. By default this method do nothing and return true.

        In digiKam this method is re-implementated like this:

        if (on)
        {
            QString version(digikam_version);
            QString software("digiKam");
            return setImageProgramId(software, version);
        }

        return true;
     */
    virtual bool setProgramId(bool on=true);

private:

    /** Internal class to store private members. Used to improve binary compatibility 
     */ 
    KExiv2Priv *d;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_H */
