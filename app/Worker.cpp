#include "Worker.hpp"

void
Worker::c_start(qint64 cycle)
{
  try {
    _algo->init_param();
  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "运行启动失败");
    return;
  }

  _S::c_start(cycle);
}

void
Worker::c_trigger()
{
  try {
    _algo->init_param();
  } catch (...) {
    esf::Application::notifier().notify_error(std::current_exception(),
                                              "运行启动失败");
    return;
  }

  _S::c_trigger();
}

void
Worker::work()
{
  auto mat = _getInput();

  if (mat.empty()) {
    esf::Application::notifier().notify_text("无法从数据源读出图像",
                                             QtMsgType::QtCriticalMsg);
    c_stop();
    return;
  }

  if (mat.type() != CV_8UC1) {
    esf::Application::notifier().notify_text(
      tr("数据源色彩模式错误: %1")
        .arg(QString::fromStdString(cv::typeToString(mat.type()))),
      QtMsgType::QtCriticalMsg);
    c_stop();
    return;
  }

  mat = _algo->warp_perspective(mat);
  auto results = _algo->detect(mat);

  bool noStop = _noStop.load(std::memory_order_relaxed);
  if (results.size() != 0 && !noStop)
    c_stop();

  emit s_displayGaoCeResults(
    mat,
    QSharedPointer<std::vector<GaoCe::GaoCe::Result>>::create(
      std::move(results)));
}
