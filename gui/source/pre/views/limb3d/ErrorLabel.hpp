#pragma once
#include <QLabel>

// Custom label class derived from QLabel, mainly to override the sizeHint method to make
// the default width larger while still allowing it to shrink when needed.

class ErrorLabel: public QLabel {
public:
    ErrorLabel(QWidget* parent = nullptr);
    QSize sizeHint() const override;
};
