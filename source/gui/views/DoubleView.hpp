#pragma once
#include "model/document/Document.hpp"
#include <QtWidgets>

class DoubleView: public QLineEdit
{
public:
    DoubleView(DocumentItem<double>& doc_item);

private:
    DocumentItem<double>& doc_item;

    void update_value();
    void update_error();
};

class DoubleValidator: public QDoubleValidator
{
public:
    DoubleValidator(DocumentItem<double>& doc_item);
    virtual State validate(QString &input, int &pos) const override;
    virtual void fixup(QString& input) const override;

private:
    DocumentItem<double>& doc_item;
};
