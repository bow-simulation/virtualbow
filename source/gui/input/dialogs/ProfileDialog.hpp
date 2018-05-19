#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/input/views/ProfileView.hpp"
#include "gui/input/editors/LayerEditor.hpp"

class ProfileDialog: public PersistentDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget* parent);

    Series getData() const;
    void setData(const Series& data);

signals:
    void modified();

private:
    SeriesEditor* edit;
    ProfileView* view;
};
