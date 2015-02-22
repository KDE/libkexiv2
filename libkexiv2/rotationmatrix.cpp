/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-08-03
 * @brief  Tools for combining rotation operations
 *
 * @author Copyright (C) 2006-2015 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2004-2012 by Marcel Wiesweg
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

#include "rotationmatrix.h"

// KDE includes

#include <kdebug.h>

// local includes

#include "version.h"

namespace KExiv2Iface
{

/**
   If the picture is displayed according to the exif orientation tag,
   the user will request rotating operations relative to what he sees,
   and that is the picture rotated according to the EXIF tag.
   So the operation requested and the given EXIF angle must be combined.
   E.g. if orientation is "6" (rotate 90 clockwiseto show correctly)
   and the user selects 180 clockwise, the operation is 270.
   If the user selected 270, the operation would be None (and clearing the exif tag).

   This requires to describe the transformations in a model which
   cares for both composing (180+90=270) and eliminating (180+180=no action),
   as well as the non-commutative nature of the operations (vflip+90 is not 90+vflip)

   All 2D transformations can be described by a 2x3 matrix, see QWRotationMatrix.
   All transformations needed here - rotate 90, 180, 270, flipV, flipH -
   can be described in a 2x2 matrix with the values 0,1,-1
   (because flipping is expressed by changing the sign only,
    and sine and cosine of 90, 180 and 270 are either 0,1 or -1).

    x' = m11 x + m12 y
    y' = m21 x + m22 y

   Moreover, all combinations of these rotate/flip operations result in one of the eight
   matrices defined below.
   (I did not proof that mathematically, but empirically)

   static const RotationMatrix identity;               //( 1,  0,  0,  1)
   static const RotationMatrix rotate90;               //( 0,  1, -1,  0)
   static const RotationMatrix rotate180;              //(-1,  0,  0, -1)
   static const RotationMatrix rotate270;              //( 0, -1,  1,  0)
   static const RotationMatrix flipHorizontal;         //(-1,  0,  0,  1)
   static const RotationMatrix flipVertical;           //( 1,  0,  0, -1)
   static const RotationMatrix rotate90flipHorizontal; //( 0,  1,  1,  0), first rotate, then flip
   static const RotationMatrix rotate90flipVertical;   //( 0, -1, -1,  0), first rotate, then flip

*/

namespace Matrix
{

static const RotationMatrix identity               ( 1,  0,  0,  1);
static const RotationMatrix rotate90               ( 0,  1, -1,  0);
static const RotationMatrix rotate180              (-1,  0,  0, -1);
static const RotationMatrix rotate270              ( 0, -1,  1,  0);
static const RotationMatrix flipHorizontal         (-1,  0,  0,  1);
static const RotationMatrix flipVertical           ( 1,  0,  0, -1);
static const RotationMatrix rotate90flipHorizontal ( 0,  1,  1,  0);
static const RotationMatrix rotate90flipVertical   ( 0, -1, -1,  0);

RotationMatrix matrix(RotationMatrix::TransformationAction action)
{
    switch (action)
    {
        case RotationMatrix::NoTransformation:
            return identity;
        case RotationMatrix::FlipHorizontal:
            return flipHorizontal;
        case RotationMatrix::FlipVertical:
            return flipVertical;
        case RotationMatrix::Rotate90:
            return rotate90;
        case RotationMatrix::Rotate180:
            return rotate180;
        case RotationMatrix::Rotate270:
            return rotate270;
    }

    return identity;
}

RotationMatrix matrix(KExiv2::ImageOrientation exifOrientation)
{
    switch (exifOrientation)
    {
        case KExiv2::ORIENTATION_NORMAL:
            return identity;
        case KExiv2::ORIENTATION_HFLIP:
            return flipHorizontal;
        case KExiv2::ORIENTATION_ROT_180:
            return rotate180;
        case KExiv2::ORIENTATION_VFLIP:
            return flipVertical;
        case KExiv2::ORIENTATION_ROT_90_HFLIP:
            return rotate90flipHorizontal;
        case KExiv2::ORIENTATION_ROT_90:
            return rotate90;
        case KExiv2::ORIENTATION_ROT_90_VFLIP:
            return rotate90flipVertical;
        case KExiv2::ORIENTATION_ROT_270:
            return rotate270;
        case KExiv2::ORIENTATION_UNSPECIFIED:
            return identity;
    }

    return identity;
}


} // namespace Matrix

RotationMatrix::RotationMatrix()
{
    set( 1, 0, 0, 1 );
}

RotationMatrix::RotationMatrix(TransformationAction action)
{
    *this = Matrix::matrix(action);
}

RotationMatrix::RotationMatrix(KExiv2::ImageOrientation exifOrientation)
{
    *this = Matrix::matrix(exifOrientation);
}

RotationMatrix::RotationMatrix(int m11, int m12, int m21, int m22)
{
    set(m11, m12, m21, m22);
}

void RotationMatrix::set(int m11, int m12, int m21, int m22)
{
    m[0][0]=m11;
    m[0][1]=m12;
    m[1][0]=m21;
    m[1][1]=m22;
}

bool RotationMatrix::isNoTransform() const
{
    return *this == Matrix::identity;
}

RotationMatrix& RotationMatrix::operator*=(const RotationMatrix& ma)
{
    set( ma.m[0][0]*m[0][0] + ma.m[0][1]*m[1][0],  ma.m[0][0]*m[0][1] + ma.m[0][1]*m[1][1],
         ma.m[1][0]*m[0][0] + ma.m[1][1]*m[1][0],  ma.m[1][0]*m[0][1] + ma.m[1][1]*m[1][1] );
         return *this;
}

bool RotationMatrix::operator==(const RotationMatrix& ma) const
{
    return m[0][0]==ma.m[0][0] &&
    m[0][1]==ma.m[0][1] &&
    m[1][0]==ma.m[1][0] &&
    m[1][1]==ma.m[1][1];
}

bool RotationMatrix::operator!=(const RotationMatrix& ma) const
{
    return !(*this==ma);
}

RotationMatrix& RotationMatrix::operator*=(TransformationAction action)
{
    return (*this *= Matrix::matrix(action));
}

RotationMatrix& RotationMatrix::operator*=(QList<TransformationAction> actions)
{
    foreach(const TransformationAction& action, actions)
    {
        *this *= Matrix::matrix(action);
    }

    return *this;
}

RotationMatrix& RotationMatrix::operator*=(KExiv2::ImageOrientation exifOrientation)
{
    return (*this *= Matrix::matrix(exifOrientation));
}

/** Converts the mathematically correct description
    into the primitive operations that can be carried out losslessly.
*/
QList<RotationMatrix::TransformationAction> RotationMatrix::transformations() const
{
    QList<TransformationAction> transforms;

    if (*this == Matrix::rotate90)
    {
        transforms << Rotate90;
    }
    else if (*this == Matrix::rotate180)
    {
        transforms << Rotate180;
    }
    else if (*this == Matrix::rotate270)
    {
        transforms << Rotate270;
    }
    else if (*this == Matrix::flipHorizontal)
    {
        transforms << FlipHorizontal;
    }
    else if (*this == Matrix::flipVertical)
    {
        transforms << FlipVertical;
    }
    else if (*this == Matrix::rotate90flipHorizontal)
    {
        //first rotate, then flip!
        transforms << Rotate90;
        transforms << FlipHorizontal;
    }
    else if (*this == Matrix::rotate90flipVertical)
    {
        //first rotate, then flip!
        transforms << Rotate90;
        transforms << FlipVertical;
    }
    return transforms;
}

KExiv2::ImageOrientation RotationMatrix::exifOrientation() const
{
    if (*this == Matrix::identity)
    {
        return KExiv2::ORIENTATION_NORMAL;
    }
    if (*this == Matrix::rotate90)
    {
        return KExiv2::ORIENTATION_ROT_90;
    }
    else if (*this == Matrix::rotate180)
    {
        return KExiv2::ORIENTATION_ROT_180;
    }
    else if (*this == Matrix::rotate270)
    {
        return KExiv2::ORIENTATION_ROT_270;
    }
    else if (*this == Matrix::flipHorizontal)
    {
        return KExiv2::ORIENTATION_HFLIP;
    }
    else if (*this == Matrix::flipVertical)
    {
        return KExiv2::ORIENTATION_VFLIP;
    }
    else if (*this == Matrix::rotate90flipHorizontal)
    {
        return KExiv2::ORIENTATION_ROT_90_HFLIP;
    }
    else if (*this == Matrix::rotate90flipVertical)
    {
        return KExiv2::ORIENTATION_ROT_90_VFLIP;
    }
    return KExiv2::ORIENTATION_UNSPECIFIED;
}

QMatrix RotationMatrix::toMatrix() const
{
    return toMatrix(exifOrientation());
}

QMatrix RotationMatrix::toMatrix(KExiv2::ImageOrientation orientation)
{
    QMatrix matrix;

    switch (orientation)
    {
        case KExiv2::ORIENTATION_NORMAL:
        case KExiv2::ORIENTATION_UNSPECIFIED:
            break;

        case KExiv2::ORIENTATION_HFLIP:
            matrix.scale(-1, 1);
            break;

        case KExiv2::ORIENTATION_ROT_180:
            matrix.rotate(180);
            break;

        case KExiv2::ORIENTATION_VFLIP:
            matrix.scale(1, -1);
            break;

        case KExiv2::ORIENTATION_ROT_90_HFLIP:
            matrix.scale(-1, 1);
            matrix.rotate(90);
            break;

        case KExiv2::ORIENTATION_ROT_90:
            matrix.rotate(90);
            break;

        case KExiv2::ORIENTATION_ROT_90_VFLIP:
            matrix.scale(1, -1);
            matrix.rotate(90);
            break;

        case KExiv2::ORIENTATION_ROT_270:
            matrix.rotate(270);
            break;
    }

    return matrix;
}

}  // namespace KExiv2Iface
