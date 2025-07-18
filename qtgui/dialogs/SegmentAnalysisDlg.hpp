// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ui_SegmentAnalysisDlg.h"

#include "dminterface/metagraphdm.hpp"

#include "salalib/radiustype.hpp"

class CSegmentAnalysisDlg : public QDialog, public Ui::CSegmentAnalysisDlg {
    Q_OBJECT
  public:
    CSegmentAnalysisDlg(MetaGraphDM *graph = NULL, QWidget *parent = 0);
    int m_analysis_type;
    QString m_radius;
    int m_tulip_bins;
    RadiusType m_radius_type;
    bool m_choice;
    bool m_weighted;
    int m_attribute;
    void UpdateData(bool value);
    void showEvent(QShowEvent *event);

    MetaGraphDM *m_meta_graph;

  private slots:
    void OnAnalysisType(bool);
    void OnAnalysisTulip(bool);
    void OnUpdateRadius(QString);
    void OnWeighted(bool);
    void OnOK();
    void OnCancel();
};
