#include "Worker.hpp"
#include "version.hpp"
#include "wrapper.hpp"

Q_DECLARE_METATYPE(QSharedPointer<std::vector<GaoCe::GaoCe::Result>>);

class GaoCeConfig : public Eyestack::Framework::Configurer::Config
{
public:
  GaoCeConfig(GaoCe::GaoCe& gaoce)
    : _gaoce(gaoce)
  {}

  // Config interface
public:
  virtual void reset_config() noexcept override;
  virtual void dump_config(QDataStream& ds) noexcept(false) override;
  virtual void load_config(QDataStream& ds) noexcept(false) override;

private:
  GaoCe::GaoCe& _gaoce;
};

bool
use_vc()
{
  auto& app = *esf::Application::instance();
  for (auto&& i : app.arguments()) {
    if (i == "--use-vc")
      return true;
  }
  return false;
}

int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);

  app.setApplicationName("高测系统 (v" GaoCeAppV3_VERSION ")");

  // 框架组件使用前需要初始化
  es::Gencom::ginit();
  es::Com_MVS::ginit();

  qRegisterMetaType<QSharedPointer<std::vector<GaoCe::GaoCe::Result>>>();

  auto worker = new Worker();
  // Wrapper 算法类
  auto wrapper = std::make_unique<GaoCeWrapper>();
  wrapper->_gaoce = std::move(GaoCe::create_v1());
  worker->_algo = std::move(wrapper);
  app.reg_worker(worker);

  GaoCeConfig gaoceConfig(*worker->_algo);
  app.reg_config(0x1231421, gaoceConfig);

  std::function<cv::Mat()> getInput;

  auto useVc = use_vc();
  if (useVc) {
    getInput = [worker]() {
      cv::Mat mat;
      worker->_vc.read(&mat);
      cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
      return mat;
    };

  } else {
    getInput = [worker]() -> cv::Mat {
      es::Com_MVS::MvsCameraWrapper::Shared mc;
      bool colored;
      bool timeout;

      if (QThread::currentThread() == worker->thread()) {
        mc = worker->_mc.camera();
        colored = worker->_mc.colored();
        timeout = worker->_mc.timeout();
      } else {
        QMetaObject::invokeMethod(
          worker,
          [worker, &mc, &colored, &timeout]() {
            mc = worker->_mc.camera();
            colored = worker->_mc.colored();
            timeout = worker->_mc.timeout();
          },
          Qt::BlockingQueuedConnection);
      }

      if (!mc)
        return cv::Mat();

      try {
        return mc->snap_cvmat(timeout, colored);

      } catch (const es::Com_MVS::MvsError& e) {
        esf::Application::notifier().notify_text(
          e.repr(), QtMsgType::QtCriticalMsg, "获取图像失败");
      }

      return cv::Mat();
    };
  }

  worker->_getInput = getInput;

  esg::VideoCapture::SubUi _vcSubUi{ worker->_vc, "视频\n捕获器" };
  _vcSubUi.setWindowTitle("视频捕获器");
  app.reg_sub_ui(_vcSubUi);

  es::Com_MVS::MvsCamera::SubUi _mcUi{ worker->_mc, "相机输入" };
  app.reg_sub_ui(_mcUi);

  GaoCeConfigSubUi _cSubUi{ *worker };
  {
    // TODO
  }
  app.reg_sub_ui(_cSubUi);

  GaoCeRunningSubUi _rSubUi{ *worker };
  app.reg_sub_ui(_rSubUi);

  GaoCeConfigWizard _wizard{ _cSubUi };
  app.reg_config_wizard(_wizard);

  if (useVc) {
    _mcUi.setEnabled(false);
  } else {
    _vcSubUi.setEnabled(false);
  }

  return app.exec();
}

void
GaoCeConfig::reset_config() noexcept
{
  _gaoce.reset_config();
}

void
GaoCeConfig::dump_config(QDataStream& ds) noexcept(false)
{
  _gaoce.dump_config(ds);
}

void
GaoCeConfig::load_config(QDataStream& ds) noexcept(false)
{
  _gaoce.load_config(ds);
}
