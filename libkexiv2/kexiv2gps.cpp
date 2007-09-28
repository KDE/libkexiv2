/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface for KDE
 *               GPS manipulation methods
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// C++ includes.

#include <climits>

// Local includes.

#include "kexiv2private.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

bool KExiv2::getGPSInfo(double& altitude, double& latitude, double& longitude) const
{
    try
    {    
        double num, den, min, sec;
        latitude=0.0, longitude=0.0, altitude=0.0;
        
        // Get the reference in first.

        QByteArray latRef = getExifTagData("Exif.GPSInfo.GPSLatitudeRef");
        if (latRef.isEmpty()) return false;

        QByteArray lngRef = getExifTagData("Exif.GPSInfo.GPSLongitudeRef");
        if (lngRef.isEmpty()) return false;

        QByteArray altRef = getExifTagData("Exif.GPSInfo.GPSAltitudeRef");

        // Latitude decoding.

        Exiv2::ExifKey exifKey("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifData exifData(d->exifMetadata);
        Exiv2::ExifData::iterator it = exifData.findKey(exifKey);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            latitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                latitude = latitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                latitude = latitude + sec/3600.0;
        }
        else 
            return false;
        
        if (latRef[0] == 'S') latitude *= -1.0;
    
        // Longitude decoding.

        Exiv2::ExifKey exifKey2("Exif.GPSInfo.GPSLongitude");
        it = exifData.findKey(exifKey2);
        if (it != exifData.end())
        {
            num = (double)((*it).toRational(0).first);
            den = (double)((*it).toRational(0).second);
            longitude = num/den;

            num = (double)((*it).toRational(1).first);
            den = (double)((*it).toRational(1).second);
            min = num/den;
            if (min != -1.0)
                longitude = longitude + min/60.0;

            num = (double)((*it).toRational(2).first);
            den = (double)((*it).toRational(2).second);
            sec = num/den;
            if (sec != -1.0)
                longitude = longitude + sec/3600.0;
        }
        else 
            return false;
        
        if (lngRef[0] == 'W') longitude *= -1.0;

        // Altitude decoding.

        if (!altRef.isEmpty()) 
        {
            Exiv2::ExifKey exifKey3("Exif.GPSInfo.GPSAltitude");
            it = exifData.findKey(exifKey3);
            if (it != exifData.end())
            {
                num = (double)((*it).toRational(0).first);
                den = (double)((*it).toRational(0).second);
                altitude = num/den;
            }
        
            if (altRef[0] == '1') altitude *= -1.0;
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot get Exif GPS tag using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::setGPSInfo(double altitude, double latitude, double longitude, bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {    
        // In first, we need to clean up all existing GPS info.
        removeGPSInfo();

        char scratchBuf[100];
        long int nom, denom;
        long int deg, min;
        
        // Do all the easy constant ones first.
        // GPSVersionID tag: standard says is should be four bytes: 02 00 00 00
        // (and, must be present).
        Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("2 0 0 0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSVersionID"), value.get());

        // Datum: the datum of the measured data. If not given, we insert WGS-84.
        d->exifMetadata["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
        
        // Now start adding data.

        // ALTITUDE.
        // Altitude reference: byte "00" meaning "sea level".
        value = Exiv2::Value::create(Exiv2::unsignedByte);
        value->read("0");
        d->exifMetadata.add(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"), value.get());
        
        // And the actual altitude.
        convertToRational(altitude, &nom, &denom, 4);
        snprintf(scratchBuf, 100, "%ld/%ld", nom, denom);
        d->exifMetadata["Exif.GPSInfo.GPSAltitude"] = scratchBuf;

        // LATTITUDE
        // Latitude reference: "N" or "S".
        if (latitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Southern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "S";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Northern hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLatitudeRef"] = "N";
        }
        
        // Now the actual lattitude itself.
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
        d->exifMetadata["Exif.GPSInfo.GPSLatitude"] = scratchBuf;
        
        // LONGITUDE
        // Longitude reference: "E" or "W".
        if (longitude < 0)
        {
            // Less than Zero: ie, minus: means
            // Western hemisphere.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "W";
        } 
        else 
        {
            // More than Zero: ie, plus: means
            // Eastern hemisphere. Where I live.
            d->exifMetadata["Exif.GPSInfo.GPSLongitudeRef"] = "E";
        }

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
        d->exifMetadata["Exif.GPSInfo.GPSLongitude"] = scratchBuf; 
    
        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot set Exif GPS tag using Exiv2 ", e);
    }        
    
    return false;
}

bool KExiv2::removeGPSInfo(bool setProgramName)
{
    if (!setProgramId(setProgramName))
        return false;

    try
    {  
        QStringList gpsTagsKeys;

        for (Exiv2::ExifData::iterator it = d->exifMetadata.begin();
             it != d->exifMetadata.end(); ++it)
        {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key.section(".", 1, 1) == QString("GPSInfo"))
                gpsTagsKeys.append(key);
        }

        for(QStringList::Iterator it2 = gpsTagsKeys.begin(); it2 != gpsTagsKeys.end(); ++it2)
        {
            Exiv2::ExifKey gpsKey((*it2).toAscii().constData());
            Exiv2::ExifData::iterator it3 = d->exifMetadata.findKey(gpsKey);
            if (it3 != d->exifMetadata.end())
                d->exifMetadata.erase(it3);
        }

        return true;
    }
    catch( Exiv2::Error &e )
    {
        printExiv2ExceptionError("Cannot remove Exif GPS tag using Exiv2 ", e);
    }

    return false;
}

void KExiv2::convertToRational(double number, long int* numerator, 
                               long int* denominator, int rounding)
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
    double rounder = pow(10, rounding);

    // Round the fractional part, and leave the number
    // as greater than 1.
    // To do this we: (for example)
    //  0.12345 * 10000 = 1234.5
    //  floor(1234.5) = 1234 - now bigger than 1 - ready...
    fractional = trunc(fractional * rounder);

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

void KExiv2::convertToRationalSmallDenominator(double number, long int* numerator, long int* denominator)
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
    for (leasterr = value, num = 1; num < lastnum; num++) {
        approx = (int)(num / value + 0.5);
        error  = fabs((double)num / approx - value);
        if (error < leasterr) {
            bestnum = num;
            bestdenom = approx;
            leasterr = error;
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

}  // NameSpace KExiv2Iface
