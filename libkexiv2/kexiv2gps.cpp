/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-15
 * @brief  GPS manipulation methods
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2010 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#include "kexiv2.h"
#include "kexiv2_p.h"

// C++ includes.

#include <climits>
#include <cmath>

#include <math.h>

namespace KExiv2Iface
{

bool KExiv2::getGPSInfo(double& altitude, double& latitude, double& longitude) const
{
    // Some GPS device do not set Altitude. So a valid GPS position can be with a zero value.
    getGPSAltitude(&altitude);

    if (!getGPSLatitudeNumber(&latitude))
         return false;
    if (!getGPSLongitudeNumber(&longitude))
         return false;
    return true;
}

bool KExiv2::getGPSLatitudeNumber(double* const latitude) const
{
    try
    {
        *latitude=0.0;

        // Try XMP first. Reason: XMP in sidecar may be more up-to-date than EXIF in original image.
        if ( convertFromGPSCoordinateString(getXmpTagString("Xmp.exif.GPSLatitude"), latitude) )
            return true;

        // Now try to get the reference from Exif.
        const QByteArray latRef = getExifTagData("Exif.GPSInfo.GPSLatitudeRef");
        if (!latRef.isEmpty())
        {
            // Latitude decoding from Exif.
            double num, den, min, sec;

            Exiv2::ExifKey exifKey("Exif.GPSInfo.GPSLatitude");
            Exiv2::ExifData exifData(d->exifMetadata());
            Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
            if (it != exifData.end() && (*it).count() == 3)
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                if (den == 0)
                    return false;
                *latitude = num/den;

                num = (double)((*it).toRational(1).first);
                den = (double)((*it).toRational(1).second);
                if (den == 0)
                    return false;
                min = num/den;
                if (min != -1.0)
                    *latitude = *latitude + min/60.0;

                num = (double)((*it).toRational(2).first);
                den = (double)((*it).toRational(2).second);
                if (den == 0)
                {
                    // be relaxed and accept 0/0 seconds. See #246077.
                    if (num == 0)
                        den = 1;
                    else
                        return false;
                }
                sec = num/den;
                if (sec != -1.0)
                    *latitude = *latitude + sec/3600.0;
            }
            else
            {
                return false;
            }

            if (latRef[0] == 'S')
                *latitude *= -1.0;

            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot get GPS tag using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::getGPSLongitudeNumber(double* const longitude) const
{
    try
    {
        *longitude=0.0;

        // Try XMP first. Reason: XMP in sidecar may be more up-to-date than EXIF in original image.
        if ( convertFromGPSCoordinateString(getXmpTagString("Xmp.exif.GPSLongitude"), longitude) )
            return true;

        // Now try to get the reference from Exif.
        const QByteArray lngRef = getExifTagData("Exif.GPSInfo.GPSLongitudeRef");
        if (!lngRef.isEmpty())
        {
            // Longitude decoding from Exif.
            double num, den, min, sec;

            Exiv2::ExifKey exifKey2("Exif.GPSInfo.GPSLongitude");
            Exiv2::ExifData exifData(d->exifMetadata());
            Exiv2::ExifData::iterator it = exifData.findKey(exifKey2);
            if (it != exifData.end() && (*it).count() == 3)
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                if (den == 0)
                    return false;
                *longitude = num/den;

                num = (double)((*it).toRational(1).first);
                den = (double)((*it).toRational(1).second);
                if (den == 0)
                    return false;
                min = num/den;
                if (min != -1.0)
                    *longitude = *longitude + min/60.0;

                num = (double)((*it).toRational(2).first);
                den = (double)((*it).toRational(2).second);
                if (den == 0)
                {
                    // be relaxed and accept 0/0 seconds. See #246077.
                    if (num == 0)
                        den = 1;
                    else
                        return false;
                }
                sec = num/den;
                if (sec != -1.0)
                    *longitude = *longitude + sec/3600.0;
            }
            else
            {
                return false;
            }

            if (lngRef[0] == 'W')
                *longitude *= -1.0;

            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot get GPS tag using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::getGPSAltitude(double* const altitude) const
{
    try
    {
        double num, den;
        *altitude=0.0;

        // Try XMP first. Reason: XMP in sidecar may be more up-to-date than EXIF in original image.
        const QString altRefXmp = getXmpTagString("Xmp.exif.GPSAltitudeRef");
        if (!altRefXmp.isEmpty())
        {
            const QString altXmp = getXmpTagString("Xmp.exif.GPSAltitude");
            if (!altXmp.isEmpty())
            {
                num = altXmp.section('/', 0, 0).toDouble();
                den = altXmp.section('/', 1, 1).toDouble();
                if (den == 0)
                    return false;
                *altitude = num/den;

                if (altRefXmp == QString("1"))
                    *altitude *= -1.0;

                return true;
            }
        }

        // Get the reference from Exif (above/below sea level)
        const QByteArray altRef = getExifTagData("Exif.GPSInfo.GPSAltitudeRef");
        if (!altRef.isEmpty())
        {
            // Altitude decoding from Exif.

            Exiv2::ExifKey exifKey3("Exif.GPSInfo.GPSAltitude");
            Exiv2::ExifData exifData(d->exifMetadata());
            Exiv2::ExifData::iterator it = exifData.findKey(exifKey3);
            if (it != exifData.end() && (*it).count())
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                if (den == 0)
                    return false;
                *altitude = num/den;
            }
            else
            {
                return false;
            }

            if (altRef[0] == '1')
                *altitude *= -1.0;

            return true;
        }
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot get GPS tag using Exiv2 ", e);
    }

    return false;
}

QString KExiv2::getGPSLatitudeString() const
{
    double latitude;
    if (!getGPSLatitudeNumber(&latitude))
        return QString();

    return convertToGPSCoordinateString(true, latitude);
}

QString KExiv2::getGPSLongitudeString() const
{
    double longitude;
    if (!getGPSLongitudeNumber(&longitude))
        return QString();

    return convertToGPSCoordinateString(false, longitude);
}

bool KExiv2::initializeGPSInfo(const bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // TODO: what happens if these already exist?

        // Do all the easy constant ones first.
        // GPSVersionID tag: standard says is should be four bytes: 02 00 00 00
        // (and, must be present).
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("2 0 0 0");
        d->exifMetadata().add(Exiv2::ExifKey("Exif.GPSInfo.GPSVersionID"), value.get());

        // Datum: the datum of the measured data. If not given, we insert WGS-84.
        d->exifMetadata()["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";

#ifdef _XMP_SUPPORT_
        setXmpTagString("Xmp.exif.GPSVersionID", QString("2.0.0.0"), false);
        setXmpTagString("Xmp.exif.GPSMapDatum", QString("WGS-84"), false);
#endif // _XMP_SUPPORT_
        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot initialize GPS data using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setGPSInfo(const double altitude, const double latitude, const double longitude, const bool setProgramName)
{
    return setGPSInfo(&altitude, latitude, longitude, setProgramName);
}

bool KExiv2::setGPSInfo(const double* const altitude, const double latitude, const double longitude, const bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        // In first, we need to clean up all existing GPS info.
        removeGPSInfo();

        // now re-initialize the GPS info:
        if (!initializeGPSInfo(setProgramName))
            return false;

        char scratchBuf[100];
        long int nom, denom;
        long int deg, min;

        // Now start adding data.

        // ALTITUDE.
        if (altitude)
        {
            // Altitude reference: byte "00" meaning "above sea level", "01" mening "behing sea level".
            Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
            if ((*altitude) >= 0) value->read("0");
            else               value->read("1");
            d->exifMetadata().add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"), value.get());

            // And the actual altitude, as absolute value..
            convertToRational(fabs(*altitude), &nom, &denom, 4);
            snprintf(scratchBuf, 100, "%ld/%ld", nom, denom);
            d->exifMetadata()["Exif.GPSInfo.GPSAltitude"] = scratchBuf;

#ifdef _XMP_SUPPORT_
            setXmpTagString("Xmp.exif.GPSAltitudeRef", ((*altitude) >= 0) ? QString("0") : QString("1"), false);
            setXmpTagString("Xmp.exif.GPSAltitude", QString(scratchBuf), false);
#endif // _XMP_SUPPORT_
        }

        // LATITUDE
        // Latitude reference:
        // latitude < 0 : "S"
        // latitude > 0 : "N"
        //
        d->exifMetadata()["Exif.GPSInfo.GPSLatitudeRef"] = (latitude < 0 ) ? "S" : "N";

        // Now the actual latitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LatRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x1000000 to get to mmmm/1000000.
        deg   = (int)floor(fabs(latitude)); // Slice off after decimal.
        min   = (int)floor((fabs(latitude) - floor(fabs(latitude))) * 60000000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/1000000 0/1", deg, min);
        d->exifMetadata()["Exif.GPSInfo.GPSLatitude"] = scratchBuf;

#ifdef _XMP_SUPPORT_
        /** @todo The XMP spec does not mention Xmp.exif.GPSLatitudeRef,
         * because the reference is included in Xmp.exif.GPSLatitude.
         * Is there a historic reason for writing it anyway?
         */
        setXmpTagString("Xmp.exif.GPSLatitudeRef", (latitude < 0) ? QString("S") : QString("N"), false);
        setXmpTagString("Xmp.exif.GPSLatitude", convertToGPSCoordinateString(true, latitude), false);
#endif // _XMP_SUPPORT_

        // LONGITUDE
        // Longitude reference:
        // longitude < 0 : "W"
        // longitude > 0 : "E"
        d->exifMetadata()["Exif.GPSInfo.GPSLongitudeRef"] = (longitude < 0 ) ? "W" : "E";

        // Now the actual longitude itself.
        // This is done as three rationals.
        // I choose to do it as:
        //   dd/1 - degrees.
        //   mmmm/100 - minutes
        //   0/1 - seconds
        // Exif standard says you can do it with minutes
        // as mm/1 and then seconds as ss/1, but its
        // (slightly) more accurate to do it as
        //  mmmm/100 than to split it.
        // We also absolute the value (with fabs())
        // as the sign is encoded in LongRef.
        // Further note: original code did not translate between
        //   dd.dddddd to dd mm.mm - that's why we now multiply
        //   by 6000 - x60 to get minutes, x1000000 to get to mmmm/1000000.
        deg   = (int)floor(fabs(longitude)); // Slice off after decimal.
        min   = (int)floor((fabs(longitude) - floor(fabs(longitude))) * 60000000);
        snprintf(scratchBuf, 100, "%ld/1 %ld/1000000 0/1", deg, min);
        d->exifMetadata()["Exif.GPSInfo.GPSLongitude"] = scratchBuf;

#ifdef _XMP_SUPPORT_
        /** @todo The XMP spec does not mention Xmp.exif.GPSLongitudeRef,
         * because the reference is included in Xmp.exif.GPSLongitude.
         * Is there a historic reason for writing it anyway?
         */
        setXmpTagString("Xmp.exif.GPSLongitudeRef", (longitude < 0) ? QString("W") : QString("E"), false);
        setXmpTagString("Xmp.exif.GPSLongitude", convertToGPSCoordinateString(false, longitude), false);
#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot set Exif GPS tag using Exiv2 ", e);
    }

    return false;
}

bool KExiv2::setGPSInfo(const double altitude, const QString& latitude, const QString& longitude, const bool setProgramName)
{
    double longitudeValue, latitudeValue;
    if (!convertFromGPSCoordinateString(latitude, &latitudeValue))
        return false;
    if (!convertFromGPSCoordinateString(longitude, &longitudeValue))
        return false;

    return setGPSInfo(&altitude, latitudeValue, longitudeValue, setProgramName);
}

bool KExiv2::removeGPSInfo(const bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {
        QStringList gpsTagsKeys;

        for (Exiv2::ExifData::iterator it = d->exifMetadata().begin();
             it != d->exifMetadata().end(); ++it)
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key.section('.', 1, 1) == QString("GPSInfo"))
                gpsTagsKeys.append(key);
        }

        for(QStringList::const_iterator it2 = gpsTagsKeys.constBegin(); it2 != gpsTagsKeys.constEnd(); ++it2)
        {
            Exiv2::ExifKey gpsKey((*it2).toAscii().constData());
            Exiv2::ExifData::iterator it3 = d->exifMetadata().findKey(gpsKey);
            if (it3 != d->exifMetadata().end())
                d->exifMetadata().erase(it3);
        }

#ifdef _XMP_SUPPORT_
        /** @todo The XMP spec does not mention Xmp.exif.GPSLongitudeRef,
         * and Xmp.exif.GPSLatitudeRef. But because we write them in setGPSInfo(),
         * we should also remove them here.
         */
        removeXmpTag("Xmp.exif.GPSLatitudeRef", false);
        removeXmpTag("Xmp.exif.GPSLongitudeRef", false);
        removeXmpTag("Xmp.exif.GPSVersionID", false);
        removeXmpTag("Xmp.exif.GPSLatitude", false);
        removeXmpTag("Xmp.exif.GPSLongitude", false);
        removeXmpTag("Xmp.exif.GPSAltitudeRef", false);
        removeXmpTag("Xmp.exif.GPSAltitude", false);
        removeXmpTag("Xmp.exif.GPSTimeStamp", false);
        removeXmpTag("Xmp.exif.GPSSatellites", false);
        removeXmpTag("Xmp.exif.GPSStatus", false);
        removeXmpTag("Xmp.exif.GPSMeasureMode", false);
        removeXmpTag("Xmp.exif.GPSDOP", false);
        removeXmpTag("Xmp.exif.GPSSpeedRef", false);
        removeXmpTag("Xmp.exif.GPSSpeed", false);
        removeXmpTag("Xmp.exif.GPSTrackRef", false);
        removeXmpTag("Xmp.exif.GPSTrack", false);
        removeXmpTag("Xmp.exif.GPSImgDirectionRef", false);
        removeXmpTag("Xmp.exif.GPSImgDirection", false);
        removeXmpTag("Xmp.exif.GPSMapDatum", false);
        removeXmpTag("Xmp.exif.GPSDestLatitude", false);
        removeXmpTag("Xmp.exif.GPSDestLongitude", false);
        removeXmpTag("Xmp.exif.GPSDestBearingRef", false);
        removeXmpTag("Xmp.exif.GPSDestBearing", false);
        removeXmpTag("Xmp.exif.GPSDestDistanceRef", false);
        removeXmpTag("Xmp.exif.GPSDestDistance", false);
        removeXmpTag("Xmp.exif.GPSProcessingMethod", false);
        removeXmpTag("Xmp.exif.GPSAreaInformation", false);
        removeXmpTag("Xmp.exif.GPSDifferential", false);
#endif // _XMP_SUPPORT_

        return true;
    }
    catch( Exiv2::Error& e )
    {
        d->printExiv2ExceptionError("Cannot remove Exif GPS tag using Exiv2 ", e);
    }

    return false;
}

void KExiv2::convertToRational(const double number, long int* const numerator, 
                               long int* const denominator, const int rounding)
{
    // This function converts the given decimal number
    // to a rational (fractional) number.
    //
    // Examples in comments use Number as 25.12345, Rounding as 4.

    // Split up the number.
    double whole      = trunc(number);
    double fractional = number - whole;

    // Calculate the "number" used for rounding.
    // This is 10^Digits - ie, 4 places gives us 10000.
    double rounder = pow(10.0, rounding);

    // Round the fractional part, and leave the number
    // as greater than 1.
    // To do this we: (for example)
    //  0.12345 * 10000 = 1234.5
    //  floor(1234.5) = 1234 - now bigger than 1 - ready...
    fractional = round(fractional * rounder);

    // Convert the whole thing to a fraction.
    // Fraction is:
    //     (25 * 10000) + 1234   251234
    //     ------------------- = ------ = 25.1234
    //           10000            10000
    double numTemp = (whole * rounder) + fractional;
    double denTemp = rounder;

    // Now we should reduce until we can reduce no more.

    // Try simple reduction...
    // if   Num
    //     ----- = integer out then....
    //      Den
    if (trunc(numTemp / denTemp) == (numTemp / denTemp))
    {
        // Divide both by Denominator.
        numTemp /= denTemp;
        denTemp /= denTemp;
    }

    // And, if that fails, brute force it.
    while (1)
    {
        // Jump out if we can't integer divide one.
        if ((numTemp / 2) != trunc(numTemp / 2)) break;
        if ((denTemp / 2) != trunc(denTemp / 2)) break;
        // Otherwise, divide away.
        numTemp /= 2;
        denTemp /= 2;
    }

    // Copy out the numbers.
    *numerator   = (int)numTemp;
    *denominator = (int)denTemp;
}

void KExiv2::convertToRationalSmallDenominator(const double number, long int* const numerator, long int* const denominator)
{
    // This function converts the given decimal number
    // to a rational (fractional) number.
    //
    // This method, in contrast to the method above, will retrieve the smallest possible
    // denominator. It is tested to retrieve the correct value for 1/x, with 0 < x <= 1000000.
    // Note: This requires double precision, storing in float breaks some numbers (49, 59, 86,...)

    // Split up the number.
    double whole      = trunc(number);
    double fractional = number - whole;

    /*
     * Find best rational approximation to a double
     * by C.B. Falconer, 2006-09-07. Released to public domain.
     *
     * Newsgroups: comp.lang.c, comp.programming
     * From: CBFalconer <cbfalconer@yahoo.com>
     * Date: Thu, 07 Sep 2006 17:35:30 -0400
     * Subject: Rational approximations
     */
    int lastnum = 500; // this is _not_ the largest possible denominator
    long int num, approx, bestnum=0, bestdenom=1;
    double   value, error, leasterr, criterion;

    value = fractional;

    if (value == 0.0)
    {
        *numerator   = (long int)whole;
        *denominator = 1;
        return;
    }

    criterion = 2 * value * DBL_EPSILON;
    for (leasterr = value, num = 1; num < lastnum; ++num)
    {
        approx = (int)(num / value + 0.5);
        error  = fabs((double)num / approx - value);
        if (error < leasterr)
        {
            bestnum   = num;
            bestdenom = approx;
            leasterr  = error;
            if (leasterr <= criterion) break;
        }
    }

    // add whole number part
    if (bestdenom * whole > (double)INT_MAX)
    {
        // In some cases, we would generate an integer overflow.
        // Fall back to Gilles's code which is better suited for such numbers.
        convertToRational(number, numerator, denominator, 5);
    }
    else
    {
        bestnum += bestdenom * (long int)whole;

        *numerator   = bestnum;
        *denominator = bestdenom;
    }
}

QString KExiv2::convertToGPSCoordinateString(const long int numeratorDegrees, const long int denominatorDegrees,
                                             const long int numeratorMinutes, const long int denominatorMinutes,
                                             const long int numeratorSeconds, long int denominatorSeconds,
                                             const char directionReference)
{
    /**
     * Precision:
     * A second at sea level measures 30m for our purposes, a minute 1800m.
     * (for more details, see http://en.wikipedia.org/wiki/Geographic_coordinate_system)
     * This means with a decimal precision of 8 for minutes we get +/-0,018mm.
     * (if I calculated correctly)
     */

    QString coordinate;

    // be relaxed with seconds of 0/0
    if (denominatorSeconds == 0 && numeratorSeconds == 0)
        denominatorSeconds = 1;

    if (denominatorDegrees == 1 &&
        denominatorMinutes == 1 &&
        denominatorSeconds == 1)
    {
        // use form DDD,MM,SSk
        coordinate = "%1,%2,%3%4";
        coordinate = coordinate.arg(numeratorDegrees).arg(numeratorMinutes).arg(numeratorSeconds).arg(directionReference);
    }
    else if (denominatorDegrees == 1 &&
             denominatorMinutes == 100 &&
             denominatorSeconds == 1)
    {
        // use form DDD,MM.mmk
        coordinate = "%1,%2%3";
        double minutes = (double)numeratorMinutes / (double)denominatorMinutes;
        minutes += (double)numeratorSeconds / 60.0;
        QString minutesString = QString::number(minutes, 'f', 8);
        while (minutesString.endsWith('0') && !minutesString.endsWith(".0"))
        {
            minutesString.chop(1);
        }
        coordinate = coordinate.arg(numeratorDegrees).arg(minutesString).arg(directionReference);
    }
    else if (denominatorDegrees == 0 ||
             denominatorMinutes == 0 ||
             denominatorSeconds == 0)
    {
        // Invalid. 1/0 is everything but 0. As is 0/0.
        return QString();
    }
    else
    {
        // use form DDD,MM.mmk
        coordinate = "%1,%2%3";
        double degrees = (double)numeratorDegrees / (double)denominatorDegrees;
        double wholeDegrees = trunc(degrees);
        double minutes = (double)numeratorMinutes / (double)denominatorMinutes;
        minutes += (degrees - wholeDegrees) * 60.0;
        minutes += ((double)numeratorSeconds / (double)denominatorSeconds) / 60.0;
        QString minutesString = QString::number(minutes, 'f', 8);
        while (minutesString.endsWith('0') && !minutesString.endsWith(".0"))
        {
            minutesString.chop(1);
        }
        coordinate = coordinate.arg((int)wholeDegrees).arg(minutesString).arg(directionReference);
    }
    return coordinate;
}

QString KExiv2::convertToGPSCoordinateString(const bool isLatitude, double coordinate)
{
    if (coordinate < -360.0 || coordinate > 360.0)
        return QString();

    QString coordinateString;

    char directionReference;

    if (isLatitude)
    {
        if (coordinate < 0)
            directionReference = 'S';
        else
            directionReference = 'N';
    }
    else
    {
        if (coordinate < 0)
            directionReference = 'W';
        else
            directionReference = 'E';
    }

    // remove sign
    coordinate = fabs(coordinate);

    int degrees = (int)floor(coordinate);
    // get fractional part
    coordinate = coordinate - (double)(degrees);
    // To minutes
    double minutes = coordinate * 60.0;

    // use form DDD,MM.mmk
    coordinateString = "%1,%2%3";
    coordinateString = coordinateString.arg(degrees);
    coordinateString = coordinateString.arg(minutes, 0, 'f', 8).arg(directionReference);
    return coordinateString;
}

bool KExiv2::convertFromGPSCoordinateString(const QString& gpsString,
                                            long int* const numeratorDegrees, long int* const denominatorDegrees,
                                            long int* const numeratorMinutes, long int* const denominatorMinutes,
                                            long int* const numeratorSeconds, long int* const denominatorSeconds,
                                            char* const directionReference)
{
    if (gpsString.isEmpty())
        return false;

    *directionReference = gpsString.at(gpsString.length() - 1).toUpper().toLatin1();
    QString coordinate = gpsString.left(gpsString.length() - 1);

    QStringList parts = coordinate.split(',');
    if (parts.size() == 2)
    {
        // form DDD,MM.mmk
        *denominatorDegrees = 1;
        *denominatorMinutes = 1000000;
        *denominatorSeconds = 1;

        *numeratorDegrees = parts[0].toLong();

        double minutes = parts[1].toDouble();
        minutes *= 1000000;

        *numeratorMinutes = (long)round(minutes);
        *numeratorSeconds = 0;
        return true;
    }
    else if (parts.size() == 3)
    {
        // use form DDD,MM,SSk
        *denominatorDegrees = 1;
        *denominatorMinutes = 1;
        *denominatorSeconds = 1;

        *numeratorDegrees = parts[0].toLong();
        *numeratorMinutes = parts[1].toLong();
        *numeratorSeconds = parts[2].toLong();
        return true;
    }
    else
        return false;
}

bool KExiv2::convertFromGPSCoordinateString(const QString& gpsString, double* const degrees)
{
    if (gpsString.isEmpty())
        return false;

    char directionReference = gpsString.at(gpsString.length() - 1).toUpper().toLatin1();
    QString coordinate      = gpsString.left(gpsString.length() - 1);

    QStringList parts = coordinate.split(',');
    if (parts.size() == 2)
    {
        // form DDD,MM.mmk
        *degrees =  parts[0].toLong();
        *degrees += parts[1].toDouble() / 60.0;

        if (directionReference == 'W' || directionReference == 'S')
            *degrees *= -1.0;

        return true;
    }
    else if (parts.size() == 3)
    {
        // use form DDD,MM,SSk

        *degrees =  parts[0].toLong();
        *degrees += parts[1].toLong() / 60.0;
        *degrees += parts[2].toLong() / 3600.0;

        if (directionReference == 'W' || directionReference == 'S')
            *degrees *= -1.0;

        return true;
    }
    else
        return false;
}

bool KExiv2::convertToUserPresentableNumbers(const QString& gpsString,
                                             int* const degrees, int* const minutes,
                                             double* const seconds, char* const directionReference)
{
    if (gpsString.isEmpty())
        return false;

    *directionReference = gpsString.at(gpsString.length() - 1).toUpper().toLatin1();
    QString coordinate = gpsString.left(gpsString.length() - 1);

    QStringList parts = coordinate.split(',');
    if (parts.size() == 2)
    {
        // form DDD,MM.mmk
        *degrees = parts[0].toInt();

        double fractionalMinutes = parts[1].toDouble();
        *minutes = (int)trunc(fractionalMinutes);
        *seconds = (fractionalMinutes - (double)(*minutes)) * 60.0;
        return true;
    }
    else if (parts.size() == 3)
    {
        // use form DDD,MM,SSk
        *degrees = parts[0].toInt();
        *minutes = parts[1].toInt();
        *seconds = (double)parts[2].toInt();
        return true;
    }
    else
    {
        return false;
    }
}

void KExiv2::convertToUserPresentableNumbers(const bool isLatitude, double coordinate,
                                             int* const degrees, int* const minutes,
                                             double* const seconds, char* const directionReference)
{
    if (isLatitude)
    {
        if (coordinate < 0)
            *directionReference = 'S';
        else
            *directionReference = 'N';
    }
    else
    {
        if (coordinate < 0)
            *directionReference = 'W';
        else
            *directionReference = 'E';
    }

    // remove sign
    coordinate = fabs(coordinate);
    *degrees = (int)floor(coordinate);
    // get fractional part
    coordinate = coordinate - (double)(*degrees);
    // To minutes
    coordinate *= 60.0;
    *minutes = (int)floor(coordinate);
    // get fractional part
    coordinate = coordinate - (double)(*minutes);
    // To seconds
    coordinate *= 60.0;
    *seconds = coordinate;
}

}  // NameSpace KExiv2Iface
