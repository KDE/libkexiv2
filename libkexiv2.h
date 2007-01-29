/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 *          Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Date   : 2006-09-15
 * Description : Exiv2 library interface
 *
 * Copyright 2006-2007 by Gilles Caulier and Marcel Wiesweg
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LIB_KEXIV2_H
#define LIB_KEXIV2_H

// C++ includes.

#include <string>

// QT includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>
#include <qdatetime.h>

// Local includes.

#include "libkexiv2_export.h"

namespace Exiv2
{
    class DataBuf;
    class Exifdatum;
    class ExifData;
    class IptcData;
}

namespace KExiv2Library
{

class LibKExiv2Priv;

class LIBKEXIV2_EXPORT LibKExiv2
{

public:

    /** The image orientation value given by Exif Orientaion tag */
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

public:

    /** Standard constructor */
    LibKExiv2();

    /** Contructor to Load Metadata from image file */
    LibKExiv2(const QString& filePath);

    /** Standard destructor */
    ~LibKExiv2();

    //-- Metadata manipulation methods ----------------------------------------------

    /** Clear the Exif metadata container in memory */
    bool clearExif();

    /** Clear the Iptc metadata container in memory */
    bool clearIptc();

    /** Return the file path open with the current instance of interface */
    QString     getFilePath() const;

    /** Return a Qt byte array copy of Comments container get from current image. 
        Comments are JFIF section of JPEG images. Look Exiv2 API for more informations.
        Return a null Qt byte array if there is no Comments metadata in memory */ 
    QByteArray  getComments() const;

    /** Return a standard C++ string copy of Comments container get from current image.
        Return a null standard string if there is no Comments metadata in memory */
    std::string getCommentsString() const;

    /** Return a Qt byte array copy of Exif container get from current image. 
        Return a null Qt byte array if there is no Exif metadata in memory */
    QByteArray  getExif() const;

    /** Return a Qt byte array copy of Iptc container get from current image. 
        Set true 'addIrbHeader' parameter to add an Irb header to IPTC metadata. 
        Return a null Qt byte array if there is no Iptc metadata in memory */
    QByteArray  getIptc(bool addIrbHeader=false) const;

    /** Set the Comments data using a Qt byte array. Return true if Comments metadata
        have been changed in memory */
    bool setComments(const QByteArray& data);

    /** Set the Exif data using a Qt byte array. Return true if Exif metadata
        have been changed in memory */
    bool setExif(const QByteArray& data);

    /** Set the Iptc data using a Qt byte array. Return true if Iptc metadata
        have been changed in memory */
    bool setIptc(const QByteArray& data);

    /** Set the Exif data using an Exiv2 byte array. Return true if Exif metadata
        have been changed in memory */
    bool setExif(Exiv2::DataBuf const data);

    /** Set the Iptc data using an Exiv2 byte array. Return true if Iptc metadata
        have been changed in memory */
    bool setIptc(Exiv2::DataBuf const data);

    //-- File access method ----------------------------------------------

    /** Load all metadata (EXIF, IPTC and JFIF Comments) from a picture (JPEG, RAW, TIFF, PNG, 
        DNG, etc...). Return true if metadata have been loaded sucessfuly from file */
    bool load(const QString& filePath);

    /** Save all metadata to a file. This one can be different than original picture to perform 
        transfert operation Return true if metadata have been saved into file */
    bool save(const QString& filePath);

    /** The same than save() method, but it apply on current image. Return true if metadata 
        have been saved into file */
    bool applyChanges();

    /** return true is the file metadata cannot be written by Exiv2 */
    static bool isReadOnly(const QString& filePath);

    //-- Metadata Image Informations manipulation methods ----------------

    /** Set Program mane and program version in Exif and Iptc Metadata. Return true if informations
        have been changed in metadata */
    bool setImageProgramId(const QString& program, const QString& version);

    /** Return the size of image in pixels using Exif tags. Return a null dimmension if size cannot 
        be found. */
    QSize getImageDimensions();

    /** Set the size of image in pixels in Exif tags. Return true if size have been changed 
        in metadata */
    bool  setImageDimensions(const QSize& size);

    /** Return a QImage copy of Exif thumbnail image. Return a null image if thumbnail cannot 
        be found. The 'fixOrientation' parameter will rotate automaticly the thumbnail if Exif 
        orientation tags informations are attached with thumbnail */
    QImage getExifThumbnail(bool fixOrientation) const;

    /** Set the Exif Thumbnail image. The thumbnail image must have the right dimensions before. 
        Look Exif specification for details. Return true if thumbnail have been changed in metadata */
    bool   setExifThumbnail(const QImage& thumb);

    /** Return the image orientation set in Exif metadata. See ImageOrientation values for details */
    LibKExiv2::ImageOrientation getImageOrientation();

    /** Set the Exif orientation tag of image. See ImageOrientation values for details 
        Return true if orientation have been changed in metadata */
    bool setImageOrientation(ImageOrientation orientation);

    /** Return the time stamp of image. Exif informations are check in first, IPTC in second 
        if image don't have Exif information. If no time stamp is found, a null date is returned */
    QDateTime getImageDateTime() const;

    /** Set the Exif and Iptc time stamp. If 'setDateTimeDigitized' parameter is true, the 'Digitalized'
        time stamp is set, else only 'Created' time stamp is set. */
    bool setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized = false);

    /** Return a QImage copy of Iptc preview image. Return a null image if preview cannot 
        be found. */
    bool getImagePreview(QImage& preview);

    /** Set the Iptc preview image. The thumbnail image must have the right size before (64Kb max 
        with JPEG file, else 256Kb). Look Iptc specification for details. Return true if preview 
        have been changed in metadata */
    bool setImagePreview(const QImage& preview);

    QStringList getImageKeywords() const;
    bool setImageKeywords(const QStringList& oldKeywords, const QStringList& newKeywords);

    QStringList getImageSubjects() const;
    bool setImageSubjects(const QStringList& oldSubjects, const QStringList& newSubjects);

    QStringList getImageSubCategories() const;
    bool setImageSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories);

    QString getExifComment() const;
    bool    setExifComment(const QString& comment);

    bool getGPSInfo(double& altitude, double& latitude, double& longitude);
    bool setGPSInfo(double altitude, double latitude, double longitude);
    bool removeGPSInfo();

    //-- Metadata Tags manipulation methods ----------------------------------------

    QString getExifTagString(const char *exifTagName, bool escapeCR=true) const;
    bool    setExifTagString(const char *exifTagName, const QString& value);

    bool getExifTagLong(const char* exifTagName, long &val);
    bool setExifTagLong(const char *exifTagName, long val);

    bool getExifTagRational(const char *exifTagName, long int &num, long int &den, int component=0);
    bool setExifTagRational(const char *exifTagName, long int num, long int den);

    QByteArray getExifTagData(const char *exifTagName) const;
    
    QString getIptcTagString(const char* iptcTagName, bool escapeCR=true) const;
    bool    setIptcTagString(const char *iptcTagName, const QString& value);

    QByteArray getIptcTagData(const char *iptcTagName) const;

    bool removeExifTag(const char *exifTagName);
    bool removeIptcTag(const char *iptcTagName);

    //-- Advanced methods to convert and decode data -------------------------

    static QString convertCommentValue(const Exiv2::Exifdatum &comment);
    static QString detectEncodingAndDecode(const std::string &value);
    static void convertToRational(double number, long int* numerator, 
                                  long int* denominator, int rounding);

protected:

    std::string&     commentsMetaData();
    Exiv2::ExifData& exifMetaData();
    Exiv2::IptcData& iptcMetaData();

private:

    LibKExiv2Priv *d;
};

}  // NameSpace KExiv2Library

#endif /* LIB_KEXIV2_H */
