#include "KeyEventFilter.hpp"
#include <QCoreApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QLocale>

bool KeyEventFilter::eventFilter(QObject* obj, QEvent* event) {
    if(event->type() == QEvent::KeyPress) {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Comma && (keyEvent->modifiers() & Qt::KeypadModifier)) {
            QKeyEvent newEvent(keyEvent->type(), Qt::Key_Period, keyEvent->modifiers(), QLocale(QLocale::C).decimalPoint(), keyEvent->isAutoRepeat(), keyEvent->count());
            QCoreApplication::sendEvent(obj, &newEvent);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
