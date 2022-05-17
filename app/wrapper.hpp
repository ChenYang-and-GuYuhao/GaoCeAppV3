#pragma once

#include "GaoCeRunningUi.hpp"
#include "StatisReport.hpp"
#include "Worker.hpp"
#include "gao_ce_ui.hpp"
#include <Eyestack/Framework.hpp>

namespace esf = Eyestack::Framework;

/**
 * @brief 算法类
 */

class GaoCeWrapper : public GaoCe::GaoCe
{
public:
  std::unique_ptr<GaoCe> _gaoce;

  // GaoCe interface
public:
  virtual void learn_warp_perspective(const cv::Vec<cv::Point2f, 4>& corners,
                                      int width,
                                      int height,
                                      float pixelPerGrid) override;
  virtual cv::Mat warp_perspective(const cv::Mat& img) override;
  virtual Eyestack::Design::Paramgr::Table& param_table() override;
  virtual void learn_brightness_threshold(const cv::Mat& img,
                                          const cv::Rect& roi) override;
  virtual void learn_real_ratio(const cv::Mat& img,
                                const cv::Vec<cv::Point2f, 4>& corners,
                                int width,
                                int height,
                                float mmPerGrid) override;
  virtual void learn_pattern(const cv::Mat& img,
                             const cv::Rect& roi,
                             uint8_t T) override;
  virtual void learn_threshold(const cv::Mat& ori_img,
                               const cv::Rect& roiRegion) override;
  virtual void init_param() override;
  virtual std::vector<Result> detect(const cv::Mat& img) override;
  virtual void reset_config() noexcept override;
  virtual void dump_config(QDataStream& ds) noexcept(false) override;
  virtual void load_config(QDataStream& ds) noexcept(false) override;
};

/**
 * @brief 高测配置UI界面
 */
class GaoCeConfigSubUi : public esf::MainWindow::SubUi::MdiMenu
{
  Q_OBJECT

  using _T = GaoCeConfigSubUi;
  using _S = esf::MainWindow::SubUi::MdiMenu;

public:
  //  GaoCeUi_WarpPerspective _wp;
  //  QMdiSubWindow& _swWp;

  //  GaoCeConfigUi_LearnBrightnessThreshold _lbt;
  //  QMdiSubWindow& _swLbt;

  //  GaoCeConfigUi_LearnPattern _lp;
  //  QMdiSubWindow& _swLp;

  //  GaoCeConfigUi_LearnThreshold _lt;
  //  QMdiSubWindow& _swLt;

  //  GaoCeConfigUi_LearnRealRatio _lrr;
  //  QMdiSubWindow& _swLrr;

  //  GaoCeConfigUi_InitParams _ip;
  //  QMdiSubWindow& _swIp;

public:
  GaoCeConfigSubUi(Worker& worker);

private slots:
  void disable_all();
  void enable_all();
};

/**
 * @brief 高测运行监控 窗口界面
 */
class GaoCeRunningSubUi : public esf::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = GaoCeRunningSubUi;
  using _S = esf::MainWindow::SubUi::MdiAction;

public:
  GaoCeRunningSubUi(Worker& worker);

private:
  class RecordIter;

private:
  Worker& _worker;
  GaoCeRunningUi _ui;

  QFile _recordFile;
  QPushButton _reportButton;
  StatisReport _statisReport;

private slots:
  void when_display_results(
    ::cv::Mat img,
    QSharedPointer<std::vector<GaoCe::GaoCe::Result>> results);
  void when_noStopUpdated(bool checked);

  void when_worker_started();
  void when_worker_stopped();
  void when_worker_paused();

  void when_reportButton_clicked();
  void print_report(QPrinter* printer);

  // MainWindow::SubUi interface
public:
  virtual void setup_ui(Eyestack::Framework::MainWindow& mw) noexcept override;
};

/**
 * @brief 高测配置向导类
 */
class GaoCeConfigWizard : public QWizard
{
  using _T = GaoCeConfigWizard;
  using _S = QWizard;

public:
  GaoCeConfigWizard(GaoCeConfigSubUi& configUi);

private:
  GaoCeConfigSubUi& _configUi;
  QWizardPage welcomPage;
  QWizardPage wpPage;
  QWizardPage lbtPage;
  QWizardPage lrrPage;
  QWizardPage lpPage;
  QWizardPage ltPage;
  QWizardPage idpPage;

  // QWidget interface
protected:
  virtual void showEvent(QShowEvent* event) override;
  virtual void hideEvent(QHideEvent* event) override;
};
