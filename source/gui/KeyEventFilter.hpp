#pragma once
#include <QObject>

// This program uses the C-locale for number representation, i.e. a dot as the decimal separator.
// Various languages however use a comma as the decimal separator and therefore the keyboards often feature
// a comma key on the keypad instead of a dot.
//
// To make using the keypad for number input more convenient in those cases, this global event filter replaces
// any key events where a comma is pressed on the number block with an equivalent key event for a dot.
//
// Implementation based on https://stackoverflow.com/a/53913107

class KeyEventFilter: public QObject
{
    Q_OBJECT

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};
