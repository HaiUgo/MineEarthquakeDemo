#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>

class Chart;

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
    ChartView();
    ~ChartView();
};

#endif
