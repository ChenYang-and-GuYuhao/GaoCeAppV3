#include "wrapper.hpp"

GaoCeConfigSubUi::GaoCeConfigSubUi(Worker& worker)
  : _S("算法")
{
  connect(&worker,
          &esf::Worker::s_started,
          this,
          &_T::disable_all,
          Qt::BlockingQueuedConnection);
  connect(&worker,
          &esf::Worker::s_paused,
          this,
          &_T::disable_all,
          Qt::BlockingQueuedConnection);
  connect(&worker,
          &esf::Worker::s_stopped,
          this,
          &_T::enable_all,
          Qt::BlockingQueuedConnection);
}

void
GaoCeConfigSubUi::disable_all()
{
  //  _wp.setEnabled(false);
  //  _lbt.setEnabled(false);
  //  _lp.setEnabled(false);
  //  _lt.setEnabled(false);
  //  _lrr.setEnabled(false);
  //  _ip.setEnabled(false);

  esf::Application::main_window()._menuBar._config._export.setEnabled(false);
}

void
GaoCeConfigSubUi::enable_all()
{
  //  _wp.setEnabled(true);
  //  _lbt.setEnabled(true);
  //  _lp.setEnabled(true);
  //  _lt.setEnabled(true);
  //  _lrr.setEnabled(true);
  //  _ip.setEnabled(true);

  esf::Application::main_window()._menuBar._config._export.setEnabled(true);
}

GaoCeRunningSubUi::GaoCeRunningSubUi(Worker& worker)
  : _S("运行监控")
  , _worker(worker)
  , _recordFile(esf::Application::logsys().manifest().logPath + "/records")
{
  setWidget(&_ui);

  connect(&_ui, &GaoCeRunningUi::noStopUpdated, this, &_T::when_noStopUpdated);

  connect(
    &worker, &Worker::s_displayGaoCeResults, this, &_T::when_display_results);

  connect(&worker,
          &Worker::s_started,
          this,
          &_T::when_worker_started,
          Qt::BlockingQueuedConnection);
  connect(&worker,
          &Worker::s_stopped,
          this,
          &_T::when_worker_stopped,
          Qt::BlockingQueuedConnection);
  connect(&worker,
          &Worker::s_paused,
          this,
          &_T::when_worker_paused,
          Qt::BlockingQueuedConnection);

  _reportButton.setText(tr("打印报告"));
  connect(&_reportButton,
          &QPushButton::clicked,
          this,
          &_T::when_reportButton_clicked);
}

void
GaoCeRunningSubUi::when_display_results(
  ::cv::Mat img,
  QSharedPointer<std::vector<GaoCe::GaoCe::Result>> results)
{
  _ui.display_results(img, results);

  if (_recordFile.isOpen()) {
    QDataStream ds(&_recordFile);
    ds << QDateTime::currentDateTime();

    ds << results->size();
    for (auto&& result : *results) {
      ds << result._rect.x << result._rect.y << result._rect.width
         << result._rect.height;
      ds << result._area << result._circumference << result._maxLen;
    }
  }
}

void
GaoCeRunningSubUi::when_noStopUpdated(bool checked)
{
  _worker._noStop.store(checked);
}

void
GaoCeRunningSubUi::when_worker_started()
{
  _recordFile.open(QFile::Append);
  _reportButton.setEnabled(false); // TODO
}

void
GaoCeRunningSubUi::when_worker_stopped()
{
  _recordFile.close();
  _reportButton.setEnabled(true);
}

void
GaoCeRunningSubUi::when_worker_paused()
{
  _reportButton.setEnabled(false);
}

class GaoCeRunningSubUi::RecordIter : public StatisReport::RecordIter
{
public:
  RecordIter(QDataStream& ds, QDateTime begin, QDateTime end)
    : _ds(ds)
    , _begin(begin)
    , _end(end)
  {}

public:
  virtual bool next(StatisReport::Record* record_) noexcept override
  {
    auto& record = *record_;

    if (_atEnd)
      return false;

    do {
      _ds >> record._timestamp;
      if (_ds.status() != QDataStream::Ok) {
        _atEnd = true;
        return false;
      }

      decltype(record._results)::size_type size;
      _ds >> size;

      record._results.resize(size);
      for (auto& result : record._results) {
        _ds >> result._rect.x >> result._rect.y >> result._rect.width >>
          result._rect.height;
        _ds >> result._area >> result._circumference >> result._maxLen;
      }
    } while (!(record._timestamp >= _begin && record._timestamp < _end));

    return true;
  }

private:
  QDataStream& _ds;
  QDateTime _begin, _end;
  bool _atEnd{ false };
};

void
GaoCeRunningSubUi::when_reportButton_clicked()
{
  struct OpenFileLock
  {
    QFile& _file;
    bool _ok;
    OpenFileLock(QFile& file)
      : _file(file)
      , _ok(file.open(QFile::ReadOnly))
    {}
    ~OpenFileLock() { _file.close(); }
  } openFileLock(_recordFile);

  if (!openFileLock._ok) {
    esf::Application::notifier().notify_text("统计数据文件打开失败");
    return;
  }

  QDialog intervalDialog(this);
  QFormLayout formLayout(&intervalDialog);

  auto now = QDateTime::currentDateTime();
  QDateTimeEdit beginDTEdit(now), endDTEdit(now);
  formLayout.addRow(tr("开始时间"), &beginDTEdit);
  formLayout.addRow(tr("结束时间"), &endDTEdit);

  QDialogButtonBox _dlgButBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  formLayout.addRow(&_dlgButBox);
  connect(&_dlgButBox,
          &QDialogButtonBox::accepted,
          &intervalDialog,
          &QDialog::accept);
  connect(&_dlgButBox,
          &QDialogButtonBox::rejected,
          &intervalDialog,
          &QDialog::reject);

  if (!intervalDialog.exec())
    return;

  if (!(beginDTEdit.dateTime() < endDTEdit.dateTime())) {
    QMessageBox::critical(this, "日期区间无效", "开始时间必须小于结束时间");
    return;
  }

  QDataStream ds(&_recordFile);
  RecordIter ri(ds, beginDTEdit.dateTime(), endDTEdit.dateTime());
  _statisReport.set_statistics(ri);
  _recordFile.close();

  QPrintPreviewDialog dlg;
  connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &_T::print_report);
  dlg.exec();
}

void
GaoCeRunningSubUi::print_report(QPrinter* printer)
{
  _statisReport.print(*printer);
}

void
GaoCeRunningSubUi::setup_ui(Eyestack::Framework::MainWindow& mw) noexcept
{
  _S::setup_ui(mw);
  mw._workctrlTools.addWidget(&_reportButton);
}

GaoCeConfigWizard::GaoCeConfigWizard(GaoCeConfigSubUi& configUi)
  : _configUi(configUi)
{
  welcomPage.setTitle("欢迎使用配置向导");
  welcomPage.setLayout(new QVBoxLayout());
  addPage(&welcomPage);

  wpPage.setTitle("透视变换矫正");
  wpPage.setLayout(new QVBoxLayout());
  addPage(&wpPage);

  lbtPage.setTitle("学习亮度阈值");
  lbtPage.setLayout(new QVBoxLayout());
  addPage(&lbtPage);

  lrrPage.setTitle("学习真实比例");
  lrrPage.setLayout(new QVBoxLayout());
  addPage(&lrrPage);

  lpPage.setTitle("学习模板");
  lpPage.setLayout(new QVBoxLayout());
  addPage(&lpPage);

  ltPage.setTitle("学习参数");
  ltPage.setLayout(new QVBoxLayout());
  addPage(&ltPage);

  idpPage.setTitle("配置运行时参数");
  idpPage.setLayout(new QVBoxLayout());
  addPage(&idpPage);
}

void
GaoCeConfigWizard::showEvent(QShowEvent* event)
{
  // 显示的时候从各个 MdiSubUi 上把界面借过来

  //  wpPage.layout()->addWidget(&_configUi._wp);
  //  _configUi._wp.show();
  //  lbtPage.layout()->addWidget(&_configUi._lbt);
  //  _configUi._lbt.show();
  //  lrrPage.layout()->addWidget(&_configUi._lrr);
  //  _configUi._lrr.show();
  //  lpPage.layout()->addWidget(&_configUi._lp);
  //  _configUi._lp.show();
  //  ltPage.layout()->addWidget(&_configUi._lt);
  //  _configUi._lt.show();
  //  idpPage.layout()->addWidget(&_configUi._ip);
  //  _configUi._ip.show();

  //  _configUi._swWp.setWidget(nullptr);
  //  _configUi._swWp.hide();
  //  _configUi._swLbt.setWidget(nullptr);
  //  _configUi._swLbt.hide();
  //  _configUi._swLp.setWidget(nullptr);
  //  _configUi._swLp.hide();
  //  _configUi._swLt.setWidget(nullptr);
  //  _configUi._swLt.hide();
  //  _configUi._swLrr.setWidget(nullptr);
  //  _configUi._swLrr.hide();
  //  _configUi._swIp.setWidget(nullptr);
  //  _configUi._swIp.hide();

  _S::showEvent(event);
}

void
GaoCeConfigWizard::hideEvent(QHideEvent* event)
{
  // 关闭时再把各个配置界面还回去

  _S::hideEvent(event);

  //  _configUi._swWp.setWidget(&_configUi._wp);
  //  _configUi._swLbt.setWidget(&_configUi._lbt);
  //  _configUi._swLp.setWidget(&_configUi._lp);
  //  _configUi._swLt.setWidget(&_configUi._lt);
  //  _configUi._swLrr.setWidget(&_configUi._lrr);
  //  _configUi._swIp.setWidget(&_configUi._ip);
}

void
GaoCeWrapper::learn_warp_perspective(const cv::Vec<cv::Point2f, 4>& corners,
                                     int width,
                                     int height,
                                     float pixelPerGrid)
{
  try {
    return _gaoce->learn_warp_perspective(corners, width, height, pixelPerGrid);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }
}

cv::Mat
GaoCeWrapper::warp_perspective(const cv::Mat& img)
{
  try {
    return _gaoce->warp_perspective(img);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }

  return cv::Mat();
}

Eyestack::Design::Paramgr::Table&
GaoCeWrapper::param_table()
{
  return _gaoce->param_table();
}

void
GaoCeWrapper::learn_brightness_threshold(const cv::Mat& img,
                                         const cv::Rect& roi)
{
  try {
    return _gaoce->learn_brightness_threshold(img, roi);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }
}

void
GaoCeWrapper::learn_real_ratio(const cv::Mat& img,
                               const cv::Vec<cv::Point2f, 4>& corners,
                               int width,
                               int height,
                               float mmPerGrid)
{
  try {
    return _gaoce->learn_real_ratio(img, corners, width, height, mmPerGrid);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }
}

void
GaoCeWrapper::learn_pattern(const cv::Mat& img, const cv::Rect& roi, uint8_t T)
{
  try {
    return _gaoce->learn_pattern(img, roi, T);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }
}

void
GaoCeWrapper::learn_threshold(const cv::Mat& ori_img, const cv::Rect& roiRegion)
{
  try {
    return _gaoce->learn_threshold(ori_img, roiRegion);

  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "高测算法");
  }
}

void
GaoCeWrapper::init_param()
{
  return _gaoce->init_param();
}

std::vector<GaoCe::GaoCe::Result>
GaoCeWrapper::detect(const cv::Mat& img)
{
  //  try {
  return _gaoce->detect(img);

  //  } catch (...) {
  //    esf::Application::notifier().notify_error(std::current_exception(),
  //                                              "高测算法");
  //  }

  //  return std::vector<GaoCe::GaoCe::Result>();
}

void
GaoCeWrapper::reset_config() noexcept
{
  return _gaoce->reset_config();
}

void
GaoCeWrapper::dump_config(QDataStream& ds) noexcept(false)
{
  return _gaoce->dump_config(ds);
}

void
GaoCeWrapper::load_config(QDataStream& ds) noexcept(false)
{
  return _gaoce->load_config(ds);
}
