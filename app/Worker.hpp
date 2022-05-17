#pragma once

#include <Eyestack/Com_MVS/MvsCamera.hpp>
#include <Eyestack/Framework.hpp>
#include <Eyestack/Gencom.hpp>
#include <GaoCe.hpp>

namespace es = Eyestack;
namespace esf = Eyestack::Framework;
namespace esg = Eyestack::Gencom;

/**
 * @brief 工作对象类
 */
class Worker : public esf::Worker
{
  Q_OBJECT

  using _S = esf::Worker;

public:
  std::function<cv::Mat()> _getInput;

  std::unique_ptr<GaoCe::GaoCe> _algo;
  esg::VideoCapture _vc{ *this };
  es::Com_MVS::MvsCamera _mc{ *this };
  std::atomic_bool _noStop{ false };

signals:
  void s_displayGaoCeResults(
    ::cv::Mat _t1,
    QSharedPointer<std::vector<GaoCe::GaoCe::Result>> _t2);

  // Worker interface
protected:
  virtual void c_start(qint64 cycle) override;
  virtual void c_trigger() override;
  virtual void work() override;
};
