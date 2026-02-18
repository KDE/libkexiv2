/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2013 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KEXIV2_H
#define KEXIV2_H

// Std

#include <memory>

// QT includes

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QSharedDataPointer>
#include <QStringList>
#include <QVariant>
#include <QUrl>
#include <QImage>

// Local includes

#include "libkexiv2_export.h"
#include "kexiv2data.h"

/*!
 * \brief  Exiv2 library interface
 *
 * \list
 * \li Exiv2: https://www.exiv2.org
 * \li Exif : https://www.exif.org/Exif2-2.PDF
 * \li IPTC : https://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
 * \li XMP  : https://www.adobe.com/devnet/xmp.html
 * \li IPTC : https://www.iptc.org/std/Iptc4xmpCore/1.0/specification/Iptc4xmpCore_1.0-spec-XMPSchema_8.pdf
 * \li Paper: http://www.metadataworkinggroup.com/pdf/mwg_guidance.pdf
 * \endlist
 */
namespace KExiv2Iface
{

/*!
 * \class KExiv2Iface::KExiv2
 * \inmodule KExiv2
 * \inheaderfile KExiv2/KExiv2
 */
class LIBKEXIV2_EXPORT KExiv2
{

public:

    /*! The image metadata writing mode, between image file metadata and XMP sidecar file, depending on the context.
     *
     * \value WRITETOIMAGEONLY
     *        Write metadata to image file only.
     * \value WRITETOSIDECARONLY
     *        Write metadata to sidecar file only.
     * \value WRITETOSIDECARANDIMAGE
     *        Write metadata to image and sidecar files.
     * \value WRITETOSIDECARONLY4READONLYFILES
     *        Write metadata to sidecar file only for read only images
     *        such as RAW files for example.
     *
     * \sa metadataWritingMode()
     */
    enum MetadataWritingMode
    {
        WRITETOIMAGEONLY                 = 0,
        WRITETOSIDECARONLY               = 1,
        WRITETOSIDECARANDIMAGE           = 2,
        WRITETOSIDECARONLY4READONLYFILES = 3
    };

    /*! The image color workspace values given by Exif metadata.
     * \value WORKSPACE_UNSPECIFIED
     * \value WORKSPACE_SRGB
     * \value WORKSPACE_ADOBERGB
     * \value WORKSPACE_UNCALIBRATED
     */
    enum ImageColorWorkSpace
    {
        WORKSPACE_UNSPECIFIED  = 0,
        WORKSPACE_SRGB         = 1,
        WORKSPACE_ADOBERGB     = 2,
        WORKSPACE_UNCALIBRATED = 65535
    };

    /*! The image orientation values given by Exif metadata.
     * \value ORIENTATION_UNSPECIFIED
     * \value ORIENTATION_NORMAL
     * \value ORIENTATION_HFLIP
     * \value ORIENTATION_ROT_180
     * \value ORIENTATION_VFLIP
     * \value ORIENTATION_ROT_90_HFLIP
     * \value ORIENTATION_ROT_90
     * \value ORIENTATION_ROT_90_VFLIP
     * \value ORIENTATION_ROT_270
     */
    enum ImageOrientation
    {
        ORIENTATION_UNSPECIFIED  = 0,
        ORIENTATION_FIRST_VALUE  = ORIENTATION_UNSPECIFIED,
        ORIENTATION_NORMAL       = 1,
        ORIENTATION_HFLIP        = 2,
        ORIENTATION_ROT_180      = 3,
        ORIENTATION_VFLIP        = 4,
        ORIENTATION_ROT_90_HFLIP = 5,
        ORIENTATION_ROT_90       = 6,
        ORIENTATION_ROT_90_VFLIP = 7,
        ORIENTATION_ROT_270      = 8,
        ORIENTATION_LAST_VALUE  = ORIENTATION_ROT_270
    };

    /*!
     * Xmp tag types, used by setXmpTag, only first three types are used.
     * \value NormalTag
     * \value ArrayBagTag
     * \value StructureTag
     * \value ArrayLangTag
     * \value ArraySeqTag
     */
    enum XmpTagType
    {
        NormalTag               = 0,
        ArrayBagTag             = 1,
        StructureTag            = 2,
        ArrayLangTag            = 3,
        ArraySeqTag             = 4
    };

    /*! A map used to store Tags Key and Tags Value.
     */
    typedef QMap<QString, QString> MetaDataMap;

    /*! A map used to store a list of Alternative Language values.
     *
     *  The map key is the language code following RFC3066 notation
     *  (like "fr-FR" for French), and the map value the text.
     */
    typedef QMap<QString, QString> AltLangMap;

    /*! A map used to store Tags Key and a list of Tags properties:
     *  \list
     *  \li name
     *  \li title
     *  \li description
     *  \endlist
     */
    typedef QMap<QString, QStringList> TagsMap;

public:

    /*! Standard constructor.
     */
    KExiv2();

    /*! Copy constructor.
     */
    KExiv2(const KExiv2& metadata);

    /*! Constructor to load from parsed \a data.
     */
    KExiv2(const KExiv2Data& data);

    /*! Contructor to load metadata from an image in \a filePath.
     */
    KExiv2(const QString& filePath);

    /*! Standard destructor.
     */
    virtual ~KExiv2();

    /*! Create a copy of container.
     */
    KExiv2& operator=(const KExiv2& metadata);

public:

    //-----------------------------------------------------------------
    /// @name Static methods
    //@{

    /*! Returns \c true if Exiv2 library initialization is done properly.
     *
     *  This method must be called before using libkexiv2 with multithreading,
     *  but is called automatically on first construction of KExiv2.
     *
     *  Call cleanupExiv2() to clean things up later if needed.
     */
    static bool initializeExiv2();

    /*! Returns \c true if Exiv2 library memory allocations are cleaned properly.
     *
     *  This method must be called after using libkexiv2 with multithreading.
     *
     *  It cleans up memory used by Adobe XMP SDK.
     *
     *  See B.K.O #166424 for details.
     */
    static bool cleanupExiv2();

    /*! Returns \c true if the library can handle XMP metadata. */
    static bool supportXmp();

    /*! Returns \c true if the library can write metadata to \a typeMime file format. */
    static bool supportMetadataWritting(const QString& typeMime);

    /*! Returns a string version of Exiv2 release in format "major.minor.patch".
     */
    static QString Exiv2Version();

    /*! Returns a string version of libkexiv2 release.
     */
    static QString version();

    /*! Returns the XMP Sidecar file \a path for an image in the given file \a path.
     *
     *  If the image file path does not include a file name or is empty, this function returns a null string.
     */
    static QString sidecarFilePathForFile(const QString& path);

    /*! Like sidecarFilePathForFile(), but it works for local file \a path.
     */
    static QString sidecarPath(const QString& path);

    /*! Like sidecarFilePathForFile(), but it works for a remote \a url.
     */
    static QUrl sidecarUrl(const QUrl& url);

    /*! Gives a file url for a local \a path.
     */
    static QUrl sidecarUrl(const QString& path);

    /*! Performs a QFileInfo based check if the file in the given file \a path has a sidecar.
     */
    static bool hasSidecar(const QString& path);

    //@}

    //-----------------------------------------------------------------
    /// @name General methods
    //@{

    /*!
     */
    KExiv2Data data() const;
    /*!
     */
    void setData(const KExiv2Data& data);

    /*! Load all metadata (Exif, IPTC, XMP, and JFIF Comments) from a byte array.
     *
     *  Returns \c true if the metadata has been loaded successfully from \a imgData.
     */
    bool loadFromData(const QByteArray& imgData) const;

    /*! Load all metadata (Exif, IPTC, XMP, and JFIF Comments) from a picture (JPEG, RAW, TIFF, PNG,
     *  DNG, etc...).
     *
     * Returns \c true if the metadata has been loaded successfully from file in \a filePath.
     */
    virtual bool load(const QString& filePath) const;

    /*! Saves all metadata to a file in \a filePath.
     *
     *  This one can be different from the original picture to perform
     *  a transfer operation.
     *
     *  Returns \c true if the metadata has been saved to the file.
     */
    bool save(const QString& filePath) const;

    /*! The same as save(), but it applies to the current image.
     *
     * Returns \c true if the metadata has been saved into file.
     */
    bool applyChanges() const;

    /*! Returns \c true if the metadata container in memory has no Comments, Exif, IPTC, and XMP.
     */
    bool isEmpty() const;

    /*! Sets the file \a path of the current image.
     */
    void setFilePath(const QString& path);

    /*! Returns the file path of the current image.
     */
    QString getFilePath() const;

    /*! Returns the pixel size of the current image.
     *
     *  This information is read from the file, not from the metadata.
     *
     *  The returned QSize is valid if the KExiv2 object was \e constructed
     *  by reading a file or image data; the information is not available when the object
     *  was created from KExiv2Data.
     *
     *  Note that in the Exif or XMP metadata, there may be fields describing the image size.
     *  These fields are not accessed by this method.
     *
     *  When replacing the metadata with setData(), the metadata may change; this information
     *  always keeps referring to the file it was initially read from.
     */
    QSize getPixelSize() const;

    /*! Returns the mime type of this image.
     *
     *  The information is read from the file;
     *  see getPixelSize() to know when it is available.
     */
    QString getMimeType() const;

    /*! Enables or disables writing metadata operations to RAW tiff based files.
     *
     *  Requires Exiv2 0.18.
     *
     *  By default RAW files are untouched.
     */
    void setWriteRawFiles(const bool on);

    /*! Returns \c true if writing metadata operations on RAW tiff based files is enabled.
     *
     *  Requires Exiv2 0.18.
     */
    bool writeRawFiles() const;

    /*! Enables or disables using an XMP sidecar for reading metadata.
     */
    void setUseXMPSidecar4Reading(const bool on);

    /*! Returns \c true if using an XMP sidecar for reading metadata is enabled.
     */
    bool useXMPSidecar4Reading() const;

    /*! Sets the metadata writing \a mode.
     * \sa MetadataWritingMode, metadataWritingMode()
     */
    void setMetadataWritingMode(const int mode);

    /*! Returns the metadata writing mode.
     * \sa MetadataWritingMode, setMetadataWritingMode()
     */
    int metadataWritingMode() const;

    /*! Enables or disables file timestamp updating when metadata are saved.
     *
     *  By default files timestamp are untouched.
     */
    void setUpdateFileTimeStamp(bool on);

    /*! Returns \c true if file timestamp is updated when metadata are saved.
     */
    bool updateFileTimeStamp() const;

    //@}

    //-------------------------------------------------------------------
    /// @name Metadata image information manipulation methods
    //@{

    /*! Sets \a program name and program \a version in Exif and IPTC Metadata.
     *
     *  Returns \c true if information has been changed in the metadata.
     */
    bool setImageProgramId(const QString& program, const QString& version) const;

    /*! Returns the size of the image in pixels using Exif tags.
     *
     *  Returns a null dimension if size cannot be found.
     */
    QSize getImageDimensions() const;

    /*! Sets the \a size of image in pixels using Exif tags.
     *
     *  Returns \c true if the size has been changed in the metadata.
     */
    bool setImageDimensions(const QSize& size, bool setProgramName=true) const;

    /*! Returns the image orientation set in Exif metadata.
     *
     * The makernotes of image are also parsed to get this information.
     * \sa ImageOrientation
     */
    KExiv2::ImageOrientation getImageOrientation() const;

    /*! Sets the Exif \a orientation tag of image.
     *
     * Returns \c true if the orientation has been changed in the metadata.
     * \sa ImageOrientation
     */
    bool setImageOrientation(ImageOrientation orientation, bool setProgramName=true) const;

    /*! Returns the image color-space set in Exif metadata.
     *
     *  The makernotes of the image are also parsed to get this information.
     *  \sa ImageColorWorkSpace
     */
    KExiv2::ImageColorWorkSpace getImageColorWorkSpace() const;

    /*! Sets the Exif color-space tag of the image.
     *
     *  Returns \c true if the \a workspace has been changed in the metadata.
     *  \sa ImageColorWorkSpace
     */
    bool setImageColorWorkSpace(ImageColorWorkSpace workspace, bool setProgramName=true) const;

    /*! Returns the timestamp of the image.
     *
     *  First Exif information is checked, then IPTC
     *  if the image don't have Exif information.
     *
     *  If no timestamp is found, a null date is returned.
     */
    QDateTime getImageDateTime() const;

    /*! Sets the Exif and IPTC timestamp.
     *
     *  If \a setDateTimeDigitized is \c true, then the \c Digitalized
     *  timestamp is set, else only the \a Created timestamp is set.
     */
    bool setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized=false,
                          bool setProgramName=true) const;

    /*! Returns the digitization timestamp of the image.
     *
     *  First Exif information is checked, then IPTC.
     *
     *  If no digitization timestamp is found, getImageDateTime() is called if \a fallbackToCreationTime
     *  is \c true, or a null QDateTime is returned if \a fallbackToCreationTime is \c false.
     */
    QDateTime getDigitizationDateTime(bool fallbackToCreationTime=false) const;

    /*! Returns a QImage copy of the IPTC \a preview image.
     *
     * Returns a null image if the preview cannot be found.
     */
    bool getImagePreview(QImage& preview) const;

    /*! Sets the IPTC \a preview image.
     *
     *  The thumbnail image must have the right size prior to this operation
     *  (64Kb max with JPEG file, else 256Kb).
     *
     *  Look at the IPTC specification for details.
     *
     *  Returns \c true if the preview has been changed in the metadata.
     *
     *  Re-implement this method if you want to use an image file format other than JPEG to
     *  save the preview.
    */
    virtual bool setImagePreview(const QImage& preview, bool setProgramName=true) const;

    //@}

    //-----------------------------------------------------------------
    /// @name Comments manipulation methods
    //@{

    /*! Returns \c true if Comments can be written in file in the given \a filePath.
     */
    static bool canWriteComment(const QString& filePath);

    /*! Returns \c true if metadata container in memory has Comments.
     */
    bool hasComments() const;

    /*! Clears the Comments metadata container in memory.
     */
    bool clearComments() const;

    /*! Returns a Qt byte array copy of Comments container acquired from the current image.
     *
     *  Comments are a JFIF section of JPEG images.
     *
     *  Look at the Exiv2 API for more information.
     *
     *  Returns a null Qt byte array if there is no Comments metadata in memory.
     */
    QByteArray getComments() const;

    /*! Returns a Qt string object of Comments from the current image decoded using
     *  detectEncodingAndDecode().
     *
     *  Returns a null string if there is no Comments metadata available.
     */
    QString getCommentsDecoded() const;

    /*! Sets the Comments \a data using a Qt byte array.
     *
     * Returns \c true if Comments metadata has been changed in memory.
     */
    bool setComments(const QByteArray& data) const;

    /*! Language Alternative autodetection.
     *
     * Returns a QString without language alternative header.
     *
     * Header is saved into \a lang. If no language alternative is found,
     * \a value is returned as well and \a lang is set to a null string.
     */
    static QString detectLanguageAlt(const QString& value, QString& lang);

    //@}

    //-----------------------------------------------------------------
    /// @name Exif manipulation methods
    //@{

    /*! Returns a map of all standard Exif tags supported by Exiv2.
     */
    TagsMap getStdExifTagsList() const;

    /*! Returns a map of all non-standard Exif tags (makernotes) supported by Exiv2.
     */
    TagsMap getMakernoteTagsList() const;

    /*! Returns \c true if Exif can be written to the file in the given \a filePath.
     */
    static bool canWriteExif(const QString& filePath);

    /*! Returns \c true if the metadata container in memory has Exif.
     */
    bool hasExif() const;

    /*! Clears the Exif metadata container in memory.
     */
    bool clearExif() const;

    /*! Returns the Exif data encoded to a QByteArray in a form suitable
     *  for storage in a JPEG image.
     *
     *  \note This encoding is a lossy operation.
     *
     *  Set \a addExifHeader to \c true to add an Exif header to the Exif metadata.
     *
     *  Returns a null Qt byte array if there is no Exif metadata in memory.
     */
    QByteArray getExifEncoded(bool addExifHeader=false) const;

    /*! Sets the Exif \a data using a Qt byte array.
     *
     * Returns \c true if Exif metadata has been changed in memory.
     */
    bool setExif(const QByteArray& data) const;

    /*! Returns a QImage copy of the Exif thumbnail image.
     *
     * Returns a null image if the thumbnail cannot be found.
     *
     * The \a fixOrientation parameter will rotate the thumbnail automatically
     * if Exif orientation tags information is attached to the thumbnail.
     */
    QImage getExifThumbnail(bool fixOrientation) const;

    /*! Fixes orientation of a QImage \a image according to the Exif \a orientation tag.
     *
     *  Returns \c true if image is rotated, \c false otherwise.
     */
    bool rotateExifQImage(QImage& image, ImageOrientation orientation) const;

    /*! Sets the Exif Thumbnail image.
     *
     * The thumbnail image must have the right dimensions prior to this operation.
     *
     * Look at the Exif specification for details.
     *
     * Returns \c true if the thumbnail has been changed in the metadata.
     */
    bool setExifThumbnail(const QImage& thumb, bool setProgramName=true) const;

    /*! Remove the Exif thumbnail from the image. */
    bool removeExifThumbnail() const;

    /*! Adds a JPEG thumbnail \a thumb to TIFF images.
     *
     * Use this instead of setExifThumbnail() for TIFF images.
     */
    bool setTiffThumbnail(const QImage& thumb, bool setProgramName=true) const;

    /*! Returns a QString copy of Exif user comments.
     *
     * Returns a null string if user comments cannot be found.
     */
    QString getExifComment() const;

    /*! Sets an Exif user \a comment from the image.
     *
     * Look at the Exif specification for more details about this tag.
     *
     * Returns \c true if the Exif user comment has been changed in the metadata.
     */
    bool setExifComment(const QString& comment, bool setProgramName=true) const;

    /*! Gets an Exif tags content \a exifTagName as a string.
     *
     * If \a escapeCR is \c true, the CR characters will be removed.
     *
     * Returns a null string if the Exif tag cannot be found.
     */
    QString getExifTagString(const char* exifTagName, bool escapeCR=true) const;

    /*! Sets an Exif tag content \a exifTagName using a string \a value.
     *
     * Returns \c true if the tag is set successfully.
     */
    bool setExifTagString(const char* exifTagName, const QString& value, bool setProgramName=true) const;

    /*! Gets an Exif tag content \a exifTagName as a long value \a val.
     *
     * Returns \c true if the Exif tag is found.
     */
    bool getExifTagLong(const char* exifTagName, long &val) const;

    /*! Gets an Exif tag content \a exifTagName as a long value \a val.
     *
     * Returns \c true if the Exif tag be found.
     */
    bool getExifTagLong(const char* exifTagName, long &val, int component) const;

    /*! Sets an Exif tag content \a exifTagName using a long value \a val.
     *
     * Returns \c true if the Exif tag is set successfully.
     */
    bool setExifTagLong(const char* exifTagName, long val, bool setProgramName=true) const;

    /*! Gets the \a component index of an Exif tags content \a exifTagName as a rational value
     *  with the numerator \a num and denominator \a den.
     *
     *  Returns \c true if the Exif tag is found.
     */
    bool getExifTagRational(const char* exifTagName, long int& num, long int& den, int component=0) const;

    /*! Sets an Exif tag content \a exifTagName using a rational value
     *  with the numerator \a num and denominator \a den.
     *
     *  Returns \c true if tag is set successfully.
     */
    bool setExifTagRational(const char* exifTagName, long int num, long int den, bool setProgramName=true) const;

    /*! Gets an Exif tag content \a exifTagName as a byte array.
     *
     * Returns an empty byte array if the Exif tag cannot be found.
     */
    QByteArray getExifTagData(const char* exifTagName) const;

    /*! Sets an Exif tag content \a exifTagName using a byte array \a data.
     *
     * Returns \c true if the tag is set successfully.
     */
    bool setExifTagData(const char* exifTagName, const QByteArray& data, bool setProgramName=true) const;

    /*! Gets an Exif tags content \a exifTagName as a QVariant.
     *
     * Returns a null QVariant if the Exif tag cannot be found.
     *
     * For string and integer values the matching QVariant types will be used,
     * for date and time values QVariant::DateTime.
     *
     * Rationals will be returned as QVariant::List with two integer QVariants (numerator, denominator)
     * if \a rationalAsListOfInts is \c true, as double if \a rationalAsListOfInts is \c false.
     *
     * An exif tag of numerical type may contain more than one value; set \a component to the desired index.
     */
    QVariant getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts=true, bool escapeCR=true, int component=0) const;

    /*! Sets an Exif tag content \a exifTagName using a QVariant \a data.
     *
     * Returns \c true if the Exif tag is set successfully.
     *
     * All types described for getExifTagVariant() are supported.
     *
     * Calling with a QVariant of type QByteArray is equivalent to calling setExifTagData().
     *
     * For the meaning of \a rationalWantSmallDenominator, see the documentation for the convertToRational methods.
     *
     * Setting a value with multiple components is currently not supported.
     */
    bool setExifTagVariant(const char* exifTagName, const QVariant& data,
                           bool rationalWantSmallDenominator=true, bool setProgramName=true) const;

    /*! Removes the Exif tag \a exifTagName from the Exif metadata.
     *
     * Returns \c true if the Exif tag is removed successfully or if no tag was present.
     */
    bool removeExifTag(const char* exifTagName, bool setProgramName=true) const;

    /*! Returns the Exif Tag title for the \a exifTagName or a null string.
     */
    QString getExifTagTitle(const char* exifTagName);

    /*! Returns the Exif Tag description for the \a exifTagName or a null string.
     */
    QString getExifTagDescription(const char* exifTagName);

    /*! Takes a QVariant value \a val as it could have been retrieved by getExifTagVariant()
     *  with the given \a exifTagName,
     *  and returns its value properly converted to a string (including translations from Exiv2).
     *
     *  This is equivalent to calling getExifTagString() directly.
     *
     *  If escapeCR is \c true CR characters will be removed from the result.
     */
    QString createExifUserStringFromValue(const char* exifTagName, const QVariant& val, bool escapeCR=true);

    /*! Returns a map of Exif tags name/value found in the metadata sorted by
     *  Exif keys given by \a exifKeysFilter.
     *
     *  The \a exifKeysFilter is a QStringList of Exif keys.
     *
     *  For example, if you use the string list given below:
     *
     *  "Iop"
     *
     *  "Thumbnail"
     *
     *  "Image"
     *
     *  "Photo"
     *
     *  The list can be empty to not filter output.
     *
     *  Given the above example, this method will return a map of all Exif tags which:
     *
     * \list
     * \li will include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys
     *     if \a invertSelection is \c false.
     * \li will not include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys
     *     if \a invertSelection is \c true.
     * \endlist
     */
    KExiv2::MetaDataMap getExifTagsDataList(const QStringList& exifKeysFilter=QStringList(), bool invertSelection=false) const;

    //@}

    //-------------------------------------------------------------
    /// @name IPTC manipulation methods
    //@{

    /*! Returns a map of all standard IPTC tags supported by Exiv2.
     */
    KExiv2::TagsMap getIptcTagsList() const;

    /*! Returns \c true if IPTC can be written to file in the given \a filePath.
     */
    static bool canWriteIptc(const QString& filePath);

    /*! Returns \c true if metadata container in memory has IPTC.
     */
    bool hasIptc() const;

    /*! Clears the IPTC metadata container in memory.
     */
    bool clearIptc() const;

    /*! Returns a Qt byte array copy of the IPTC container acquired from the current image.
     *
     * Set \a addIrbHeader to \c true to add an Irb header to IPTC metadata.
     *
     * Returns a null QByteArray if there is no IPTC metadata in memory.
     */
    QByteArray  getIptc(bool addIrbHeader=false) const;

    /*! Sets the IPTC data using a QByteArray \a data.
     *
     * Returns \c true if the IPTC metadata has been changed in memory.
     */
    bool setIptc(const QByteArray& data) const;

    /*! Gets an IPTC tag content \a iptcTagName as a string.
     *
     * If \a escapeCR is set to \c true, the CR characters will be removed.
     *
     * Returns a null string if the IPTC tag cannot be found.
     */
    QString getIptcTagString(const char* iptcTagName, bool escapeCR=true) const;

    /*! Sets an IPTC tag content \a iptcTagName using a string \a value.
     *
     *  Returns \c true if tag is set successfully.
     */
    bool setIptcTagString(const char* iptcTagName, const QString& value, bool setProgramName=true) const;

    /*! Gets the values of all IPTC tags with the given tag name \a iptcTagName in a string list.
     *
     * Returns an empty list if no tag is found.
     *
     * (In IPTC, there can be multiple tags with the same name)
     *
     * If \a escapeCR is set to \c true, the CR characters will be removed.
     */
    QStringList getIptcTagsStringList(const char* iptcTagName, bool escapeCR=true) const;

    /*! Sets multiple IPTC tags contents with the given tag name \a iptcTagName using a string list \a newValues to replace \a oldValues.
     *
     * \a maxSize is the max characters size of one entry.
     *
     * Returns \c true if all tags have been set successfully.
     */
    bool setIptcTagsStringList(const char* iptcTagName, int maxSize,
                               const QStringList& oldValues, const QStringList& newValues,
                               bool setProgramName=true) const;

    /*! Gets an IPTC tag content \a iptcTagName as a byte array.
     *
     * Returns an empty byte array if the IPTC tag cannot be found.
     */
    QByteArray getIptcTagData(const char* iptcTagName) const;

    /*! Sets an IPTC tag content \a iptcTagName using a byte array \a data.
     *
     * Returns \c true if tag is set successfully.
     */
    bool setIptcTagData(const char* iptcTagName, const QByteArray& data, bool setProgramName=true) const;

    /*! Removes all instances of the IPTC tag \a iptcTagName from the IPTC metadata.
     *
     * Returns \c true if all tags have been removed successfully (or none were present).
     */
    bool removeIptcTag(const char* iptcTagName, bool setProgramName=true) const;

    /*! Returns the IPTC Tag title for \a iptcTagName or a null string.
     */
    QString getIptcTagTitle(const char* iptcTagName);

    /*! Returns the IPTC Tag description for \a iptcTagName or a null string.
     */
    QString getIptcTagDescription(const char* iptcTagName);

    /*! Returns a map of IPTC tags name/value found in the metadata sorted by
     *  IPTC keys given by \a iptcKeysFilter.
     *
     *  \a iptcKeysFilter is a QStringList of IPTC keys.
     *
     *  For example, if you use the string list given below:
     *
     *  "Envelope"
     *
     *  "Application2"
     *
     *  List can be empty to not filter output.
     *
     *  Given the above example, this method will return a map of all IPTC tags which:
     *
     *  \list
     *  \li will include "Envelope", or "Application2" in the IPTC tag keys
     *     if \a invertSelection is \c false.
     *  \li will not include "Envelope", or "Application2" in the IPTC tag keys
     *     if \a invertSelection is \c true.
     *  \endlist
     */
    KExiv2::MetaDataMap getIptcTagsDataList(const QStringList& iptcKeysFilter=QStringList(), bool invertSelection=false) const;

    /*! Returns a strings list of IPTC keywords from the image.
     *
     * Returns an empty list if no keywords are set.
     */
    QStringList getIptcKeywords() const;

    /*! Sets IPTC keywords using a list of strings defined by \a newKeywords to replace \a oldKeywords.
     *
     * Use getImageKeywords() to set \a oldKeywords to existing keywords from the image.
     *
     * This method will compare all new keywords with all old keywords to prevent
     * duplicate entries in the image.
     *
     * Returns \c true if the keywords have been changed in the metadata.
     */
    bool setIptcKeywords(const QStringList& oldKeywords, const QStringList& newKeywords,
                         bool setProgramName=true) const;

    /*! Returns a strings list of IPTC subjects from the image.
     *
     * Returns an empty list if no subjects are set.
     */
    QStringList getIptcSubjects() const;

    /*! Sets IPTC subjects using a list of strings defined by \a newSubjects to replace \a oldSubjects.
     *
     * Use getImageSubjects() to set \a oldSubjects with existing subjects from the image.
     *
     * This method will compare all new subjects with all old subjects to
     * prevent duplicate entries in image.
     *
     * Returns \c true if the subjects have been changed in the metadata.
     */
    bool setIptcSubjects(const QStringList& oldSubjects, const QStringList& newSubjects,
                         bool setProgramName=true) const;

    /*! Returns a strings list of IPTC subcategories from the image.
     *
     * Returns an empty list if no subcategory is set.
     */
    QStringList getIptcSubCategories() const;

    /*! Sets IPTC subcategories using a list of strings defined by \a newSubCategories to replace \a oldSubCategories.
     *
     * Use getImageSubCategories() to set \a oldSubCategories with existing subcategories from the image.
     *
     * This method will compare all new subcategories with all old subcategories to prevent
     * duplicate entries in image.
     *
     * Returns \c true if the subcategories have been changed in the metadata.
     */
    bool setIptcSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories,
                              bool setProgramName=true) const;

    //@}

    //------------------------------------------------------------
    /// @name XMP manipulation methods
    //@{

    /*! Returns a map of all standard XMP tags supported by Exiv2.
     */
    KExiv2::TagsMap getXmpTagsList() const;

    /*! Returns \c true if XMP can be written to file in \a filePath.
     */
    static bool canWriteXmp(const QString& filePath);

    /*! Returns \c true if the metadata container in memory has XMP.
     */
    bool hasXmp() const;

    /*! Clears the XMP metadata container in memory.
     */
    bool clearXmp() const;

    /*! Returns a QByteArray copy of the XMP container acquired from the current image.
     *
     *  Returns a null QByteArray if there is no XMP metadata in memory.
     */
    QByteArray getXmp() const;

    /*! Sets the XMP \a data using a QByteArray.
     *
     * Returns \c true if the XMP metadata has been changed in memory.
     */
    bool setXmp(const QByteArray& data) const;

    /*! Gets an XMP tag content \a xmpTagName as a string.
     *
     * If \a escapeCR is set to \c true, the CR characters will be removed.
     *
     * Returns a null string if the XMP tag cannot be found.
     */
    QString getXmpTagString(const char* xmpTagName, bool escapeCR=true) const;

    /*! Sets an XMP tag content \a xmpTagName using a string \a value.
     *
     * Returns \c true if tag is set successfully.
     */
    bool setXmpTagString(const char* xmpTagName, const QString& value,
                         bool setProgramName=true) const;

    /*! Sets an XMP tag \a xmpTagName with a specific \a type to the given \a value.
     *
     * Returns \c true if tag is set successfully.
     *
     * This method only accept NormalTag, ArrayBagTag and StructureTag.
     * Other XmpTagTypes do nothing.
     */
    bool setXmpTagString(const char* xmpTagName, const QString& value,
                         XmpTagType type,bool setProgramName=true) const;

    /*! Returns the XMP Tag title for \a xmpTagName or a null string.
     */
    QString getXmpTagTitle(const char* xmpTagName);

    /*! Returns the XMP Tag description for \a xmpTagName or a null string.
     */
    QString getXmpTagDescription(const char* xmpTagName);

    /*! Returns a map of XMP tags name/value pairs found in the metadata sorted by
     *  XMP keys given by \a xmpKeysFilter.
     *
     *  \a xmpKeysFilter is a QStringList of XMP keys.
     *
     *  For example, if you use the string list given below:
     *
     *  "dc"           // Dubling Core schema.
     *
     *  "xmp"          // Standard XMP schema.
     *
     *  List can be empty to not filter output.
     *
     *  Given the above example, this method will return a map of all XMP tags which:
     *
     *  \list
     *  \li will include "dc", or "xmp" in the XMP tag keys
     *      if \a invertSelection is \c false.
     *  \li will not include "dc", or "xmp" in the XMP tag keys
     *      if \a invertSelection is \c true.
     *  \endlist
     */
    KExiv2::MetaDataMap getXmpTagsDataList(const QStringList& xmpKeysFilter=QStringList(), bool invertSelection=false) const;

    /*! Gets all redundant Alternative Language XMP tags content \a xmpTagName as a map.
     *
     *  See AltLangMap class description for details.
     *
     *  If \a escapeCR is set to \c true, the CR characters will be removed from strings.
     *
     *  Returns  null string list if the XMP tag cannot be found.
     */
    KExiv2::AltLangMap getXmpTagStringListLangAlt(const char* xmpTagName, bool escapeCR=true) const;

    /*! Sets an Alternative Language XMP tag content \a xmpTagName using a map with the given \a values.
     *
     * See AltLangMap class description for details.
     *
     * If \a xmpTagName already exists, it wil be removed prior to this operation.
     *
     * Returns \c true if the tag is set successfully.
     */
    bool setXmpTagStringListLangAlt(const char* xmpTagName, const KExiv2::AltLangMap& values,
                                    bool setProgramName) const;

    /*! Gets an XMP tag content \a xmpTagName as a string set with an alternative language
     *  header \a langAlt (like "fr-FR" for French - RFC3066 notation).
     *
     *  If \a escapeCR is set to \c true, the CR characters will be removed.
     *
     *  Returns a null string if the XMP tag cannot be found.
     */
    QString getXmpTagStringLangAlt(const char* xmpTagName, const QString& langAlt, bool escapeCR) const;

    /*! Sets an XMP tag content \a xmpTagName using a string \a value with an alternative language header \a langAlt.
     *
     *  \a langAlt contains the language alternative information
     *  (like "fr-FR" for French - RFC3066 notation) or is null to
     *  set alternative language to default settings ("x-default").
     *
     *  Returns \c true if the tag is set successfully.
     */
    bool setXmpTagStringLangAlt(const char* xmpTagName, const QString& value,
                                const QString& langAlt, bool setProgramName=true) const;

    /*! Gets an XMP tag content \a xmpTagName as a sequence of strings.
     *
     * If \a escapeCR is set to \c true, the CR characters will be removed from strings.
     *
     * Returns a null string list if the XMP tag cannot be found.
     */
    QStringList getXmpTagStringSeq(const char* xmpTagName, bool escapeCR=true) const;

    /*! Sets an XMP tag content \a xmpTagName using the sequence of strings \a seq.
     *
     *  Returns \c true if the tag is set successfully.
     */
    bool setXmpTagStringSeq(const char* xmpTagName, const QStringList& seq,
                            bool setProgramName=true) const;

    /*! Gets an XMP tag content \a xmpTagName as a bag of strings.
     *
     * If \a escapeCR is set to \c true, the CR characters will be removed from strings.
     *
     * Returns a null string list if the XMP tag cannot be found.
     */
    QStringList getXmpTagStringBag(const char* xmpTagName, bool escapeCR) const;

    /*! Sets an XMP tag content \a xmpTagName using the bag of strings \a bag.
     *
     *  Returns \c true if tag is set successfully.
     */
    bool setXmpTagStringBag(const char* xmpTagName, const QStringList& bag,
                            bool setProgramName=true) const;

    /*! Sets an XMP tag content \a xmpTagName using a list of strings defined by the \a entriesToAdd.
     *
     *  The existing entries are preserved.
     *
     *  This method will compare all new entries with all already existing entries
     *  to prevent duplicates in the image.
     *
     *  Returns \c true if the entries have been added to metadata.
     */
    bool addToXmpTagStringBag(const char* xmpTagName, const QStringList& entriesToAdd,
                              bool setProgramName) const;

    /*! Removes the XMP tag entries \a entriesToRemove for the XMP tag content \a xmpTagName from the entries in the metadata.
     *
     *  Returns \c true if the tag entries are no longer contained in the metadata.
     *
     *  All other entries are preserved.
     */
    bool removeFromXmpTagStringBag(const char* xmpTagName, const QStringList& entriesToRemove,
                                   bool setProgramName) const;

    /*! Gets an XMP tag content \a xmpTagName as a QVariant.
     *
     *  Returns a null QVariant if the XMP tag cannot be found.
     *
     *  For string and integer values the matching QVariant types will be used,
     *  for date and time values QVariant::DateTime.
     *
     *  Rationals will be returned as QVariant::List with two integer QVariants (numerator, denominator)
     *  if \a rationalAsListOfInts is \c true, as double if \a rationalAsListOfInts is \c false.
     *
     *  Arrays (ordered, unordered, alternative) are returned as a QStringList.
     *
     *  LangAlt values will have type Map (\c {QMap<QString, QVariant>}) with the language
     *  code as key and the contents as value, of type QString.
     */
    QVariant getXmpTagVariant(const char* xmpTagName, bool rationalAsListOfInts=true, bool stringEscapeCR=true) const;

    /*! Returns a strings list of XMP keywords from the image.
     *
     * Returns a null string list if no keywords are set.
     */
    QStringList getXmpKeywords() const;

    /*! Sets XMP keywords using a list of strings defined by \a newKeywords.
     *
     *  The existing keywords from the image are preserved.
     *
     *  This method will compare all new keywords with all already existing keywords
     *  to prevent duplicate entries in the image.
     *
     *  Returns \c true if the keywords have been changed in the metadata.
     */
    bool setXmpKeywords(const QStringList& newKeywords, bool setProgramName=true) const;

    /*! Removes the XMP keywords \a keywordsToRemove from the keywords in the metadata.
     *
     *  Returns \c true if the keywords are no longer contained in the metadata.
     */
    bool removeXmpKeywords(const QStringList& keywordsToRemove, bool setProgramName=true);

    /*! Returns a string list of XMP subjects from the image.
     *
     *  Returns a null string list if no subjects are set.
     */
    QStringList getXmpSubjects() const;

    /*! Sets XMP subjects using a string list of \a newSubjects.
     *
     *  The existing subjects from the image are preserved.
     *
     *  This method will compare all new subjects with all already existing subjects
     *  to prevent duplicate entries in the image.
     *
     *  Returns \c true if the subjects have been changed in the metadata.
     */
    bool setXmpSubjects(const QStringList& newSubjects, bool setProgramName=true) const;

    /*! Removes the XMP \a subjectsToRemove from the subjects in the metadata.
     *
     *  Returns \c true if the subjects are no longer contained in the metadata.
     */
    bool removeXmpSubjects(const QStringList& subjectsToRemove, bool setProgramName=true);

    /*! Returns a string list of XMP subcategories from the image.
     *
     * Returns a null string list if no subcategories are set.
     */
    QStringList getXmpSubCategories() const;

    /*! Sets XMP subcategories using the string list \a newSubCategories.
     *
     *  The existing subcategories from the image are preserved.
     *
     *  This method will compare all new subcategories with all already existing subcategories
     *  to prevent duplicate entries in the image.
     *
     *  Returns \c true if the subcategories have been changed in the metadata.
     */
    bool setXmpSubCategories(const QStringList& newSubCategories, bool setProgramName=true) const;

    /*! Removes the XMP sub \a categoriesToRemove from the subcategories in the metadata.
     *
     *  Returns \c true if the subjects are no longer contained in the metadata.
     */
    bool removeXmpSubCategories(const QStringList& categoriesToRemove, bool setProgramName=true);

    /*! Removes the XMP tag \a xmpTagName from XMP metadata.
     *
     * Returns \c true if the tag is removed successfully or if no tag was present.
     */
    bool removeXmpTag(const char* xmpTagName, bool setProgramName=true) const;


    /*! Registers a namespace which Exiv2 doesn't know yet.
     *
     *  This is only needed when new XMP properties are added manually.
     *
     *  The \a uri is the namespace url and prefix the string used to construct new XMP key.
     *
     *  \note If the XMP metadata is read from an image, namespaces are decoded and registered
     *  by Exiv2 at the same time.
     */
    static bool registerXmpNameSpace(const QString& uri, const QString& prefix);

    /*! Unregisters a previously registered custom namespace. */
    static bool unregisterXmpNameSpace(const QString& uri);

    //@}

    //------------------------------------------------------------
    /// @name GPS manipulation methods
    //@{

    /*! Makes sure all static required GPS EXIF and XMP tags exist.
     */
    bool initializeGPSInfo(const bool setProgramName);

    /*! Gets all GPS location information set in the image.
     *
     * Returns \c true if all information can be found.
     */
    bool getGPSInfo(double& altitude, double& latitude, double& longitude) const;

    /*! Gets GPS location information set in the image, in the GPSCoordinate format
     *  as described in the XMP specification.
     *
     *  Returns a null string if the information cannot be found.
     */
    QString getGPSLatitudeString() const;
    /*! Gets GPS location information set in the image, in the GPSCoordinate format
     *  as described in the XMP specification.
     *
     *  Returns a null string in the information cannot be found.
     */
    QString getGPSLongitudeString() const;

    /*! Gets GPS location information set in the image, as a double floating point number as in degrees
     *  where the sign determines the direction ref (North + / South - ; East + / West -).
     *
     *  Returns \c true if the information is available.
     */
    bool getGPSLatitudeNumber(double* const latitude) const;
    /*! Gets GPS location information set in the image, as a double floating point number as in degrees
     *  where the sign determines the direction ref (North + / South - ; East + / West -).
     *
     *  Returns \c true if the information is available.
     */
    bool getGPSLongitudeNumber(double* const longitude) const;

    /*! Gets GPS altitude information, in meters, relative to sea level (positive sign above sea level).
     */
    bool getGPSAltitude(double* const altitude) const;

    /*! Sets all GPS location information to the image.
     *
     * Returns \c true if all information has been changed in the metadata.
     */
    bool setGPSInfo(const double altitude, const double latitude, const double longitude, const bool setProgramName=true);

    /*! Sets all GPS location information to the image.
     *
     * Returns \c true if all information has been changed in the metadata.
     *
     * If you do not want altitude to be set, pass a null pointer.
     */
    bool setGPSInfo(const double* const altitude, const double latitude, const double longitude, const bool setProgramName=true);

    /*! Sets all GPS location information to the image.
     *
     * Returns \c true if all information has been changed in the metadata.
     */
    bool setGPSInfo(const double altitude, const QString &latitude, const QString &longitude, const bool setProgramName=true);

    /*! Removes all Exif tags relevant to the GPS location information.
     *
     * Returns \c true if all tags have been removed successfully from the metadata.
     */
    bool removeGPSInfo(const bool setProgramName=true);

    /*! Converts \a number to a rational value, returned in the \a numerator and
     *  \a denominator parameters.
     *
     *  Set the precision using the \a rounding parameter.
     *
     *  Use this method if you want to retrieve a most exact rational for a number
     *  without further properties, without any requirements for the denominator.
     */
    static void convertToRational(const double number, long int* const numerator,
                                  long int* const denominator, const int rounding);

    /*! Converts a \a number to a rational value, returned in the \a numerator and
     *  \a denominator parameters.
     *
     *  This method will be able to retrieve a rational number from a double.
     *
     * If you constructed your double with 1.0 / 4786.0, this method will retrieve 1 / 4786.
     *
     *  If your number is not expected to be rational, use convertToRational() which is just as
     *  exact with rounding = 4 and more exact with rounding > 4.
     */
    static void convertToRationalSmallDenominator(const double number, long int* const numerator,
                                                  long int* const denominator);

    /*! Converts a GPS position stored as rationals in Exif to the form described
     *  as GPSCoordinate in the XMP specification, either in the from "256,45,34N" or "256,45.566667N"
     */
    static QString convertToGPSCoordinateString(const long int numeratorDegrees, const long int denominatorDegrees,
                                                const long int numeratorMinutes, const long int denominatorMinutes,
                                                const long int numeratorSeconds, long int denominatorSeconds,
                                                const char directionReference);

    /*! Converts a GPS position stored as double floating point number in degrees to the form described
     *  as GPSCoordinate in the XMP specification.
     */
    static QString convertToGPSCoordinateString(const bool isLatitude, double coordinate);

    /*! Converts a GPSCoordinate string as defined by XMP to three rationals and the direction reference.
     *
     *  Returns \c true if the conversion was successful.
     *
     *  If minutes is given in the fractional form, a denominator of 1000000 for the minutes will be used.
     */
    static bool convertFromGPSCoordinateString(const QString& coordinate,
                                               long int* const numeratorDegrees, long int* const denominatorDegrees,
                                               long int* const numeratorMinutes, long int* const denominatorMinutes,
                                               long int* const numeratorSeconds, long int* const denominatorSeconds,
                                               char* const directionReference);

    /*! Converts a GPSCoordinate string as defined by XMP to a double floating point number in degrees
     *  where the sign determines the direction ref (North + / South - ; East + / West -).
     *
     *  Returns \c true if the conversion was successful.
     */
    static bool convertFromGPSCoordinateString(const QString& gpsString, double* const coordinate);

    /*! Converts a GPSCoordinate string to user presentable numbers, integer degrees and minutes and
     *  double floating point seconds, and a direction reference ('N' or 'S', 'E' or 'W').
     */
    static bool convertToUserPresentableNumbers(const QString& coordinate,
                                                int* const degrees, int* const minutes,
                                                double* const seconds, char* const directionReference);

    /*! Converts a double floating point number to user presentable numbers, integer degrees and minutes and
     *  double floating point seconds, and a direction reference ('N' or 'S', 'E' or 'W').
     *
     *  This method needs to know for the direction reference
     *  if the latitude or the longitude is meant by the double parameter.
     */
    static void convertToUserPresentableNumbers(const bool isLatitude, double coordinate,
                                                int* const degrees, int* const minutes,
                                                double* const seconds, char* const directionReference);

    //@}

protected:

    /*! Re-implement this method to automatically set the Program Name and Program Version
     *  information in Exif and IPTC metadata if \a on is set to \c true.
     *
     *  This method is called by all methods which change tags in the metadata.
     *
     *  By default this method does nothing and returns \c true.
     */
    virtual bool setProgramId(bool on=true) const;

private:

    /*! Internal container to store private members.
     *
     * Used to improve binary compatibility.
     * \internal
     */
    std::unique_ptr<class KExiv2Private> const d;

    friend class KExiv2Previews;
};

}  // NameSpace KExiv2Iface

#endif /* KEXIV2_H */
