#pragma once

#include "GaoCe.hpp"
#include "GaoCeRunningUi.hpp"
#include <Eyestack/Design/Paramgr.hpp>
#include <Eyestack/Design/Progressor.hpp>
#include <Eyestack/Design/QuadrangleSelector.hpp>
#include <Eyestack/Design/RectangleSelector.hpp>
#include <Eyestack/Framework.hpp>

namespace esb = Eyestack::Base;
namespace esd = Eyestack::Design;

// ==========================================================================
// GaoCeUi_WarpPerspective
// ==========================================================================

/**
 * @brief 透视变换窗口
 */
class GaoCeUi_WarpPerspective : public QWidget
{
  Q_OBJECT

  using _T = GaoCeUi_WarpPerspective;
  using _S = QWidget;

public:
  /**
   * @brief 获取输入
   */
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  GaoCeUi_WarpPerspective(GaoCe::GaoCe& algo, QWidget* parent = nullptr);

private:
  GaoCe::GaoCe& _algo;
  QPushButton _inputButton, _outputButton;
  QSpinBox _widthSpin, _heightSpin, _ppgSpin;
  QWidget _leftWidget;
  QStackedLayout _leftLayout;
  esd::QuadrangleSelector _inputBench;
  esd::ImageBench _outputBench;

private slots:
  void on_inputButton_clicked();
  void on_outputButton_clicked();
};

// ==========================================================================
// GaoCeConfigUi_LearnBrightnessThreshold
// ==========================================================================

/**
 * @brief 学习背景阈值窗口
 */
class GaoCeConfigUi_LearnBrightnessThreshold : public QWidget
{
  Q_OBJECT

  using _T = GaoCeConfigUi_LearnBrightnessThreshold;
  using _S = QWidget;

public:
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  GaoCeConfigUi_LearnBrightnessThreshold(GaoCe::GaoCe& algo);

private:
  GaoCe::GaoCe& _algo;
  QPushButton _refreshButton, _resetButton, _learnButton;

  cv::Mat _current;
  esd::RectangleSelector _rectSelector;

private slots:
  void when_refreshButton_clicked();
  void when_resetButton_clicked();
  void when_learnButton_clicked();
};

// ==========================================================================
// GaoCeConfigUi_LearnRealRatio
// ==========================================================================

/**
 * @brief GaoCeConfigUi_LearnRealRatio 学习真实比例窗口
 */
class GaoCeConfigUi_LearnRealRatio : public QWidget
{
  Q_OBJECT

  using _T = GaoCeConfigUi_LearnRealRatio;
  using _S = QWidget;

public:
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  GaoCeConfigUi_LearnRealRatio(GaoCe::GaoCe& algo);

private:
  GaoCe::GaoCe& _algo;
  QPushButton _inputButton, _outputButton;
  QSpinBox _widthSpin, _heightSpin;
  QDoubleSpinBox _ppgSpin;
  QWidget _leftWidget;
  QStackedLayout _leftLayout;
  esd::QuadrangleSelector _inputBench;
  esd::ImageBench _outputBench;

  cv::Mat _current;

private slots:
  void on_inputButton_clicked();
  void on_outputButton_clicked();
};

// ==========================================================================
// GaoCeConfigUi_LearnPattern
// ==========================================================================

/**
 * @brief GaoCeConfigUi_LearnPattern 学习模板窗口
 */
class GaoCeConfigUi_LearnPattern : public QWidget
{
  Q_OBJECT

  using _T = GaoCeConfigUi_LearnPattern;
  using _S = QWidget;

public:
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  GaoCeConfigUi_LearnPattern(GaoCe::GaoCe& algo);

private:
  GaoCe::GaoCe& _algo;
  QPushButton _refreshButton, _resetButton, _learnButton;
  QSpinBox _stripeNumSpin;

  cv::Mat _current;
  esd::RectangleSelector _rectSelector;

private slots:
  void when_refreshButton_clicked();
  void when_resetButton_clicked();
  void when_learnButton_clicked();
};

// ==========================================================================
// GaoCeConfigUi_LearnThreshold
// ==========================================================================

/**
 * @brief GaoCeConfigUi_LearnThreshold 学习阈值窗口
 */
class GaoCeConfigUi_LearnThreshold : public QWidget
{
  Q_OBJECT

  using _T = GaoCeConfigUi_LearnThreshold;
  using _S = QWidget;

public:
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  /**
   * @brief GaoCeConfigUi_LearnThreshold
   */
  GaoCeConfigUi_LearnThreshold(GaoCe::GaoCe& algo);

private:
  GaoCe::GaoCe& _algo;
  QPushButton _refreshButton, _resetButton, _learnButton;

  cv::Mat _current;
  esd::RectangleSelector _rectSelector;

private slots:
  void when_refreshButton_clicked();
  void when_resetButton_clicked();
  void when_learnButton_clicked();
};

// ==========================================================================
// GaoCeConfigUi_InitDrpParams
// ==========================================================================

/**
 * @brief GaoCeConfigUi_InitParams 初始化参数窗口
 */
class GaoCeConfigUi_InitParams : public esd::Paramgr
{
  Q_OBJECT

  using _T = GaoCeConfigUi_InitParams;
  using _S = esd::Paramgr;

public:
  GaoCeConfigUi_InitParams(GaoCe::GaoCe& algo);
};
