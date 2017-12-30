/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "ConfigDialog.h"
#include <QFile>
#include <QLayout>
#include <QPushButton>
//#include "CaptureConfigPage.h"
//#include "DecoderConfigPage.h"
//#include "AVFormatConfigPage.h"
//#include "AVFilterConfigPage.h"
#include "WebConfigPage.h"
#include "MiscPage.h"
//#include "ShaderPage.h"
#include "common/Config.h"
#include <QMessageBox>
#include <QDebug>

void ConfigDialog::display()
{
    static ConfigDialog *dialog = new ConfigDialog();
    dialog->show();
}

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Settings..."));
    QVBoxLayout *vbl = new QVBoxLayout();
    setLayout(vbl);
    vbl->setSizeConstraint(QLayout::SetFixedSize);

    mpContent = new QTabWidget();
    mpContent->setTabPosition(QTabWidget::West);
    mpContent->setMinimumHeight(350);

    mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
    mpButtonBox->addButton(tr("Reset"), QDialogButtonBox::ResetRole);// (QDialogButtonBox::Reset);
    mpButtonBox->addButton(tr("Ok"), QDialogButtonBox::AcceptRole); //QDialogButtonBox::Ok
    mpButtonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    mpButtonBox->addButton(tr("Apply"), QDialogButtonBox::ApplyRole);

    connect(mpButtonBox, SIGNAL(accepted()), this ,SLOT(accept()));
    connect(mpButtonBox, SIGNAL(rejected()), this ,SLOT(reject()));
    connect(mpButtonBox, SIGNAL(clicked(QAbstractButton*)), this ,SLOT(onButtonClicked(QAbstractButton*)));

    vbl->addWidget(mpContent);
    vbl->addWidget(mpButtonBox);

    mPages
           << new MiscPage()
//           << new CaptureConfigPage()
//           << new DecoderConfigPage()
//           << new AVFormatConfigPage()
//           << new AVFilterConfigPage()
           << new WebConfigPage()
//           << new ShaderPage()
              ;
    foreach (ConfigPageBase* page, mPages) {
        page->applyToUi();
        page->applyOnUiChange(false);
        mpContent->addTab(page, page->name());
    }
}

void ConfigDialog::onButtonClicked(QAbstractButton *btn)
{
    qDebug("QDialogButtonBox clicked role=%d", mpButtonBox->buttonRole(btn));
    switch (mpButtonBox->buttonRole(btn)) {
    case QDialogButtonBox::ResetRole:
        qDebug("QDialogButtonBox ResetRole clicked");
        onReset();
        break;
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::ApplyRole:
        qDebug("QDialogButtonBox ApplyRole clicked");
        onApply();
        break;
    case QDialogButtonBox::DestructiveRole:
    case QDialogButtonBox::RejectRole:
        onCancel();
        break;
    default:
        break;
    }
}

void ConfigDialog::onReset()
{
    Config::instance().reset();
    // TODO: check change
    int ret=QMessageBox::information(this,tr("Reset all settings"),tr("Reset will delete all data"),
                                  QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel);
    if (ret!=QMessageBox::Ok) return;

    Config::instance().reset();

    foreach (ConfigPageBase* page, mPages) {
        page->reset();
    }
}

void ConfigDialog::onApply()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->apply();
    }
    Config::instance().save();
}

void ConfigDialog::onCancel()
{
    // TODO: check change
    foreach (ConfigPageBase* page, mPages) {
        page->cancel();
    }
}

