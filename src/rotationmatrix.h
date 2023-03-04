/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2004-2012 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef LIBKEXIV2_ROTATIONMATRIX_H
#define LIBKEXIV2_ROTATIONMATRIX_H

// Local includes

#include "kexiv2.h"
#include "libkexiv2_export.h"

// Qt includes
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#if KEXIV2_ENABLE_DEPRECATED_SINCE(5, 1)
#include <QMatrix>
#endif
#endif
#include <QTransform>

namespace KExiv2Iface
{

/**
 * @class RotationMatrix rotationmatrix.h <KExiv2/RotationMatrix>
 *
 * RotationMatrix
 */
class LIBKEXIV2_EXPORT RotationMatrix
{

public:

    /** This describes single transform primitives.
     *  Note some of the defined Exif rotation flags combine
     *  two of these actions.
     *  The enum values correspond to those defined
     *  as JXFORM_CODE in the often used the JPEG tool transupp.h.
     */
    enum TransformationAction
    {
        NoTransformation = 0, /// no transformation
        FlipHorizontal   = 1, /// horizontal flip
        FlipVertical     = 2, /// vertical flip
        Rotate90         = 5, /// 90-degree clockwise rotation
        Rotate180        = 6, /// 180-degree rotation
        Rotate270        = 7  /// 270-degree clockwise (or 90 ccw)
    };

public:

    /// Constructs the identity matrix (the matrix describing no transformation)
    RotationMatrix();
    /// Returns the matrix corresponding to the given TransformationAction
    RotationMatrix(TransformationAction action);
    /// Returns the matrix corresponding to the given TransformationAction
    RotationMatrix(KExiv2::ImageOrientation exifOrientation);

    bool operator==(const RotationMatrix& ma) const;
    bool operator!=(const RotationMatrix& ma) const;

    /// Returns true of this matrix describes no transformation (is the identity matrix)
    bool isNoTransform() const;

    RotationMatrix& operator*=(const RotationMatrix& ma);

    /// Applies the given transform to this matrix
    RotationMatrix& operator*=(TransformationAction action);

    /// Applies the given transform actions to this matrix
    RotationMatrix& operator*=(QList<TransformationAction> actions);

    /// Applies the given Exif orientation flag to this matrix
    RotationMatrix& operator*=(KExiv2::ImageOrientation exifOrientation);

    /** Returns the actions described by this matrix. The order matters.
     *  Not all possible matrices are supported, but all those that can be combined
     *  by Exif rotation flags and the transform actions above.
     *  If isNoTransform() or the matrix is not supported returns an empty list. */
    QList<TransformationAction> transformations() const;

    /** Returns the Exif orienation flag describing this matrix.
     *  Returns ORIENTATION_UNSPECIFIED if no flag matches this matrix.
     */
    KExiv2::ImageOrientation exifOrientation() const;

   /**
     * Returns a QTransform representing this matrix
     * @since 5.1
     */
    QTransform toTransform() const;

    /**
     * Returns a QTransform for the given Exif orientation
     * @since 5.1
     */
    static QTransform toTransform(KExiv2::ImageOrientation orientation);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#if KEXIV2_ENABLE_DEPRECATED_SINCE(5, 1)
    /// Returns a QMatrix representing this matrix
    /// @deprecated Since 5.1, use toTransform().
    KEXIV2_DEPRECATED_VERSION(5, 1, "Use toTransform()")
    QMatrix toMatrix() const;
#endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#if KEXIV2_ENABLE_DEPRECATED_SINCE(5, 1)
    /// Returns a QMatrix for the given Exif orientation
    KEXIV2_DEPRECATED_VERSION(5, 1, "Use toTransform(KExiv2::ImageOrientation)")
    static QMatrix toMatrix(KExiv2::ImageOrientation orientation);
#endif
#endif

    RotationMatrix(int m11, int m12, int m21, int m22);

protected:

    void set(int m11, int m12, int m21, int m22);

protected:

    int m[2][2];
};

}  // namespace KExiv2Iface

#endif  // LIBKEXIV2_ROTATIONMATRIX_H
