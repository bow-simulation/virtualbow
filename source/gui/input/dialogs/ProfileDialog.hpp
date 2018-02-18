#pragma once
#include "bow/input/Profile.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/input/views/ProfileView.hpp"
#include "gui/input/editors/LayerEditor.hpp"

class ProfileDialog: public PersistentDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget* parent);

    Profile getData() const;
    void setData(const Profile& profile);

signals:
    void modified();

private:
    SeriesEditor* table;
    ProfileView* view;
    DoubleEditor* edit_x_pos;
    DoubleEditor* edit_y_pos;
    DoubleEditor* edit_angle;
};
