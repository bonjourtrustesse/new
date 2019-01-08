#include "aggregationxy.h"
#include "datamanager.h"

#include "math.h"

AggregationXY::AggregationXY(QObject *parent) : QObject(parent)
{}

double AggregationXY::Lagranj(QVector<double> x, QVector<double> y, double X)
{
    if (x.size() != y.size())
        return 0;

    const int n = x.size();

    double result = 0;

    for (int i = 0; i < n; ++i)
    {
        double l = 1;

        for (int j = 0; j < n; ++j)
            if (i != j)
                l *= (X - x[j]) / (x[i] - x[j]);

        result += y[i] * l;
    }

    return result;
}

double AggregationXY::Linear(QVector<double> x, QVector<double> y, double X)
{
    if (x.size() != y.size())
       return 0;
    const int n = x.size();

    double max = X;
    double min;
    int indexMax, indexMin;

    for (int i = 0; i < n; i++){
        if (x[i] > X){
            max  = x[i];
            indexMax = i;
            min = x[i-1];
            indexMin = i-1;
            break;
        }
        if (i == (n-1)){
            max  = x[n-1];
            indexMax = n-1;
            min = x[n-2];
            indexMin = n-2;
            break;
        }

    }

    double result = (double(max - X) / (max-min)) * y[indexMin] +
            (double(X - min) / (max - min) * y[indexMax]);

    //double result = y[indexMin] + (y[indexMax] - y[indexMin]) *
   //         (X - min) / (max - min);

    return result;
}

Coordinates AggregationXY::CalcNewLagranjData(QVector<double> X, QVector<double> Y, double step)
{
    Coordinates newPoints;

    QMap <double, double> coords;
    for (int i = 0; i < X.size(); ++i)
        coords[X[i]] = Y[i];

    QVector <double> xNew, yNew;

    foreach(double key, coords.keys()){
        double value = coords.value(key);
        xNew.push_back(key);
        yNew.push_back(value);
    }

    const auto first = xNew[0];
    const auto last = xNew[xNew.size() - 1];

    newPoints.x = xNew;

    for (double i = first; i <= last; i += step)
        newPoints.x.push_back(i);

    std::sort(newPoints.x.begin(), newPoints.x.end());

    for (auto i : newPoints.x)
        newPoints.y.push_back(Lagranj(xNew, yNew, i));

    return newPoints;
}

Coordinates AggregationXY::CalcNewLinearData(QVector<double> X, QVector<double> Y, double step)
{
    Coordinates newPoints;

    QMap <double, double> coords;
    for (int i = 0; i < X.size(); ++i)
        coords[X[i]] = Y[i];

    QVector <double> xNew, yNew;

    foreach(double key, coords.keys()){
        double value = coords.value(key);
        xNew.push_back(key);
        yNew.push_back(value);
    }

    const auto first = xNew[0];
    const auto last = xNew[xNew.size() - 1];

    newPoints.x = xNew;

    for (double i = first; i <= last; i += step)
        newPoints.x.push_back(i);

    std::sort(newPoints.x.begin(), newPoints.x.end());

    for (auto i : newPoints.x)
        newPoints.y.push_back(Linear(xNew, yNew, i));

    return newPoints;
}
