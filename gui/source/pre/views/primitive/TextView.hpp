#pragma once
#include "pre/widgets/CustomTextEdit.hpp"

class QAbstractItemModel;

class TextView: public CustomTextEdit
{
public:
    TextView(QAbstractItemModel* model, QPersistentModelIndex index);
};
