#include "datamanager.h"
#include "aggregationxy.h"
#include "itemdelegate.h"

DataManager::DataManager(QWidget *parent) : QWidget(parent)
{
    initDataInput();
    showCoorSystem();
    textItem = new QCPItemText(&cPlot);

    connect(&cPlot, &QCustomPlot::mouseMove, this, &DataManager::showPointToolTip);
    connect(&entryStep, &QLineEdit::textChanged, this, &DataManager::changeValue);
}

void DataManager::initDataInput()
{
    auto mainLayout = new QGridLayout;

    interpolationChoose.addItem(tr("Интерполяция Лагранжа"));
    interpolationChoose.addItem(tr("Линейная интерполяция"));

    auto exitBtn = new QPushButton(tr("Выход"));
    connect(exitBtn, SIGNAL(clicked()), qApp, SLOT(quit()));

    auto startBtn = new QPushButton(tr("Построить график"));
    startBtn->setFixedSize(150, 30);
    connect(startBtn, SIGNAL(clicked()), this, SLOT(plotGraphs()));

    auto resetBtn = new QPushButton(tr("Обнулить данные"));
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(reset()));

    auto addPointBtn = new QPushButton;
    addPointBtn->setText(tr("Добавить точку"));
    connect(addPointBtn, SIGNAL(clicked()),  this, SLOT(addColumn()));

    auto deleteBtn = new QPushButton;
    deleteBtn->setText(tr("Удалить точку"));
    connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deleteColumn()));

    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    auto validator = new QDoubleValidator;
    validator->setLocale(locale);
    entryStep.setValidator(validator);

    auto graphLayout = new QHBoxLayout;
    graphLayout->addWidget(&cPlot);

    newCoordsDisplay.setReadOnly(true);

    initCoordTable();

    setLayout(mainLayout);

    mainLayout->addWidget(new QLabel(tr("Выберите способ интерполяции: ")), 0, 0);
    mainLayout->addWidget(&interpolationChoose, 0, 1);
    mainLayout->addWidget(new QLabel(tr("Введите шаг интерполяции: ")), 1, 0);
    mainLayout->addWidget(&entryStep, 1, 1);
    mainLayout->addWidget(startBtn, 5, 1);
    mainLayout->addWidget(resetBtn, 5, 3);
    mainLayout->addLayout(graphLayout, 6, 0, 4, 4);
    mainLayout->addWidget(exitBtn, 11, 6);
    mainLayout->addWidget(new QLabel(tr("Введите координаты точек: ")), 6, 6);
    mainLayout->addWidget(&coordTable, 7, 6, 1, 2);
    mainLayout->addWidget(new QLabel(tr("Получившиеся координаты точек: ")), 0, 6);
    mainLayout->addWidget(&newCoordsDisplay, 1, 6, 3, 3);
    mainLayout->addWidget(addPointBtn, 8, 6, 1, 1);
    mainLayout->addWidget(deleteBtn, 8, 7, 1, 1);
    mainLayout->columnStretch(1);
}

void DataManager::reset()
{
    newCoordsDisplay.clear();
    initCoordTable();
    entryStep.clear();
    cPlot.clearGraphs();
    cPlot.replot();
}

void DataManager::addColumn()
{
    auto count = coordTable.columnCount();
    coordTable.insertColumn(count);
}

void DataManager::deleteColumn()
{
    auto count = coordTable.columnCount();
    if (count > 2)
        coordTable.removeColumn(count-1);
}

void DataManager::initCoordTable()
{
    ItemDelegate *delegate = new  ItemDelegate;

    coordTable.setItemDelegate(delegate);
    coordTable.clear();
    coordTable.setRowCount(2);
    coordTable.setColumnCount(2);
    coordTable.setVerticalHeaderLabels({"X", "Y"});

}

void DataManager::showCoorSystem()
{
    cPlot.xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    cPlot.yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    cPlot.xAxis->setLabel("x Axis");
    cPlot.yAxis->setLabel("y Axis");

    cPlot.replot();
    cPlot.resize(400, 400);
    cPlot.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                          QCP::iSelectPlottables | QCP::iSelectAxes |
                          QCP::iSelectLegend);


    cPlot.legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    cPlot.legend->setFont(legendFont);
    cPlot.legend->setSelectedFont(legendFont);
    cPlot.legend->setSelectableParts(QCPLegend::spItems);
}

void DataManager::addGraphs()
{
    cPlot.addGraph();
    cPlot.graph(0)->setPen(QPen(Qt::red));
    cPlot.graph(0)->rescaleAxes();
    cPlot.graph(0)->setName(tr("Новый график"));

    QCPScatterStyle myScatter;
    myScatter.setShape(QCPScatterStyle::ssCircle);
    myScatter.setPen(QPen(Qt::black, 1.5));
    myScatter.setBrush(Qt::white);
    cPlot.graph(0)->setScatterStyle(myScatter);

    cPlot.addGraph();
    cPlot.graph(1)->setPen(QPen(Qt::black));
    cPlot.graph(1)->rescaleAxes();
    cPlot.graph(1)->setName(tr("Изначальный график"));
}

void DataManager::plotGraphs()
{
    if (!checkDataIsValid())
        return;

    AggregationXY aggr;
    auto points = getData();

    cPlot.clearGraphs();
    addGraphs();

    if (interpolationChoose.currentIndex() == 0){
        auto newPoints = aggr.CalcNewLagranjData(points.x, points.y, interpolationStep_);
        cPlot.graph(0)->setData(newPoints.x, newPoints.y);
        cPlot.graph(1)->setData(points.x, points.y);
        cPlot.graph(0)->rescaleAxes();
        cPlot.graph(1)->rescaleAxes();
        cPlot.replot();

    showNewData(newPoints.x, newPoints.y);
    }
    if (interpolationChoose.currentIndex() == 1) {
        auto newPoints = aggr.CalcNewLinearData(points.x, points.y, interpolationStep_);
        cPlot.graph(0)->setData(newPoints.x, newPoints.y);
        cPlot.graph(1)->setData(points.x, points.y);
        cPlot.graph(0)->rescaleAxes();
        cPlot.graph(1)->rescaleAxes();
        cPlot.replot();

        showNewData(newPoints.x, newPoints.y);
    }
}

bool DataManager::checkDataIsValid()
{
    if (interpolationStep_ == NULL){
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Ошибка"));
        msgBox.setText(tr("Вы не ввели шаг интерполяции, "
                          "сделайте это пожалуйста"));
        msgBox.exec();
        return false;
    }

    auto count = coordTable.columnCount();

    auto model = coordTable.model();

    for(int i = 0; i < count; ++i)
    {
        if(model->index(0, i).data().isNull() || model->index(1, i).data().isNull()){
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Ошибка"));
            msgBox.setText(tr("Вы ввели не все координаты, "
                              "сделайте это пожалуйста"));
            msgBox.exec();
            return false;
        }
    }
    return true;
}

void DataManager::showNewData(QVector<double> x, QVector<double> y)
{
    QMap<double, double> coords;

    for (int i = 0; i<x.size(); ++i)
        coords[x[i]] = y[i];

    newCoordsDisplay.clear();
    newCoordsDisplay.insertPlainText("X\tY\n");

    foreach(double key, coords.keys()){
        double value = coords.value(key);
        newCoordsDisplay.insertPlainText(QString::number(key));
        newCoordsDisplay.insertPlainText("\t");
        newCoordsDisplay.insertPlainText(QString::number(value));
        newCoordsDisplay.insertPlainText("\n");
    }
}

Coordinates DataManager::getData()
{
    Coordinates coords;
    auto count = coordTable.columnCount();
    auto model = coordTable.model();

    for(int i = 0; i < count; ++i)
    {
        auto x_i = model->index(0, i).data().toDouble();
        auto y_i = model->index(1, i).data().toDouble();
        coords.x.push_back(x_i);
        coords.y.push_back(y_i);
    }
    return coords;
}

void DataManager::showPointToolTip(QMouseEvent * event)
{
    double x = cPlot.xAxis->pixelToCoord(event->pos().x());
    double y = cPlot.yAxis->pixelToCoord(event->pos().y());
    textItem->setText(QString("(%1, %2)").arg(x).arg(y));
    textItem->position->setCoords(QPointF(x, y));
    textItem->setFont(QFont(font().family(), 10));
    cPlot.replot();
}

void DataManager::changeValue()
{
    auto newValue = entryStep.text().toDouble();
    interpolationStep_ = newValue;
}
