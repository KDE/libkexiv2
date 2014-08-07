/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-10-15
 * @brief  IPTC subjects editor.
 *
 * @author Copyright (C) 2006-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2009-2012 by Andi Clemens
 *         <a href="mailto:andi dot clemens at googlemail dot com">andi dot clemens at googlemail dot com</a>
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

#ifndef SUBJECTWIDGET_H
#define SUBJECTWIDGET_H

// Qt includes

#include <QtGui/QButtonGroup>
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

// KDE includes

#include <kurl.h>
#include <klineedit.h>

// Local includes

#include "libkexiv2_export.h"

namespace KExiv2Iface
{

class KEXIV2_EXPORT SubjectData
{
public:

    SubjectData(const QString& n, const QString& m, const QString& d)
    {
        name   = n;
        matter = m;
        detail = d;
    }

    QString name;         // English and Ascii Name of subject.
    QString matter;       // English and Ascii Matter Name of subject.
    QString detail;       // English and Ascii Detail Name of subject.
};

// --------------------------------------------------------------------------------

class KEXIV2_EXPORT SubjectWidget : public QWidget
{
    Q_OBJECT

public:

    SubjectWidget(QWidget* parent);
    ~SubjectWidget();

    void setSubjectsList(const QStringList& list);
    QStringList subjectsList() const;

Q_SIGNALS:

    void signalModified();

protected Q_SLOTS:

    virtual void slotSubjectsToggled(bool);
    virtual void slotRefChanged();
    virtual void slotEditOptionChanged(int);
    virtual void slotSubjectSelectionChanged();
    virtual void slotAddSubject();
    virtual void slotDelSubject();
    virtual void slotRepSubject();

protected:

    virtual bool loadSubjectCodesFromXML(const KUrl& url);
    virtual QString buildSubject() const;

protected:

    QLabel*    m_note;

    QCheckBox* m_subjectsCheck;

    KLineEdit* m_iprEdit;
    KLineEdit* m_refEdit;
    KLineEdit* m_nameEdit;
    KLineEdit* m_matterEdit;
    KLineEdit* m_detailEdit;

private:

    class Private;
    Private* const d;
};

}  // namespace KExiv2Iface

#endif // SUBJECTWIDGET_H
