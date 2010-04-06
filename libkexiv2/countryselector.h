/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-07-07
 * Description : country selector combo-box.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef COUNTRY_SELECTOR_H
#define COUNTRY_SELECTOR_H

// Qt includes

#include <QtCore/QString>
#include <QtGui/QWidget>

// KDE includes

#include <kcombobox.h>

// Local includes.

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class CountrySelectorPriv;

class KEXIV2_EXPORT CountrySelector : public KComboBox
{

public:

    CountrySelector(QWidget* parent);
    ~CountrySelector();

    void setCountry(const QString& countryCode);
    bool country(QString& countryCode, QString& countryName);

    static QString countryForCode(const QString& countryCode);

private:

    CountrySelectorPriv* const d;
};

}  // namespace KExiv2Iface

#endif // COUNTRY_SELECTOR_H
