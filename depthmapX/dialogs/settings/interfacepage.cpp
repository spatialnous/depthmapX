// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "interfacepage.hpp"

#include <QColorDialog>
#include <QtWidgets>

InterfacePage::InterfacePage(Settings &settings, QWidget *parent) : SettingsPage(settings, parent) {
    readSettings(settings);

    QGroupBox *generalGroup = new QGroupBox(tr("General"));
    QCheckBox *legacyMapCheckBox = new QCheckBox(tr("Legacy map window as default"));
    legacyMapCheckBox->setChecked(m_defaultMapWindowIsLegacy);
    connect(legacyMapCheckBox, &QCheckBox::stateChanged,
            [=]() { m_defaultMapWindowIsLegacy = !m_defaultMapWindowIsLegacy; });
    QCheckBox *hoverCheckBox = new QCheckBox(tr("Allow highlighting shapes on hover"));
    hoverCheckBox->setChecked(m_highlightOnHover);
    connect(hoverCheckBox, &QCheckBox::stateChanged,
            [=]() { m_highlightOnHover = !m_highlightOnHover; });

    QVBoxLayout *generalLayout = new QVBoxLayout;
    generalLayout->addWidget(legacyMapCheckBox);
    generalLayout->addWidget(hoverCheckBox);
    generalGroup->setLayout(generalLayout);

    QGroupBox *interfaceColoursGroup = new QGroupBox(tr("Interface colours"));
    QListWidget *interfaceColoursList = new QListWidget;
    connect(interfaceColoursList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
            SLOT(onInterfaceColourlItemClicked(QListWidgetItem *)));

    QListWidgetItem *bgItem = new QListWidgetItem(interfaceColoursList);
    bgItem->setText(tr("Background"));
    QPixmap pixmap(20, 20);
    pixmap.fill(m_background);
    QIcon bgColourIcon(pixmap);
    bgItem->setIcon(bgColourIcon);
    colourMap.insert(std::pair<QListWidgetItem *, QColor *>(bgItem, &m_background));

    QListWidgetItem *fgItem = new QListWidgetItem(interfaceColoursList);
    fgItem->setText(tr("Foreground"));
    pixmap.fill(m_foreground);
    QIcon fgColourIcon(pixmap);
    fgItem->setIcon(fgColourIcon);
    colourMap.insert(std::pair<QListWidgetItem *, QColor *>(fgItem, &m_foreground));

    QVBoxLayout *interfaceColoursLayout = new QVBoxLayout;
    interfaceColoursLayout->addWidget(interfaceColoursList);
    interfaceColoursGroup->setLayout(interfaceColoursLayout);
    QGroupBox *glOptionsGroup = new QGroupBox(tr("OpenGL view options"));

    QLabel *samplesLabel = new QLabel(tr("Number of antialising samples:"));
    QComboBox *samplesCombo = new QComboBox;
    samplesCombo->addItem(tr("0 (fastest)"), 0);
    samplesCombo->addItem(tr("2"), 2);
    samplesCombo->addItem(tr("4"), 4);
    samplesCombo->addItem(tr("8"), 8);
    samplesCombo->addItem(tr("16 (prettiest)"), 16);
    samplesCombo->setToolTip(tr("This will make lines smoother if higher, "
                                "but also the overall rendering slower. "
                                "To see the change close and reopen the file"));

    int index = samplesCombo->findData(m_antialiasingSamples);
    if (index != -1) {
        samplesCombo->setCurrentIndex(index);
    }

    connect(samplesCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            [=](int index) { m_antialiasingSamples = samplesCombo->itemData(index).toInt(); });

    QHBoxLayout *samplesLayout = new QHBoxLayout;
    samplesLayout->addWidget(samplesLabel);
    samplesLayout->addWidget(samplesCombo);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(samplesLayout);
    glOptionsGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(generalGroup);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(interfaceColoursGroup);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(glOptionsGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void InterfacePage::onInterfaceColourlItemClicked(QListWidgetItem *item) {
    QColor colour = QColorDialog::getColor();
    colourMap[item]->setRed(colour.red());
    colourMap[item]->setGreen(colour.green());
    colourMap[item]->setBlue(colour.blue());
    QPixmap pixmap(100, 100);
    pixmap.fill(colour);
    QIcon bgColourIcon(pixmap);
    item->setIcon(bgColourIcon);
}
