#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "../Downloads/qcustomplot-source/qcustomplot.h"
#include "aggregationxy.h"

class DataManager : public QWidget
{
    Q_OBJECT
public:
    explicit DataManager(QWidget *parent = nullptr);

    void initDataInput();                                       //!< Инициализация полей ввода данных
    void showCoorSystem();                                      //!< Инициализация отображения системы координат
    void addGraphs();                                           //!< Добавление графиков
    void showNewData(QVector <double> x, QVector <double> y);   //!< Отображение полученных данных
    void initCoordTable();                                      //!< Инициализация таблицы с координатами
    bool checkDataIsValid();                                    //!< Проверка входных данных

    Coordinates getData();                                      //!< Получение данных из таблицы

public slots:

    /*! Слот отображения координат */
    void showPointToolTip(QMouseEvent* event);

    /*! Слот построения графиков */
    void plotGraphs();

    /*! Слот изменения значения шага интерполяции */
    void changeValue();

    /*! Слот добавления колонки */
    void addColumn();

    /*! Слот добавления колонки */
    void deleteColumn();

    /*! Слот обнуления данных */
    void reset();

private:
    double interpolationStep_;         //!< Шаг интерполяции

    QTableWidget coordTable;               //!< Таблица заполения координат
    QLineEdit entryStep;                   //!< Ввод шага интерполяции
    QCustomPlot cPlot;                     //!< Отображение графиков
    QComboBox interpolationChoose;         //!< Виджет выбора вида интерполяции
    QCPItemText *textItem;                 //!< Отображение координат на графике
    QTextEdit newCoordsDisplay;            //!< Виджет отображения полученных координат
};

#endif // DATAMANAGER_H
