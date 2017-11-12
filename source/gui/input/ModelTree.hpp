#pragma once
#include "model/input/InputData.hpp"
#include "gui/input/CommentDialog.hpp"
#include "gui/input/ProfileDialog.hpp"
#include "gui/input/WidthDialog.hpp"
#include "gui/input/HeightDialog.hpp"
#include "gui/input/NumberDialog.hpp"

#include <QtWidgets>
#include <functional>

class TreeItem: public QTreeWidgetItem
{
public:
    std::function<void()> action;

    template<class parent_t>
    TreeItem(parent_t* parent, const QString& name, const QIcon& icon, const std::function<void()>& action = []{})
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
        new TreeItem(this, "Comments", QIcon(":/icons/model-tree/comments"), [&]
        {
            CommentDialog dialog(this, data);
            dialog.exec();
        });

        new TreeItem(this, "Settings", QIcon(":/icons/model-tree/settings"), [&]
        {
            NumberDialog dialog(this, "Settings");

            dialog.addGroup("General");
            dialog.addField("Limb elements:", "", data.settings.n_elements_limb);
            dialog.addField("String elements:", "", data.settings.n_elements_string);

            dialog.addGroup("Statics");
            dialog.addField("Draw steps:", "", data.settings.n_draw_steps);

            dialog.addGroup("Dynamics");
            dialog.addField("Time span factor:", "", data.settings.time_span_factor);
            dialog.addField("Time step factor:", "", data.settings.time_step_factor);
            dialog.addField("Sampling rate:", "Hz", data.settings.sampling_rate);

            dialog.exec();
        });

        auto item_parameters = new TreeItem(this, "Parameters", QIcon(":/icons/model-tree/parameters"));

        new TreeItem(item_parameters, "Profile", QIcon(":/icons/model-tree/profile"), [&]
        {
            ProfileDialog dialog(this, data);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Width", QIcon(":/icons/model-tree/width"), [&]
        {
            WidthDialog dialog(this, data);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Height", QIcon(":/icons/model-tree/height"), [&]
        {
            HeightDialog dialog(this, data);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Material", QIcon(":/icons/model-tree/material"), [&]
        {
            NumberDialog dialog(this, "Material");
            dialog.addField("rho:", "kg/m³", data.sections.rho);    // Todo: Use unicode character (\u2374). Problem: Windows
            dialog.addField("E:", "N/m²", data.sections.E);
            dialog.exec();
        });

        new TreeItem(item_parameters, "String", QIcon(":/icons/model-tree/string"), [&]
        {
            NumberDialog dialog(this, "String");
            dialog.addField("Strand stiffness:", "N/100%", data.string.strand_stiffness);
            dialog.addField("Strand density:", "kg/m", data.string.strand_density);
            dialog.addField("Number of strands:", "", data.string.n_strands);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Masses", QIcon(":/icons/model-tree/masses"), [&]
        {
            NumberDialog dialog(this, "Masses");
            dialog.addField("String center:", "kg", data.masses.string_center);
            dialog.addField("String tip:", "kg", data.masses.string_tip);
            dialog.addField("Limb tip:", "kg", data.masses.limb_tip);
            dialog.exec();
        });

        new TreeItem(item_parameters, "Operation", QIcon(":/icons/model-tree/operation"), [&]
        {
            NumberDialog dialog(this, "Operation");
            dialog.addField("Brace height:", "m", data.operation.brace_height);
            dialog.addField("Draw length:", "m", data.operation.draw_length);
            dialog.addField("Arrow mass:", "kg", data.operation.mass_arrow);
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
