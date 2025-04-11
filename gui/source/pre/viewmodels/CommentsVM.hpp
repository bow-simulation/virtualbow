#pragma once
#include "pre/viewmodels/PropertiesVM.hpp"

class CommentsVM: public PropertiesVM {
public:
    QModelIndex COMMENT;

    CommentsVM(MainVM *parent, std::string& comment):
        PropertiesVM(parent)
    {
        COMMENT = addString(comment);
    }
};
