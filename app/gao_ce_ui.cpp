#include "gao_ce_ui.hpp"

using namespace Eyestack::Base::util;

// ==========================================================================
// GaoCeUi_WarpPerspective
// ==========================================================================

GaoCeUi_WarpPerspective::GaoCeUi_WarpPerspective(GaoCe::GaoCe& algo,
                                                 QWidget* parent)
  : _S(parent)
  , _algo(algo)
  , _inputButton("刷新输入")
  , _outputButton("计算输出")
{
  // 配置组件
  _leftWidget.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  _widthSpin.setPrefix("宽");
  _widthSpin.setSuffix("格");
  _heightSpin.setPrefix("高");
  _heightSpin.setSuffix("格");
  _ppgSpin.setPrefix("每格");
  _ppgSpin.setSuffix("像素");
  _ppgSpin.setRange(1, 10000);

  // 布局界面
  _leftWidget.setLayout(&_leftLayout);
  _leftLayout.addWidget(&_inputBench);
  _leftLayout.addWidget(&_outputBench);

  auto vlayout = new QVBoxLayout();
  vlayout->addStretch();
  vlayout->addWidget(&_inputButton);
  vlayout->addSpacing(20);
  vlayout->addWidget(&_widthSpin);
  vlayout->addWidget(&_heightSpin);
  vlayout->addWidget(&_ppgSpin);
  vlayout->addStretch();
  vlayout->addWidget(&_outputButton);

  auto hlayout = new QHBoxLayout();
  hlayout->addWidget(&_leftWidget);
  hlayout->addLayout(vlayout);
  setLayout(hlayout);

  // 连接信号
  connect(
    &_inputButton, &QPushButton::clicked, this, &_T::on_inputButton_clicked);
  connect(
    &_outputButton, &QPushButton::clicked, this, &_T::on_outputButton_clicked);

  // 初始化状态
  _leftLayout.setCurrentIndex(0);
}

void
GaoCeUi_WarpPerspective::on_inputButton_clicked()
{
  _leftLayout.setCurrentIndex(0);
  _inputBench.display(_getInput());
}

void
GaoCeUi_WarpPerspective::on_outputButton_clicked()
{
  try {
    _algo.learn_warp_perspective(_inputBench.get_quadrangle(),
                                 _widthSpin.value(),
                                 _heightSpin.value(),
                                 _ppgSpin.value());

    auto out = _algo.warp_perspective(qimage_as_cvmat(_inputBench.current()));
    _outputBench.display(out);
    _leftLayout.setCurrentIndex(1);

  } catch (cv::Exception& e) {
    QMessageBox::critical(this, "错误", e.what());
  }
}

// ==========================================================================
// GaoCeConfigUi_LearnBrightnessThreshold
// ==========================================================================

GaoCeConfigUi_LearnBrightnessThreshold::GaoCeConfigUi_LearnBrightnessThreshold(
  GaoCe::GaoCe& algo)
  : _algo(algo)
{
  // 配置控件
  _refreshButton.setText(tr("刷新画面"));
  _resetButton.setText(tr("重置选择"));
  _learnButton.setText("学习参数");

  // 布局界面
  auto vLayout = new QVBoxLayout();
  vLayout->addWidget(&_refreshButton);
  vLayout->addWidget(&_resetButton);
  vLayout->addWidget(&_learnButton);

  auto hLayout = new QHBoxLayout();
  hLayout->addWidget(&_rectSelector);
  hLayout->addLayout(vLayout);

  setLayout(hLayout);

  // 连接信号
  connect(&_refreshButton,
          &QPushButton::clicked,
          this,
          &_T::when_refreshButton_clicked);
  connect(
    &_resetButton, &QPushButton::clicked, this, &_T::when_resetButton_clicked);
  connect(
    &_learnButton, &QPushButton::clicked, this, &_T::when_learnButton_clicked);
}

void
GaoCeConfigUi_LearnBrightnessThreshold::when_refreshButton_clicked()
{
  auto mat = _getInput();

  if (mat.type() != CV_8UC1) {
    QMessageBox::critical(
      this, tr("错误"), tr("数据源提供的图片类型不是 CV_8UC1"));
    return;
  }

  _rectSelector.display(mat);
  _current = mat;
}

void
GaoCeConfigUi_LearnBrightnessThreshold::when_resetButton_clicked()
{
  _rectSelector.reset_selection();
}

void
GaoCeConfigUi_LearnBrightnessThreshold::when_learnButton_clicked()
{
  if (_current.empty()) {
    QMessageBox::critical(this, tr("错误"), tr("没有图片输入"));
    return;
  }

  esd::Progressor::exec(
    [this](esd::Progressor& pg) {
      _algo.learn_brightness_threshold(_current, _rectSelector.get_cv_rect());
    },
    "学习参数中……");

  QMessageBox::information(this, tr("成功"), tr("参数学习完毕"));
}

// ==========================================================================
// GaoCeConfigUi_LearnRealRatio
// ==========================================================================

GaoCeConfigUi_LearnRealRatio::GaoCeConfigUi_LearnRealRatio(GaoCe::GaoCe& algo)
  : _algo(algo)
  , _inputButton("刷新输入")
  , _outputButton("计算输出")
{
  // 配置组件
  _leftWidget.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  _widthSpin.setPrefix("宽");
  _widthSpin.setSuffix("格");
  _heightSpin.setPrefix("高");
  _heightSpin.setSuffix("格");
  _ppgSpin.setPrefix("每格");
  _ppgSpin.setSuffix("mm");
  _ppgSpin.setSingleStep(0.1);

  // 布局界面
  _leftWidget.setLayout(&_leftLayout);
  _leftLayout.addWidget(&_inputBench);
  _leftLayout.addWidget(&_outputBench);

  auto vlayout = new QVBoxLayout();
  vlayout->addStretch();
  vlayout->addWidget(&_inputButton);
  vlayout->addSpacing(20);
  vlayout->addWidget(&_widthSpin);
  vlayout->addWidget(&_heightSpin);
  vlayout->addWidget(&_ppgSpin);
  vlayout->addStretch();
  vlayout->addWidget(&_outputButton);

  auto hlayout = new QHBoxLayout();
  hlayout->addWidget(&_leftWidget);
  hlayout->addLayout(vlayout);
  setLayout(hlayout);

  // 连接信号
  connect(
    &_inputButton, &QPushButton::clicked, this, &_T::on_inputButton_clicked);
  connect(
    &_outputButton, &QPushButton::clicked, this, &_T::on_outputButton_clicked);

  // 初始化状态
  _leftLayout.setCurrentIndex(0);
}

void
GaoCeConfigUi_LearnRealRatio::on_inputButton_clicked()
{
  _leftLayout.setCurrentIndex(0);
  _current = _getInput();
  _inputBench.display(_current);
}

void
GaoCeConfigUi_LearnRealRatio::on_outputButton_clicked()
{
  try {
    _algo.learn_real_ratio(_current,
                           _inputBench.get_quadrangle(),
                           _widthSpin.value(),
                           _heightSpin.value(),
                           _ppgSpin.value());

    QMessageBox::information(this, tr("成功"), tr("参数学习完毕"));

  } catch (cv::Exception& e) {
    QMessageBox::critical(this, "错误", e.what());
  }
}

// ==========================================================================
// GaoCeConfigUi_LearnPattern
// ==========================================================================

GaoCeConfigUi_LearnPattern::GaoCeConfigUi_LearnPattern(GaoCe::GaoCe& algo)
  : _algo(algo)
{
  // 配置控件
  _refreshButton.setText(tr("刷新画面"));
  _resetButton.setText(tr("重置选择"));
  _learnButton.setText("学习参数");
  _stripeNumSpin.setPrefix(tr("条纹"));
  _stripeNumSpin.setSuffix(tr("道"));

  // 布局界面
  auto vLayout = new QVBoxLayout();
  vLayout->addWidget(&_refreshButton);
  vLayout->addWidget(&_resetButton);
  vLayout->addWidget(&_stripeNumSpin);
  vLayout->addWidget(&_learnButton);

  auto hLayout = new QHBoxLayout();
  hLayout->addWidget(&_rectSelector);
  hLayout->addLayout(vLayout);

  setLayout(hLayout);

  // 连接信号
  connect(&_refreshButton,
          &QPushButton::clicked,
          this,
          &_T::when_refreshButton_clicked);
  connect(
    &_resetButton, &QPushButton::clicked, this, &_T::when_resetButton_clicked);
  connect(
    &_learnButton, &QPushButton::clicked, this, &_T::when_learnButton_clicked);
}

void
GaoCeConfigUi_LearnPattern::when_refreshButton_clicked()
{
  auto mat = _getInput();

  if (mat.type() != CV_8UC1) {
    QMessageBox::critical(
      this, tr("错误"), tr("数据源提供的图片类型不是 CV_8UC1"));
    return;
  }

  _rectSelector.display(mat);
  _current = mat;
}

void
GaoCeConfigUi_LearnPattern::when_resetButton_clicked()
{
  _rectSelector.reset_selection();
}

void
GaoCeConfigUi_LearnPattern::when_learnButton_clicked()
{
  if (_current.empty()) {
    QMessageBox::critical(this, tr("错误"), tr("没有图片输入"));
    return;
  }

  esd::Progressor::exec(
    [this](esd::Progressor& pg) {
      _algo.learn_pattern(_current,
                          _rectSelector.get_cv_rect(),
                          _current.cols / _stripeNumSpin.value());
    },
    tr("学习参数中……"));

  QMessageBox::information(this, tr("成功"), tr("参数学习完毕"));
}

// ==========================================================================
// GaoCeConfigUi_LearnThreshold
// ==========================================================================

GaoCeConfigUi_LearnThreshold::GaoCeConfigUi_LearnThreshold(GaoCe::GaoCe& algo)
  : _algo(algo)
{
  // 配置控件
  _refreshButton.setText(tr("刷新画面"));
  _resetButton.setText(tr("重置选择"));
  _learnButton.setText("学习参数");

  // 布局界面
  auto vLayout = new QVBoxLayout();
  vLayout->addWidget(&_refreshButton);
  vLayout->addWidget(&_resetButton);
  vLayout->addWidget(&_learnButton);

  auto hLayout = new QHBoxLayout();
  hLayout->addWidget(&_rectSelector);
  hLayout->addLayout(vLayout);

  setLayout(hLayout);

  // 连接信号
  connect(&_refreshButton,
          &QPushButton::clicked,
          this,
          &_T::when_refreshButton_clicked);
  connect(
    &_resetButton, &QPushButton::clicked, this, &_T::when_resetButton_clicked);
  connect(
    &_learnButton, &QPushButton::clicked, this, &_T::when_learnButton_clicked);
}

void
GaoCeConfigUi_LearnThreshold::when_refreshButton_clicked()
{
  auto mat = _getInput();

  if (mat.type() != CV_8UC1) {
    QMessageBox::critical(
      this, tr("错误"), tr("数据源提供的图片类型不是 CV_8UC1"));
    return;
  }

  _rectSelector.display(mat);
  _current = mat;
}

void
GaoCeConfigUi_LearnThreshold::when_resetButton_clicked()
{
  _rectSelector.reset_selection();
}

void
GaoCeConfigUi_LearnThreshold::when_learnButton_clicked()
{
  if (_current.empty()) {
    QMessageBox::critical(this, tr("错误"), tr("没有图片输入"));
    return;
  }

  esd::Progressor::exec(
    [this](esd::Progressor& pg) {
      _algo.learn_threshold(_current, _rectSelector.get_cv_rect());
    },
    tr("学习参数中……"));

  QMessageBox::information(this, tr("成功"), tr("参数学习完毕"));
}

GaoCeConfigUi_InitParams::GaoCeConfigUi_InitParams(GaoCe::GaoCe& algo)
{
  set_table(algo.param_table());
}
