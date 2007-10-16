/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * Exiv2: http://www.exiv2.org
 * Exif : http://www.exif.org/Exif2-2.PDF
 * Iptc : http://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
 * Xmp  : http://www.adobe.com/devnet/xmp/pdfs/xmp_specification.pdf
 *        http://www.iptc.org/std/Iptc4xmpCore/1.0/specification/Iptc4xmpCore_1.0-spec-XMPSchema_8.pdf
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

// C++ includes.

#include <string>

// QT includes.

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QtDebug>

// Local includes.

#include "libkexiv2_export.h"

namespace Exiv2
{
    class DataBuf;
    class Exifdatum;
    class ExifData;
    class IptcData;
    class XmpData;
    class Error;
}

namespace KExiv2Iface
{

class KExiv2Priv;

class KEXIV2_EXPORT KExiv2
{

public:

    /** The image color workspace values given by Exif metadata. */
    enum ImageColorWorkSpace
    {
        WORKSPACE_UNSPECIFIED  = 0,
        WORKSPACE_SRGB         = 1,
        WORKSPACE_ADOBERGB     = 2,
        WORKSPACE_UNCALIBRATED = 65535
    };

    /** The image orientation values given by Exif metadata. */
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
        to store Tags Key and Tags Value. */
    typedef QMap<QString, QString>  MetaDataMap;

public:

    /** Standard constructor. */
    KExiv2();

    /** Copy constructor. */
    KExiv2(const KExiv2& metadata);

    /** Contructor to Load Metadata from image file. */
    KExiv2(const QString& filePath);

    /** Standard destructor */
    virtual ~KExiv2();

    /** Create a deep copy of container */
    KExiv2& operator=(const KExiv2& metadata);

public:

    //-- Statics methods ----------------------------------------------

    /** Return true if library can handle Xmp metadata */
    static bool supportXmp();

    /** Return a string version of Exiv2 release in format "major.minor.patch" */ 
    static QString Exiv2Version();

    /** Generic method to print the Exiv2 C++ Exception error message from 'e'.
        'msg' string is printed just before like debug header.
    */
    static void printExiv2ExceptionError(const QString& msg, Exiv2::Error& e);

    /** return true if metadata from file cannot be written by Exiv2. */
    static bool isReadOnly(const QString& filePath);

    //-- General methods ----------------------------------------------

    /** Load all metadata (Exif, Iptc, Xmp, and JFIF Comments) from a picture (JPEG, RAW, TIFF, PNG, 
        DNG, etc...). Return true if metadata have been loaded successfully from file. */
    virtual bool load(const QString& filePath) const;

    /** Save all metadata to a file. This one can be different than original picture to perform 
        transfert operation Return true if metadata have been saved into file. */
    bool save(const QString& filePath) const;

    /** The same than save() method, but it apply on current image. Return true if metadata 
        have been saved into file. */
    bool applyChanges() const;

    /** Return 'true' if metadata container in memory as no Comments, Exif, Iptc, and Xmp. */
    bool isEmpty() const;

    /** Set the file path of current image. */
    void setFilePath(const QString& path);

    /** Return the file path of current image. */
    QString getFilePath() const;

    //-- Metadata Image Information manipulation methods ----------------

    /** Set Program name and program version in Exif and Iptc Metadata. Return true if information
        have been changed in metadata. */
    bool setImageProgramId(const QString& program, const QString& version) const;

    /** Return the size of image in pixels using Exif tags. Return a null dimmension if size cannot 
        be found. */
    QSize getImageDimensions() const;

    /** Set the size of image in pixels in Exif tags. Return true if size have been changed 
        in metadata. */
    bool setImageDimensions(const QSize& size, bool setProgramName=true) const;

    /** Return the image orientation set in Exif metadata. The makernotes of image are also parsed to 
        get this information. See ImageOrientation values for details. */
    KExiv2::ImageOrientation getImageOrientation() const;

    /** Set the Exif orientation tag of image. See ImageOrientation values for details 
        Return true if orientation have been changed in metadata. */
    bool setImageOrientation(ImageOrientation orientation, bool setProgramName=true) const;

    /** Return the image color-space set in Exif metadata. The makernotes of image are also parsed to 
        get this information. See ImageColorWorkSpace values for details. */
    KExiv2::ImageColorWorkSpace getImageColorWorkSpace() const;

    /** Set the Exif color-space tag of image. See ImageColorWorkSpace values for details 
        Return true if work-space have been changed in metadata. */
    bool setImageColorWorkSpace(ImageColorWorkSpace workspace, bool setProgramName=true) const;

    /** Return the time stamp of image. Exif information are check in first, IPTC in second 
        if image don't have Exif information. If no time stamp is found, a null date is returned. */
    QDateTime getImageDateTime() const;

    /** Set the Exif and Iptc time stamp. If 'setDateTimeDigitized' parameter is true, the 'Digitalized'
        time stamp is set, else only 'Created' time stamp is set. */
    bool setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized = false, 
                          bool setProgramName=true) const;

    /** Return a QImage copy of Iptc preview image. Return a null image if preview cannot 
        be found. */
    bool getImagePreview(QImage& preview) const;

    /** Set the Iptc preview image. The thumbnail image must have the right size before (64Kb max 
        with JPEG file, else 256Kb). Look Iptc specification for details. Return true if preview 
        have been changed in metadata. 
        Re-implemente this method if you want to use another image file format than JPEG to 
        save preview.
    */
    virtual bool setImagePreview(const QImage& preview, bool setProgramName=true) const;

    //-- Comments manipulation methods --------------------------------

    /** Return 'true' if metadata container in memory as Comments. */
    bool hasComments() const;

    /** Clear the Comments metadata container in memory. */
    bool clearComments() const;

    /** Return a Qt byte array copy of Comments container get from current image. 
        Comments are JFIF section of JPEG images. Look Exiv2 API for more information.
        Return a null Qt byte array if there is no Comments metadata in memory. */ 
    QByteArray getComments() const;

    /** Return a Qt string object of Comments from current image decoded using 
        the 'detectEncodingAndDecode()' method. Return a null string if there is no 
        Comments metadata available. */ 
    QString getCommentsDecoded() const;

    /** Set the Comments data using a Qt byte array. Return true if Comments metadata
        have been changed in memory. */
    bool setComments(const QByteArray& data) const;

    /** Wrapper method to convert a Comments content to a QString. */ 
    static QString convertCommentValue(const Exiv2::Exifdatum &comment);

    /** Charset autodetection to convert a string to a QString. */ 
    static QString detectEncodingAndDecode(const std::string &value);

    /** Language Alternative autodetection. Return a QString without language alternative 
        header. Header is saved into 'lang'. If no language alternative is founf, value is returned 
        as well and 'lang' is set to a null string. */ 
    static QString detectLanguageAlt(const QString &value, QString& lang);

    //-- Exif manipulation methods --------------------------------

    /** Return 'true' if metadata container in memory as Exif. */
    bool hasExif() const;

    /** Clear the Exif metadata container in memory. */
    bool clearExif() const;

    /** Return a Qt byte array copy of Exif container get from current image. 
        Set true 'addExifHeader' parameter to add an Exif header to Exif metadata. 
        Return a null Qt byte array if there is no Exif metadata in memory. */
    QByteArray getExif(bool addExifHeader=false) const;

    /** Set the Exif data using a Qt byte array. Return true if Exif metadata
        have been changed in memory. */
    bool setExif(const QByteArray& data) const;

    /** Return a QImage copy of Exif thumbnail image. Return a null image if thumbnail cannot 
        be found. The 'fixOrientation' parameter will rotate automatically the thumbnail if Exif 
        orientation tags information are attached with thumbnail. */
    QImage getExifThumbnail(bool fixOrientation) const;

    /** Set the Exif Thumbnail image. The thumbnail image must have the right dimensions before. 
        Look Exif specification for details. Return true if thumbnail have been changed in metadata. */
    bool setExifThumbnail(const QImage& thumb, bool setProgramName=true) const;
    
    /** Return a QString copy of Exif user comments. Return a null string if user comments cannot 
        be found. */
    QString getExifComment() const;

    /** Set the Exif user comments from image. Look Exif specification for more details about this tag. 
        Return true if Exif user comments have been changed in metadata. */
    bool setExifComment(const QString& comment, bool setProgramName=true) const;

    /** Get an Exif tags content like a string. If 'escapeCR' parameter is true, the CR characters
        will be removed. If Exif tag cannot be found a null string is returned. */
    QString getExifTagString(const char *exifTagName, bool escapeCR=true) const;

    /** Set an Exif tag content using a string. Return true if tag is set successfully. */
    bool setExifTagString(const char *exifTagName, const QString& value, bool setProgramName=true) const;

    /** Get an Exif tag content like a long value. Return true if Exif tag be found. */
    bool getExifTagLong(const char* exifTagName, long &val) const;

    /** Set an Exif tag content using a long value. Return true if tag is set successfully. */
    bool setExifTagLong(const char *exifTagName, long val, bool setProgramName=true) const;

    /** Get the 'component' index of an Exif tags content like a rational value. 
        'num' and 'den' are the numerator and the denominator of the rational value. 
        Return true if Exif tag be found. */
    bool getExifTagRational(const char *exifTagName, long int &num, long int &den, int component=0) const;

    /** Set an Exif tag content using a rational value. 
        'num' and 'den' are the numerator and the denominator of the rational value. 
        Return true if tag is set successfully. */
    bool setExifTagRational(const char *exifTagName, long int num, long int den, bool setProgramName=true) const;

    /** Get an Exif tag content like a bytes array. Return an empty bytes array if Exif 
        tag cannot be found. */
    QByteArray getExifTagData(const char *exifTagName) const;

    /** Set an Exif tag content using a bytes array. Return true if tag is set successfully. */
    bool setExifTagData(const char *exifTagName, const QByteArray& data, bool setProgramName=true) const;

    /** Get an Exif tags content as a QVariant. Returns a null QVariant if the Exif
        tag cannot be found.
        For string and integer values the matching QVariant types will be used,
        for date and time values QVariant::DateTime.
        Rationals will be returned as QVariant::List with two integer QVariants (numerator, denominator)
        if rationalAsListOfInts is true, as double if rationalAsListOfInts is false.
     */
    QVariant getExifTagVariant(const char *exifTagName, bool rationalAsListOfInts = true, bool escapeCR=true) const;

    /** Set an Exif tag content using a QVariant. Returns true if tag is set successfully.
        All types described for the above method are supported.
        Calling with a QVariant of type ByteArray is equivalent to calling setExifTagData.
        For the meaning of rationalWantSmallDenominator, see the documentation of the convertToRational methods.
     */
    bool setExifTagVariant(const char *exifTagName, const QVariant& data,
                           bool rationalWantSmallDenominator=true, bool setProgramName=true) const;

    /** Remove the Exif tag 'exifTagName' from Exif metadata. Return true if tag is 
        removed successfully. */
    bool removeExifTag(const char *exifTagName, bool setProgramName=true) const;

    /** Return the Exif Tag title or a null string. */ 
    static QString getExifTagTitle(const char *exifTagName);

    /** Return the Exif Tag description or a null string. */ 
    static QString getExifTagDescription(const char *exifTagName);

    /** TODO document me */
    static QString createExifTagStringFromValue(const char *exifTagName, const QVariant &val, bool escapeCR=true);

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
    KExiv2::MetaDataMap getExifTagsDataList(const QStringList &exifKeysFilter, bool invertSelection=false) const;

    //-- Iptc manipulation methods --------------------------------

    /** Return 'true' if metadata container in memory as Iptc. */
    bool hasIptc() const;

    /** Clear the Iptc metadata container in memory. */
    bool clearIptc() const;

    /** Return a Qt byte array copy of Iptc container get from current image. 
        Set true 'addIrbHeader' parameter to add an Irb header to Iptc metadata. 
        Return a null Qt byte array if there is no Iptc metadata in memory. */
    QByteArray  getIptc(bool addIrbHeader=false) const;

    /** Set the Iptc data using a Qt byte array. Return true if Iptc metadata
        have been changed in memory. */
    bool setIptc(const QByteArray& data) const;

    /** Get an Iptc tag content like a string. If 'escapeCR' parameter is true, the CR characters
        will be removed. If Iptc tag cannot be found a null string is returned. */
    QString getIptcTagString(const char* iptcTagName, bool escapeCR=true) const;

    /** Set an Iptc tag content using a string. Return true if tag is set successfully. */
    bool setIptcTagString(const char *iptcTagName, const QString& value, bool setProgramName=true) const;

    /** Return a strings list of a multiple Iptc tags from image. Return an empty list if no tag is found. */
    QStringList getIptcTagsStringList(const char* iptcTagName, bool escapeCR=true) const;

    /** Set multiple Iptc tags contents using a strings list. 'maxSize' is the max characters size 
        of one entry. Return true if all tags have been set successfully. */
    bool setIptcTagsStringList(const char* iptcTagName, int maxSize,
                               const QStringList& oldValues, const QStringList& newValues, 
                               bool setProgramName=true) const;

    /** Get an Iptc tag content like a bytes array. Return an empty bytes array if Iptc 
        tag cannot be found. */
    QByteArray getIptcTagData(const char *iptcTagName) const;

    /** Set an Iptc tag content using a bytes array. Return true if tag is set successfully. */
    bool setIptcTagData(const char *iptcTagName, const QByteArray& data, bool setProgramName=true) const;

    /** Remove the all instance of Iptc tags 'iptcTagName' from Iptc metadata. Return true if all 
        tags have been removed successfully. */
    bool removeIptcTag(const char *iptcTagName, bool setProgramName=true) const;

    /** Return the Iptc Tag title or a null string. */ 
    static QString getIptcTagTitle(const char *iptcTagName);

    /** Return the Iptc Tag description or a null string. */ 
    static QString getIptcTagDescription(const char *iptcTagName);

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
    KExiv2::MetaDataMap getIptcTagsDataList(const QStringList &iptcKeysFilter, bool invertSelection=false) const;

    /** Return a strings list of Iptc keywords from image. Return an empty list if no keyword are set. */
    QStringList getIptcKeywords() const;

    /** Set Iptc keywords using a list of strings defined by 'newKeywords' parameter. Use 'getImageKeywords()' 
        method to set 'oldKeywords' parameter with existing keywords from image. The method will compare 
        all new keywords with all old keywords to prevent duplicate entries in image. Return true if keywords
        have been changed in metadata. */
    bool setIptcKeywords(const QStringList& oldKeywords, const QStringList& newKeywords, 
                         bool setProgramName=true) const;

    /** Return a strings list of Iptc subjects from image. Return an empty list if no subject are set. */
    QStringList getIptcSubjects() const;

    /** Set Iptc subjects using a list of strings defined by 'newSubjects' parameter. Use 'getImageSubjects()' 
        method to set 'oldSubjects' parameter with existing subjects from image. The method will compare 
        all new subjects with all old subjects to prevent duplicate entries in image. Return true if subjects
        have been changed in metadata. */
    bool setIptcSubjects(const QStringList& oldSubjects, const QStringList& newSubjects, 
                         bool setProgramName=true) const;

    /** Return a strings list of Iptc sub-categories from image. Return an empty list if no sub-category 
        are set. */
    QStringList getIptcSubCategories() const;

    /** Set Iptc sub-categories using a list of strings defined by 'newSubCategories' parameter. Use
        'getImageSubCategories()' method to set 'oldSubCategories' parameter with existing sub-categories
        from image. The method will compare all new sub-categories with all old sub-categories to prevent
        duplicate entries in image. Return true if sub-categories have been changed in metadata. */
    bool setIptcSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories, 
                              bool setProgramName=true) const;

    //-- Xmp manipulation methods --------------------------------

    /** Return 'true' if metadata container in memory as Xmp. */
    bool hasXmp() const;

    /** Clear the Xmp metadata container in memory. */
    bool clearXmp() const;

    /** Return a Qt byte array copy of XMp container get from current image. 
        Return a null Qt byte array if there is no Xmp metadata in memory. */
    QByteArray getXmp() const;

    /** Set the Xmp data using a Qt byte array. Return true if Xmp metadata
        have been changed in memory. */
    bool setXmp(const QByteArray& data) const;

    /** Get a Xmp tag content like a string. If 'escapeCR' parameter is true, the CR characters
        will be removed. If Xmp tag cannot be found a null string is returned. */
    QString getXmpTagString(const char* xmpTagName, bool escapeCR=true) const;

    /** Set a Xmp tag content using a string. Return true if tag is set successfully. */
    bool setXmpTagString(const char *xmpTagName, const QString& value, bool setProgramName=true) const;

    /** Return the Xmp Tag title or a null string. */ 
    static QString getXmpTagTitle(const char *xmpTagName);

    /** Return the Xmp Tag description or a null string. */ 
    static QString getXmpTagDescription(const char *xmpTagName);

    /** Return a map of Xmp tags name/value found in metadata sorted by 
        Xmp keys given by 'xmpKeysFilter'.

        'xmpKeysFilter' is a QStringList of Xmp keys.
        For example, if you use the string list given below:

        "dc"           // Dubling Core schema.
        "xmp"          // Standard Xmp schema.

        ... this method will return a map of all Xmp tags witch :

        - include "dc", or "xmp" in the Xmp tag keys 
          if 'inverSelection' is false.
        - not include "dc", or "xmp" in the Xmp tag keys 
          if 'inverSelection' is true.
        */ 
    KExiv2::MetaDataMap getXmpTagsDataList(const QStringList &xmpKeysFilter, bool invertSelection=false) const;

    /** Get a Xmp tag content like a string set with an alternative language 
        header 'langAlt' (like "fr-FR" for French).
        If 'escapeCR' parameter is true, the CR characters will be removed. 
        If Xmp tag cannot be found a null string is returned. */
    QString getXmpTagStringLangAlt(const char* xmpTagName, const QString& langAlt, bool escapeCR) const;

    /** Set a Xmp tag content using a string with an alternative language header. 'langAlt' contain the 
        language alternative information (like "fr-FR" for French - RFC3066 notation) or is null to 
        set alternative language to default settings ("x-default").
        Return true if tag is set successfully. */
    bool setXmpTagStringLangAlt(const char *xmpTagName, const QString& value, 
                                const QString& langAlt, bool setProgramName=true) const;

    /** Get a Xmp tag content like a sequence of strings. If 'escapeCR' parameter is true, the CR characters
        will be removed from strings. If Xmp tag cannot be found a null string list is returned. */
    QStringList getXmpTagStringSeq(const char* xmpTagName, bool escapeCR=true) const;

    /** Set a Xmp tag content using the sequence of strings 'seq'.
        Return true if tag is set successfully. */
    bool setXmpTagStringSeq(const char *xmpTagName, const QStringList& seq,
                            bool setProgramName=true) const;

    /** Get a Xmp tag content like a bag of strings. If 'escapeCR' parameter is true, the CR characters
        will be removed from strings. If Xmp tag cannot be found a null string list is returned. */
    QStringList getXmpTagStringBag(const char* xmpTagName, bool escapeCR) const;

    /** Set a Xmp tag content using the bag of strings 'bag'.
        Return true if tag is set successfully. */
    bool setXmpTagStringBag(const char *xmpTagName, const QStringList& bag,
                            bool setProgramName=true) const;

    /** Return a strings list of Xmp keywords from image. Return an empty list if no keyword are set. */
    QStringList getXmpKeywords() const;

    /** Set Xmp keywords using a list of strings defined by 'newKeywords' parameter. 
        The existing keywords from image are preserved. The method will compare 
        all new keywords with all already existing keywords to prevent duplicate entries in image. 
        Return true if keywords have been changed in metadata. */
    bool setXmpKeywords(const QStringList& newKeywords, bool setProgramName=true) const;

    /** Return a strings list of Xmp sub-categories from image. Return an empty list if no sub-category 
        are set. */
    QStringList getXmpSubCategories() const;

    /** Set Xmp sub-categories using a list of strings defined by 'newSubCategories' parameter. 
        The existing sub-categories from image are preserved. The method will compare 
        all new sub-categories with all already existing sub-categories to prevent duplicate entries in image. 
        Return true if sub-categories have been changed in metadata. */
    bool setXmpSubCategories(const QStringList& newSubCategories, bool setProgramName=true) const;

    /** Register a namespace which Exiv2 doesn't know yet. This is only needed
        when new Xmp properties are added manually. 'uri' is the namespace url and prefix the 
        string used to construct new Xmp key (ex. "Xmp.digiKam.tagList").
        NOTE: If the Xmp metadata is read from an image, namespaces are decoded and registered 
        by Exiv2 at the same time. */
    bool registerXmpNameSpace(const QString& uri, const QString& prefix) const;

    /** Remove the Xmp tag 'xmpTagName' from Xmp metadata. Return true if tag is 
        removed successfully. */
    bool removeXmpTag(const char *xmpTagName, bool setProgramName=true) const;

    //-- GPS manipulation methods --------------------------------

    /** Get all GPS location information set in image. Return true if all information can be found. */
    bool getGPSInfo(double& altitude, double& latitude, double& longitude) const;

    /** Set all GPS location information into image. Return true if all information have been 
        changed in metadata. */
    bool setGPSInfo(double altitude, double latitude, double longitude, bool setProgramName=true) const;

    /** Remove all Exif tags relevant of GPS location information. Return true if all tags have been 
        removed successfully in metadata. */
    bool removeGPSInfo(bool setProgramName=true) const;

    /** This method converts 'number' to a rational value, returned in the 'numerator' and
        'denominator' parameters. Set the precision using 'rounding' parameter.
        Use this method if you want to retrieve a most exact rational for a number
        without further properties, without any requirements to the denominator.
     */
    static void convertToRational(double number, long int* numerator,
                                  long int* denominator, int rounding);
    /** This method convert a'number' to a rational value, returned in 'numerator' and
        'denominator' parameters.
        This method will be able to retrieve a rational number from a double - if you
        constructed your double with 1.0 / 4786.0, this method will retrieve 1 / 4786.
        If your number is not expected to be rational, use the method above which is just as
        exact with rounding = 4 and more exact with rounding > 4.
     */
    static void convertToRationalSmallDenominator(double number, long int* numerator,
                                                  long int* denominator);

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
    virtual bool setProgramId(bool on=true) const;

private:

    /** Return a standard C++ string copy of Comments container get from current image.
        Return a null standard string if there is no Comments metadata in memory. */
    std::string commentsMetaData() const;

    /** Return a reference to Exif metadata object from memory. */
    Exiv2::ExifData exifMetaData() const;

    /** Return a reference to Iptc metadata object from memory. */
    Exiv2::IptcData iptcMetaData() const;

    /** Return a reference to Xmp metadata object from memory. */
    Exiv2::XmpData xmpMetaData() const;

    /** Set the Comments data using an string object. Return true if comments metadata
        have been changed in memory. */
    void setComments(std::string comments);

    /** Set the Exif data using an Exiv2 byte array. Return true if Exif metadata
        have been changed in memory. */
    void setExif(Exiv2::ExifData data);

    /** Set the Iptc data using an Exiv2 byte array. Return true if Iptc metadata
        have been changed in memory. */
    void setIptc(Exiv2::IptcData data);

    /** Set the Xmp data using an Exiv2 byte array. Return true if Xmp metadata
        have been changed in memory. */
    void setXmp(Exiv2::XmpData data);

private:

    /** Internal class to store private members. Used to improve binary compatibility */ 
    KExiv2Priv *d;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_H */
