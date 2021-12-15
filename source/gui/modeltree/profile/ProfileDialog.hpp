#pragma once
#include "ProfileEditor.hpp"
#include "ProfileView.hpp"
#include "gui/widgets/PersistentDialog.hpp"
#include "gui/units/UnitSystem.hpp"

class ProfileDialog: public PersistentDialog {
    Q_OBJECT

public:
    ProfileDialog(QWidget* parent);

    std::vector<SegmentInput> getData() const;
    void setData(const std::vector<SegmentInput>& data);

signals:
    void modified();

private:
    ProfileEditor* edit;
    ProfileView* view;
};
