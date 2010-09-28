/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-06-15
 * @brief  multi-languages string editor
 *
 * @author Copyright (C) 2009-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#ifndef ALTLANGSTREDIT_H
#define ALTLANGSTREDIT_H

// Qt includes

#include <QtGui/QWidget>
#include <QtCore/QString>

// Local includes.

#include "libkexiv2_export.h"
#include "kexiv2.h"

namespace KExiv2Iface
{

class KEXIV2_EXPORT AltLangStrEdit : public QWidget
{
    Q_OBJECT

public:

    AltLangStrEdit(QWidget* parent);
    ~AltLangStrEdit();

    void setTitle(const QString& title);
    void setClickMessage(const QString& msg);

    void setValues(const KExiv2::AltLangMap& values);
    KExiv2::AltLangMap& values();

    QString currentLanguageCode() const;
    QString languageCode(int index) const;

    QString defaultAltLang() const;
    bool    asDefaultAltLang() const;

    /**
     * Reset widget, clear all entries
     */
    void reset();

    /**
     * Ensure that the current language is added to the list of entries,
     * even if the text is empty.
     * signalValueAdded() will be emitted.
     */
    void addCurrent();

Q_SIGNALS:

    /**
     * Emitted when the user changes the text for the current language.
     */
    void signalModified(const QString& lang, const QString& text);
    /// Emitted when the current language changed
    void signalSelectionChanged(const QString& lang);
    /// Emitted when an entry for a new language is added
    void signalValueAdded(const QString& lang, const QString& text);
    /// Emitted when the entry for a language is removed.
    void signalValueDeleted(const QString& lang);

protected Q_SLOTS:

    void slotTextChanged();
    void slotSelectionChanged();
    void slotDeleteValue();

protected:

    void loadLangAltListEntries();

private:

    class AltLangStrEditPriv;
    AltLangStrEditPriv* const d;
};

}  // namespace KExiv2Iface

#endif // ALTLANGSTREDIT_H
