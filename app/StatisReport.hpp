#pragma once

#include <GaoCe.hpp>
#include <QtCharts>
#include <QtPrintSupport>

/**
 * @brief 用于生成打印报告
 */
class StatisReport : private QWidget
{
  friend class QObject;

  using _T = StatisReport;
  using _S = QWidget;

public:
  struct Record
  {
    QDateTime _timestamp;
    std::vector<GaoCe::GaoCe::Result> _results;
  };

  struct RecordIter
  {
    virtual ~RecordIter() = default;

    /**
     * @brief 获取下一个记录
     * @param record [out] 取出的记录
     * @return 获取成功返回 true，遍历完毕返回 false
     */
    virtual bool next(Record* record) noexcept = 0;
  };

public:
  /**
   * @brief 配置布局
   */
  StatisReport();

public:
  /**
   * @brief 设置打印报告的数据
   */
  void set_statistics(RecordIter& iter);

  void print(QPrinter& printer);

private:
  QVBoxLayout _mainLayout;

  QGridLayout _headerLayout;
  QLabel _total, _totalValue;
  QLabel _dframe, _dframeValue;
  QLabel _dcount, _dcountValue;
  QLabel _time, _timeValue;

  QChartView _chartView;
  QScopedPointer<QChart> _chart;

private:
  void clear();
};
