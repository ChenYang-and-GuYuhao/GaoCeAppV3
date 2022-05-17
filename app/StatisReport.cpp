#include "StatisReport.hpp"

StatisReport::StatisReport()
{
  // 设置背景色为白纸
  auto pal = palette();
  pal.setColor(QPalette::Window, Qt::white);
  setPalette(pal);

  // 布局界面
  setLayout(&_mainLayout);
  _mainLayout.setSpacing(0);

  _mainLayout.addLayout(&_headerLayout);
  {
    _headerLayout.setSpacing(0);
    _headerLayout.setContentsMargins(0, 0, 0, 0);
    _headerLayout.setRowMinimumHeight(0, 50);
    _headerLayout.setRowMinimumHeight(1, 50);

    _headerLayout.addWidget(&_total, 0, 0);
    {
      _total.setText("已检测帧数：");
      _total.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_totalValue, 0, 1);
    {
      _totalValue.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_dframe, 0, 2);
    {
      _dframe.setText("包含缺陷的帧数：");
      _dframe.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_dframeValue, 0, 3);
    {
      _dframeValue.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_dcount, 1, 0);
    {
      _dcount.setText("缺陷总数：");
      _dcount.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_dcountValue, 1, 1);
    {
      _dcountValue.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_time, 1, 2);
    {
      _time.setText("运行时间：");
      _time.setFrameStyle(QFrame::Box | QFrame::Plain);
    }

    _headerLayout.addWidget(&_timeValue, 1, 3);
    {
      _timeValue.setFrameStyle(QFrame::Box | QFrame::Plain);
    }
  }

  _mainLayout.addWidget(&_chartView);
  {
    _chartView.setFrameStyle(QFrame::Box | QFrame::Plain);
  }
}

void
StatisReport::set_statistics(RecordIter& iter)
{
  static const double kMin = 0.2, kMax = 2.0, kStep = 0.1;

  clear();

  Record rec;
  if (!iter.next(&rec))
    return;

  int frameNum = 0, dframeNum = 0, dcount = 0;

  QDateTime earliest, latest;
  earliest = latest = rec._timestamp;

  QVector<int> chartCounter(1 + floor((kMax - kMin) / kStep));

  do {
    // 计数
    ++frameNum;
    if (rec._results.size() != 0) {
      ++dframeNum;
      dcount += rec._results.size();
    }

    // 更新最早最晚时刻
    if (rec._timestamp < earliest)
      earliest = rec._timestamp;
    else if (rec._timestamp > latest)
      latest = rec._timestamp;

    // 统计缺陷长度
    for (auto&& result : rec._results) {
      auto maxLen = result._maxLen;
      if (maxLen < kMin)
        ++chartCounter.front();
      else if (maxLen > kMax)
        ++chartCounter.back();
      else
        ++chartCounter[1 + (maxLen - kMin) / kStep];
    }
  } while (iter.next(&rec));

  _totalValue.setText(QString::number(frameNum));
  _dframeValue.setText(QString::number(dframeNum));
  _dcountValue.setText(QString::number(dcount));
  _timeValue.setText(earliest.toString() + "\n-> " + latest.toString());

  auto barSet = new QBarSet("缺陷最长边");
  for (auto&& val : chartCounter)
    *barSet << val;

  auto barSeries = new QBarSeries();
  barSeries->append(barSet);
  _chart->addSeries(barSeries);

  auto axisX = new QBarCategoryAxis();
  {
    axisX->append(QString("≤%1").arg(kMin, 2));
    double i = kMin;
    while ((i += kStep) < kMax)
      axisX->append(QString::number(i, 'g', 2));
    axisX->append(QString(">%1").arg(i, 2));
  }
  _chart->addAxis(axisX, Qt::AlignBottom);
  barSeries->attachAxis(axisX);

  QValueAxis* axisY = new QValueAxis();
  auto maxCount = *std::max_element(chartCounter.begin(), chartCounter.end());
  axisY->setRange(0, maxCount);
  _chart->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);
}

void
StatisReport::print(QPrinter& printer)
{
  auto pageRect = printer.pageRect(QPrinter::DevicePixel);
  resize(pageRect.size().toSize());
  render(&printer);
}

void
StatisReport::clear()
{
  _totalValue.clear();
  _dframeValue.clear();
  _dcountValue.clear();
  _timeValue.clear();

  auto chart = new QChart();
  _chartView.setChart(chart);
  _chart.reset(chart);
  _chart->setTitle("缺陷统计");
}
