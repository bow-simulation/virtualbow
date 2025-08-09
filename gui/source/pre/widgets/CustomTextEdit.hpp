#pragma once
#include <QPlainTextEdit>

// A custom QTextEdit subclass that has an editingFinished() signal like QLineEdit.
// Source: https://github.com/Stellarium/stellarium/blob/58e675ecd3dfc21b69dfb4981407e925e96224f2/plugins/Scenery3d/src/gui/StoredViewDialog_p.hpp#L32
class CustomTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    CustomTextEdit(QWidget* parent = nullptr);

protected:
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;

signals:
    void editingFinished();    // Emitted when focus was lost and text was changed

private:
    bool textChanged;
    bool trackChange;
};
