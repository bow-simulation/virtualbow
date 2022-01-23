#pragma once
#include <QObject>
#include "solver/model/input/InputData.hpp"

class DataViewModel: public QObject
{
    Q_OBJECT

public:
    DataViewModel();

    void loadDefaults();
    void loadFile(const QString& path);
    void saveFile(const QString& path);

    const InputData& getData() const;
    void setData(const InputData& data, QObject* source);

    QString getComments() const;
    void setComments(const QString& comment, QObject* source);

signals:
    void nothingSelected();    // TODO: noneSelected();
    void commentsSelected();
    void settingsSelected();
    void dimensionsSelected();
    void materialsSelected();
    void layersSelected();
    void profileSelected();
    void widthSelected();
    void stringSelected();
    void massesSelected();
    void dampingSelected();

    void anyModified(QObject* source);
    void allModified(QObject* source);
    void commentsModified(QObject* source);
    void settingsModified(QObject* source);
    void dimensionsModified(QObject* source);

private:
    InputData data;
};
