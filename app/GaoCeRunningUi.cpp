#include "GaoCeRunningUi.hpp"
#include "version.hpp"
#include <Eyestack/Design/ImageLabel.hpp>

GaoCeRunningUi::GaoCeRunningUi(QWidget* parent)
  : QWidget(parent)
{
  // 配置组件
  _monitor.set_power(true);
  _resultLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);
  _resultLabel.setLineWidth(5);
  _printButton.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  // 布局界面
  auto mainLayout = new QHBoxLayout(this);

  {
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setHorizontalStretch(2);
    _monitor.setSizePolicy(sp);
    _monitor.setMinimumWidth(300);
  }
  mainLayout->addWidget(&_monitor);

  auto rightWidget = new QWidget();
  {
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setHorizontalStretch(1);
    rightWidget->setSizePolicy(sp);
    rightWidget->setMinimumWidth(300);
  }
  mainLayout->addWidget(rightWidget);

  auto rightLayout = new QVBoxLayout(rightWidget);

  {
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setVerticalStretch(1);
    _resultLabel.setSizePolicy(sp);
  }
  rightLayout->addWidget(&_resultLabel);

  auto middleLayout = new QHBoxLayout();
  {
    _noStopCheck.setText(tr("不间断运行"));
    _printButton.setText(tr("打印缺陷报告"));
    _snapButton.setText(tr("保存图像"));

    middleLayout->addWidget(&_totalLabel);
    middleLayout->addStretch();
    middleLayout->addWidget(&_snapButton);
    middleLayout->addWidget(&_noStopCheck);
    middleLayout->addWidget(&_printButton);

    connect(&_noStopCheck, &QCheckBox::toggled, this, &_T::noStopUpdated);
    connect(&_printButton,
            &QPushButton::clicked,
            this,
            &_T::when_printButton_clicked);
    connect(&_snapButton, &QPushButton::clicked, this, [this]() {
      if (_currentImage.empty()) {
        QMessageBox::critical(this, tr("保存图像"), tr("当前没有图像"));
        return;
      }

      auto fileName =
        QFileDialog::getSaveFileName(this,
                                     tr("保存图像"),
                                     tr("无标题.png"),
                                     "PNG 格式 (*.png);;JPG 格式 (*.jpg)");
      if (fileName.isNull())
        return;

      if (cv::imwrite(fileName.toStdString(), _currentImage)) {
        QMessageBox::information(
          this, tr("保存图像"), tr("图像保存至 %1").arg(fileName));
      } else {
        QMessageBox::critical(this, tr("保存图像"), tr("图像保存失败"));
      }
    });
  }
  rightLayout->addLayout(middleLayout);

  {
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setVerticalStretch(3);
    _resultList.setSizePolicy(sp);
  }
  rightLayout->addWidget(&_resultList);

  // 初始化状态
  set_status_ready();
}

void
GaoCeRunningUi::display_results(
  const cv::Mat& img,
  QSharedPointer<std::vector<GaoCe::GaoCe::Result>> results)
{
  _currentImage = img;
  _currentResults = results;

  cv::Mat bgrImg;
  cv::cvtColor(img, bgrImg, cv::COLOR_GRAY2BGR);

  for (auto&& i : *results)
    cv::rectangle(bgrImg,
                  { i._rect.x, i._rect.y },
                  { i._rect.x + i._rect.width, i._rect.y + i._rect.height },
                  { 0, 0, 255 },
                  3);
  _monitor.display(bgrImg);

  if (results->size() == 0) {
    _totalLabel.setText(tr("无缺陷"));
    set_status_ok();
  } else {
    _totalLabel.setText(tr("共 %1 处缺陷").arg(results->size()));
    set_status_ng();
  }

  if (_noStopCheck.isChecked())
    _resultList.display_results(img, std::vector<GaoCe::GaoCe::Result>());
  else
    _resultList.display_results(img, *results);
}

void
GaoCeRunningUi::display_ready(const cv::Mat& img)
{
  _monitor.display(img);
  set_status_ready();
}

void
GaoCeRunningUi::set_status_ok()
{
  _monitor.set_caption("检测结果");
  _resultLabel.display("OK");
  _resultLabel.set_text_color(Qt::green);
  _printButton.setEnabled(false);
}

void
GaoCeRunningUi::set_status_ng()
{
  _monitor.set_caption("缺陷图像");
  _resultLabel.display("NG");
  _resultLabel.set_text_color(Qt::red);
  _printButton.setEnabled(true);
}

void
GaoCeRunningUi::set_status_ready()
{
  _monitor.set_caption("实时画面");
  _resultLabel.display("READY");
  _resultLabel.set_text_color(Qt::blue);
  _printButton.setEnabled(false);
}

void
GaoCeRunningUi::when_printButton_clicked()
{
  QPrintPreviewDialog dlg;

  connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &_T::print_report);

  dlg.exec();
}

void
GaoCeRunningUi::print_report(QPrinter* printer)
{
  ReportPage reportPage(*this);

  auto pageRect = printer->pageRect(QPrinter::DevicePixel);
  reportPage.resize(pageRect.size().toSize());

  reportPage.render(printer);
}

GaoCeRunningUi::ResultList::ResultList()
  : _widget()
  , _resultWidgets()
{
  _S::setWidget(&_widget);
}

void
GaoCeRunningUi::ResultList::clear()
{
  for (auto&& i : _resultWidgets)
    delete i;
  _resultWidgets.clear();
}

void
GaoCeRunningUi::ResultList::display_results(
  const cv::Mat& img,
  const std::vector<GaoCe::GaoCe::Result>& results)
{
  clear();
  _resultWidgets.reserve(results.size());

  for (auto&& result : results) {
    auto cell = new QFrame(&_widget);
    cell->setFrameStyle(QFrame::Box | QFrame::Raised);
    _resultWidgets.push_back(cell);

    auto layout = new QHBoxLayout(cell);

    auto leftImage = new Eyestack::Design::ImageLabel();
    leftImage->display(img(result._rect));
    leftImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(leftImage);

    auto rightWidget = new QWidget();
    layout->addWidget(rightWidget);

    auto rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(
      new QLabel(tr("位置：%1,%2").arg(result._rect.x).arg(result._rect.y)));
    rightLayout->addWidget(new QLabel(
      tr("大小：%1x%2").arg(result._rect.width).arg(result._rect.height)));
    rightLayout->addWidget(new QLabel(tr("面积：%1").arg(result._area)));
    rightLayout->addWidget(
      new QLabel(tr("周长：%1").arg(result._circumference)));
    rightLayout->addWidget(new QLabel(tr("最大长度：%1").arg(result._maxLen)));

    cell->show(); // 居然不会自动显示？
  }

  QResizeEvent event(size(), size());
  resizeEvent(&event);
}

void
GaoCeRunningUi::ResultList::resizeEvent(QResizeEvent* event)
{
  static const int kCellWidth = 200, kCellHeight = 150;

  auto newSize = event->size();

  int cols = std::max(1, newSize.width() / kCellWidth);
  int rows = _resultWidgets.size() / cols + 1;

  _widget.resize(newSize.width() - 2, rows * kCellHeight);

  int colWidth = newSize.width() / cols;

  for (int i = 0, col = 0; i < _resultWidgets.size();
       ++i, col = (col + 1) % cols) {

    _resultWidgets[i]->setGeometry(
      col * colWidth, i / cols * kCellHeight, colWidth, kCellHeight);
  }

  _S::resizeEvent(event);
}

GaoCeRunningUi::ReportPage::ReportPage(GaoCeRunningUi& self)
  : _self(self)
{
  // 配置组件
  auto pal = palette();
  pal.setColor(QPalette::Window, Qt::white);
  setPalette(pal);

  _imageLabel.display(_self._currentImage);

  _resultList.display_results(_self._currentImage, *_self._currentResults);

  _infoLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  _infoLabel.setText(QDateTime::currentDateTime().toString() +
                     tr("\n系统版本：" GaoCeAppV3_VERSION));

  // 布局界面
  auto topWidget = new QWidget();
  auto topLayout = new QHBoxLayout();
  topWidget->setLayout(topLayout);
  topLayout->addWidget(&_imageLabel);
  topLayout->addWidget(&_infoLabel);

  auto layout = new QVBoxLayout();
  setLayout(layout);
  layout->addWidget(topWidget);
  layout->addWidget(&_resultList);

  QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  sizePolicy.setVerticalStretch(1);
  topWidget->setSizePolicy(sizePolicy);
  sizePolicy.setVerticalStretch(3);
  _resultList.setSizePolicy(sizePolicy);
}
