#pragma once
#include "model/InputData.hpp"
#include "gui/input/CommentsDialog.hpp"
#include "gui/input/NumberDialog.hpp"

#include <QtWidgets>
#include <functional>

class TreeItem: public QTreeWidgetItem
{
public:
    std::function<void()> action;

    template<class parent_t>
    TreeItem(parent_t* parent, const QString& name, const QIcon& icon, const std::function<void()>& action = [](){})
        : QTreeWidgetItem(parent, {name}),
          action(action)
    {
        this->setIcon(0, icon);
    }
};

class ModelTree: public QTreeWidget
{
public:
    ModelTree(InputData& data)
    {
        new TreeItem(this, "Comments", QIcon(":/icons/comments"), [&](){
            CommentsDialog dialog(this, data);
            dialog.exec();
        });

        new TreeItem(this, "Settings", QIcon(":/icons/settings"), [&](){
            NumberDialog dialog(this, "Settings");

            dialog.addGroup("General");
            dialog.addField("Limb elements:", "", data.settings_n_elements_limb);
            dialog.addField("String elements:", "", data.settings_n_elements_string);

            dialog.addGroup("Statics");
            dialog.addField("Draw steps:", "", data.settings_n_draw_steps);

            dialog.addGroup("Dynamics");
            dialog.addField("Time span factor:", "", data.settings_time_span_factor);
            dialog.addField("Time step factor:", "", data.settings_time_step_factor);
            dialog.addField("Sampling time:", "s", data.settings_sampling_time);

            dialog.exec();
        });

        auto item_parameters = new TreeItem(this, "Parameters", QIcon(":/icons/circle-grey"));

        auto item_limbs = new TreeItem(item_parameters, "Limbs", QIcon(":/icons/circle-grey"));

        new TreeItem(item_limbs, "Material", QIcon(":/icons/circle-grey"), [&](){
            NumberDialog dialog(this, "Material");
            dialog.addField("rho:", "kg/m³", data.sections_rho);    // Todo: Use unicode character (\u2374). Problem: Windows
            dialog.addField("E:", "N/m²", data.sections_E);
            dialog.exec();
        });

        new TreeItem(item_limbs, "Profile", QIcon(":/icons/circle-grey"));

        new TreeItem(item_limbs, "Width", QIcon(":/icons/circle-grey"));

        new TreeItem(item_limbs, "Height", QIcon(":/icons/circle-grey"));

        new TreeItem(item_parameters, "String", QIcon(":/icons/circle-grey"), [&](){
            NumberDialog dialog(this, "String");
            dialog.addField("Strand stiffness:", "N/100%", data.string_strand_stiffness);
            dialog.addField("Strand density:", "kg/m", data.string_strand_density);
            dialog.addField("Number of strands:", "", data.string_n_strands);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Masses", QIcon(":/icons/circle-grey"), [&](){
            NumberDialog dialog(this, "Masses");
            dialog.addField("String center:", "kg", data.mass_string_center);
            dialog.addField("String tip:", "kg", data.mass_string_tip);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Operation", QIcon(":/icons/circle-grey"), [&](){
            NumberDialog dialog(this, "Operation");
            dialog.addField("Brace height:", "m", data.operation_brace_height);
            dialog.addField("Draw length:", "m", data.operation_draw_length);
            dialog.addField("Arrow mass:", "kg", data.operation_mass_arrow);
            dialog.exec();
        });

        QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* item, int column)
        {
            auto ptr = dynamic_cast<TreeItem*>(item);
            if(ptr)
            {
                ptr->action();
            }
        });

        this->setHeaderLabel("Model Tree");
        this->expandAll();
    }

};
