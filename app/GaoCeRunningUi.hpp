#pragma once

#include "GaoCe.hpp"
#include <Eyestack/Design/Monitor.hpp>
#include <QtPrintSupport>

/**
 * @brief 高测算法运行监控界面类
 */
class GaoCeRunningUi : public QWidget
{
  Q_OBJECT

  using _T = GaoCeRunningUi;
  using _S = QWidget;

signals:
  /**
   * @brief 不间断运行模式更新时发射此信号
   */
  void noStopUpdated(bool checked);

public:
  /**
   * @brief 配置组件 + 布局
   */
  GaoCeRunningUi(QWidget* parent = nullptr);

public:
  /**
   * @brief 显示高测算法结果
   */
  void display_results(
    const cv::Mat& img,
    QSharedPointer<std::vector<GaoCe::GaoCe::Result>> results);

  /**
   * @brief 显示准备图像
   */
  void display_ready(const cv::Mat& img);

private:
  /**
   * @brief 用于排版结果
   */
  class ResultList : public QScrollArea
  {
    using _T = ResultList;
    using _S = QScrollArea;

  public:
    ResultList();

  public:
    /**
     * @brief 用于清空排版结果
     */
    void clear();
    /**
     * @brief 用于展示排版结果
     */
    void display_results(const cv::Mat& img,
                         const std::vector<GaoCe::GaoCe::Result>& results);

  private:
    QWidget _widget;
    std::vector<QWidget*> _resultWidgets;

    // QWidget interface
  protected:
    virtual void resizeEvent(QResizeEvent* event) override;
  };

  /**
   * @brief 用于生成报告打印页
   */
  class ReportPage : public QWidget
  {
  public:
    Eyestack::Design::ImageLabel _imageLabel;
    QLabel _infoLabel;
    ResultList _resultList;

  public:
    ReportPage(GaoCeRunningUi& self);

  private:
    GaoCeRunningUi& _self;
  };

private:
  Eyestack::Design::Monitor _monitor;
  ResultList _resultList;
  Eyestack::Design::ImageLabel _resultLabel;
  QLabel _totalLabel;
  QCheckBox _noStopCheck;
  QPushButton _printButton;
  QPushButton _snapButton;

  cv::Mat _currentImage;
  QSharedPointer<std::vector<GaoCe::GaoCe::Result>> _currentResults{ nullptr };

private:
  void set_status_ok();
  void set_status_ng();
  void set_status_ready();

private slots:
  void when_printButton_clicked();
  void print_report(QPrinter* printer);
};
