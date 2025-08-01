// SPDX-FileCopyrightText: 2011-2012 Tasos Varoudis
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "dminterface/metagraphdm.hpp"

#include "salalib/genlib/comm.hpp"
#include "salalib/ianalysis.hpp"

#include <QApplication>
#include <QElapsedTimer>
#include <QMutex>
#include <QProgressDialog>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

#include <math.h>

QT_BEGIN_NAMESPACE

//! [0]
class RenderThread : public QThread {
    Q_OBJECT

  public:
    RenderThread(QObject *parent = 0);
    ~RenderThread();

    void *m_parent;
    bool simple_version;
    void render(void *param);

  signals:
    void renderedImage(const QImage &image, double scaleFactor);
    void runtimeExceptionThrown(int type, std::string message);
    void showWarningMessage(QString title, QString message);
    void closeWaitDialog();

  protected:
    void run() override;

  private:
    bool restart;
    bool abort;
    QMutex mutex;
    QWaitCondition condition;
};

class QGraphDoc; // forward declaration required by
                 // CMSCommunicator::run(QGraphDoc *)

class CMSCommunicator : public Communicator {
  public:
    enum {
        IMPORT,
        IMPORTMIF,
        MAKEPOINTS,
        MAKEGRAPH,
        ANALYSEGRAPH,
        POINTDEPTH,
        METRICPOINTDEPTH,
        ANGULARPOINTDEPTH,
        TOPOLOGICALPOINTDEPTH,
        MAKEISOVIST,
        MAKEISOVISTPATH,
        MAKEISOVISTSFROMFILE,
        MAKEALLLINEMAP,
        MAKEFEWESTLINEMAP,
        MAKEDRAWING,
        MAKEUSERMAP,
        MAKEUSERMAPSHAPE,
        MAKEUSERSEGMAP,
        MAKEUSERSEGMAPSHAPE,
        MAKEGATESMAP,
        MAKEBOUNDARYMAP,
        MAKESEGMENTMAP,
        MAKECONVEXMAP,
        AXIALANALYSIS,
        SEGMENTANALYSISTULIP,
        SEGMENTANALYSISANGULAR,
        TOPOMETANALYSIS,
        AGENTANALYSIS,
        FROMCONNECTOR
    };

  public:
    CMSCommunicator();
    virtual ~CMSCommunicator();
    virtual void CommPostMessage(size_t m,
                                 size_t x) const override; // Inline below CWaitDialog

    void *parent_doc;

    bool simple_version; // public is not a good thing but will do for now! // TV

    void SetFunction(int function) { m_function = function; }
    void setAnalysis(int function) { m_function = function; }
    int GetFunction() const { return m_function; }

    void SetOption(int option, size_t which = 0) {
        while (which >= m_options.size())
            m_options.push_back(-1);
        m_options[which] = option;
    }
    int GetOption(size_t which = 0) const {
        return (which >= m_options.size()) ? -1 : m_options[which];
    }
    void SetSeedPoint(const Point2f &p) { m_seed_point = p; }
    const Point2f &GetSeedPoint() const { return m_seed_point; }
    void SetSeedAngle(const double angle) { m_seed_angle = angle; }
    double GetSeedAngle() const { return m_seed_angle; }
    void SetSeedFoV(const double fov) { m_seed_fov = fov; }
    double GetSeedFoV() const { return m_seed_fov; }
    //

    void SetString(const QString &str) { m_string = str; }
    const QString &GetString() const { return m_string; }
    void SetFileSet(QStringList strings) {
        m_fileset.clear();
        for (int i = 0; i < strings.size(); i++) {
            m_fileset.push_back(strings[i].toStdString());
        }
    }
    void setAnalysis(std::unique_ptr<IAnalysis> &&analysis) { m_analysis = std::move(analysis); }
    std::unique_ptr<IAnalysis> &getAnalysis() { return m_analysis; }

    void setPostAnalysisFunc(
        std::function<void(std::unique_ptr<IAnalysis> &analysis, AnalysisResult &result)> func) {
        m_postAnalysisFunc = func;
    }
    void setSuccessUpdateFlags(int type, bool modified = true) {
        m_successUpdateFlagType = type;
        m_successUpdateFlagModified = modified;
    }
    void setSuccessRedrawFlags(int viewtype, int flag, int reason) {
        m_successRedrawFlagViewType = viewtype;
        m_successRedrawFlag = flag;
        m_successRedrawReason = reason;
    }

    void runAnalysis(QGraphDoc &graphDoc);

    void logError(const std::string &message) const override { std::cerr << message << std::endl; }
    void logWarning(const std::string &message) const override {
        std::cerr << message << std::endl;
    }
    void logInfo(const std::string &message) const override { std::cout << message << std::endl; }

  protected:
    std::vector<int> m_options;
    int m_function;
    Point2f m_seed_point;
    double m_seed_angle;
    double m_seed_fov;
    // CImportedModule m_module;
    QString m_string; // for a generic string
    std::unique_ptr<IAnalysis> m_analysis;
    std::function<void(std::unique_ptr<IAnalysis> &analysis, AnalysisResult &result)>
        m_postAnalysisFunc;
    int m_successUpdateFlagType;
    bool m_successUpdateFlagModified = true;
    int m_successRedrawFlagViewType;
    bool m_successRedrawFlag;
    int m_successRedrawReason;
};

struct QFilePath {
    QString m_path;
    QString m_name;
    QString m_ext;
    QFilePath(const QString &pathname) {
        int dot = pathname.lastIndexOf('.');
        int slash = pathname.lastIndexOf('\\');
        if (slash != -1) {
            m_path = pathname.left(slash + 1);
        } else {
            slash = pathname.lastIndexOf('/');
            if (slash != -1)
                m_path = pathname.left(slash + 1);
        }
        if (dot != -1) {
            m_name = pathname.mid(slash + 1, dot - slash - 1);
            m_ext = pathname.mid(dot + 1);
            m_ext = m_ext.toUpper();
        } else {
            m_name = pathname.mid(slash + 1);
        }
    }
};

class QGraphDoc : public QWidget {
    Q_OBJECT
  private:
    void exceptionThrownInRenderThread(int type, std::string message);
    void messageFromRenderThread(QString title, QString message);

    std::recursive_mutex mLock;

  public:
    QGraphDoc(const QString &author, const QString &organisation);
    CMSCommunicator *m_communicator;

    int m_make_algorithm;  // algorithm to make graph
    double m_make_maxdist; // maximum distance you can see (set to -1.0 for infinite)

    MetaGraphDM *m_meta_graph;

    QString m_base_title;
    QString m_opened_name;

    bool modifiedFlag;

    // Redraw commands
    enum { REDRAW_DONE, REDRAW_POINTS, REDRAW_GRAPH, REDRAW_TOTAL };
    // Redraw reasons
    enum {
        UNDECLARED,
        NEW_FOCUS,
        NEW_DEPTHMAPVIEW_SETUP,
        NEW_LINESET,
        NEW_DATA,
        NEW_SELECTION,
        NEW_TABLE,
        NEW_COLUMN,
        NEW_FILE,
        DELETED_TABLE
    };
    // Mainframe table changes:
    enum {
        CONTROLS_DESTROYALL,
        CONTROLS_LOADALL,
        CONTROLS_LOADGRAPH,
        CONTROLS_RELOADGRAPH,
        CONTROLS_LOADCONVERT,
        CONTROLS_LOADDRAWING,
        CONTROLS_LOADATTRIBUTES,
        CONTROLS_CHANGEATTRIBUTE
    };

    // Views attached (by viewtypes)
    enum {
        VIEW_ALL = 0,
        VIEW_MAP = 1,
        VIEW_SCATTER = 2,
        VIEW_TABLE = 3,
        VIEW_3D = 4,
        VIEW_MAP_GL = 5,
        VIEW_TYPES = 6
    };

    void *m_mainFrame;
    QWidget *m_view[VIEW_TYPES];

    // now individual to each view
    bool m_flag_lock;
    int m_redraw_flag[VIEW_TYPES];

    bool SetRedrawFlag(int viewtype, int flag, int reason = UNDECLARED,
                       QWidget *originator = NULL); // (almost) thread safe

    int GetRedrawFlag(int viewtype) const { return m_redraw_flag[viewtype]; }

    bool m_remenu_flag[VIEW_TYPES];
    void SetRemenuFlag(int viewtype, bool on) {
        if (viewtype) {
            m_remenu_flag[viewtype] = on;
        } else {
            for (int i = 1; i < VIEW_TYPES; i++) {
                m_remenu_flag[i] = on;
            }
        }
    }

    bool GetRemenuFlag(int viewtype) const { return m_remenu_flag[viewtype]; }

    void SetUpdateFlag(int type, bool modified = true);
    Point2f m_position; // Last known mouse position, in DXF units
    // Paths for the March 05 evolved agents
    // (loaded from file using the test button)
    std::vector<std::vector<Point2f>> m_evolved_paths;
    RenderThread m_thread;

    QProgressDialog *m_waitdlg;
    QString m_base_description;
    bool modify_prog;
    int Tid_progress;
    int m_num_steps;
    int m_record;
    int m_step;
    int m_num_records;
    QElapsedTimer m_timer;
    void ProcPostMessage(int m, int x);
    void UpdateMainframestatus();

    std::unique_lock<std::recursive_mutex> getLock() {
        return std::unique_lock<std::recursive_mutex>(mLock);
    }

    std::unique_lock<std::recursive_mutex> getLockDeferred() {
        return std::unique_lock<std::recursive_mutex>(mLock, std::defer_lock_t());
    }
    LayerManagerImpl &getLayers(int type = -1, std::optional<size_t> layer = std::nullopt);
    const LayerManagerImpl &getLayers(int type = -1,
                                      std::optional<size_t> layer = std::nullopt) const;

    AttributeTableHandle &getAttributeTableHandle(int type = -1,
                                                  std::optional<size_t> layer = std::nullopt);
    const AttributeTableHandle &
    getAttributeTableHandle(int type = -1, std::optional<size_t> layer = std::nullopt) const;

  public slots:
    void cancel_wait();

    // Operations
  public:
    void CreateWaitDialog(const QString &description, QWidget *pr = NULL);
    void DestroyWaitDialog();

    void OnFillPoints(const Point2f &p, int fill_type = 0);
    void OnMakeIsovist(const Point2f &seed, double angle = -1.0);
    void OnToolsAxialMap(const Point2f &seed);
    int RenameColumn(AttributeTable *tab, int col);
    bool ReplaceColumnContents(LatticeMapDM *latticemap, ShapeMapDM *shapemap, int col);
    bool SelectByQuery(LatticeMapDM *latticemap, ShapeMapDM *shapemap);
    void OnToolsTopomet();

    bool OnNewDocument();
    int OnSaveDocument(QString lpszPathName);
    int OnSaveDocument(QString lpszPathName, int version);
    bool OnCloseDocument(int);
    int OnOpenDocument(char *lpszPathName);
    void OnToolsTPD();
    void OnVGALinksFileImport();
    void OnGenerateIsovistsFromFile();
    void OnFileImport();
    void OnFileExport();
    void OnFileExportMapGeometry();
    void OnFileExportLinks();
    void OnAxialConnectionsExportAsDot();
    void OnAxialConnectionsExportAsPairCSV();
    void OnSegmentConnectionsExportAsPairCSV();
    void OnToolsMakeGraph();
    void OnToolsUnmakeGraph();
    void OnEditClear();
    void OnToolsRun();
    void OnEditUndo();
    void OnToolsPD();
    void OnPushToLayer();
    void OnEditGrid();
    void OnToolsMPD();
    void OnToolsMakeFewestLineMap();
    void OnToolsRunSeg();
    void OnAddColumn();
    void OnRemoveColumn();
    void OnFileProperties();
    void OnToolsAPD();
    void OnViewShowGrid();
    void OnToolsRunAxa();
    void OnSwapColours();
    void OnViewSummary();
    void OnToolsPointConvShapeMap();
    void OnToolsAxialConvShapeMap();
    void OnUpdateColumn();
    void OnToolsAgentRun();
    void OnRenameColumn();
    void OnEditQuery();
    void OnColumnProperties();
    void OnLayerNew();
    void OnLayerDelete();
    void OnLayerConvert();
    void OnLayerConvertDrawing();
    void OnEditSelectToLayer();
    void OnToolsIsovistpath();
    bool OnFileSave();
    bool OnFileSaveAs();
    void OnConvertMapShapes();
    void OnToolsLineLoadUnlinks();
    void OnLatticeMapExportConnectionsAsCSV();

  protected:
    virtual void timerEvent(QTimerEvent *event);
};

QT_END_NAMESPACE
