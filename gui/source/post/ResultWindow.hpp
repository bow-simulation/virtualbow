#pragma once
#include "solver/BowResult.hpp"
#include <QMainWindow>

class RecentFilesMenu;

class ResultWindow: public QMainWindow {
public:
    ResultWindow(const QString& filePath, const BowResult& data, QWidget* parent = nullptr);

private:
    BowResult data;

    void closeEvent(QCloseEvent *event) override;
};
