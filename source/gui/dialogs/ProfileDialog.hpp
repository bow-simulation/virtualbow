#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/views/ProfileView.hpp"
#include "gui/editors/LayerEditor.hpp"
#include "gui/editors/ProfileEditor.hpp"
#include "gui/units/UnitSystem.hpp"

class ProfileDialog: public PersistentDialog
{
    Q_OBJECT

public:
    ProfileDialog(QWidget* parent, const UnitSystem& units);

    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();

private:
    ProfileEditor* edit;
    ProfileView* view;
};
