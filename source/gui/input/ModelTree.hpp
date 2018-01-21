#pragma once
#include "bow/input2/InputData.hpp"
#include "gui/input2/CommentDialog.hpp"
#include "gui/input2/LayerDialog.hpp"

#include <QtWidgets>
#include <functional>

class TreeItem: public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    template<class parent_t>
    TreeItem(parent_t* parent, const QString& name, const QIcon& icon, std::function<void()> action)
        : QTreeWidgetItem(parent, {name}),
          action(action)
    {
        this->setIcon(0, icon);
    }

    void performAction()
    {
        action();
    }

private:
    std::function<void()> action;
};

class ModelTree: public QTreeWidget
{
public:
    ModelTree()
    {
        new TreeItem(this, "Comments", QIcon(":/icons/model-tree/comments"), [&]{
            if(comments == nullptr)
                comments = new CommentDialog(this);

            comments->setData(input.meta.comments);
            comments->activateWindow();
            comments->show();

            /*
            QObject::connect(comments, &LayerDialog::accepted, [&]{
                input.meta.comments = comments->getData();
            });
            */
        });

        new TreeItem(this, "Layers", QIcon(":/icons/model-tree/layers"), [&]{
            if(layers == nullptr)
                layers = new LayerDialog(this);

            layers->setData(input.layers);
            layers->activateWindow();
            layers->show();

            /*
            QObject::connect(layers, &LayerDialog::accepted, [&]{
                input.layers = layers->getData();
            });
            */
        });

        /*
        new TreeItem(this, data.meta.comments, "Comments", QIcon(":/icons/model-tree/comments"), [&]
        {
            auto dialog = new CommentDialog(this, data);
            return dialog;
        });

        new TreeItem(this, data.settings, "Settings", QIcon(":/icons/model-tree/settings"), [&]
        {
            auto dialog = new NumberDialog(this, "Settings");
            dialog->addGroup("General");
            dialog->addField("Limb elements:", "", data.settings.n_elements_limb);
            dialog->addField("String elements:", "", data.settings.n_elements_string);

            dialog->addGroup("Statics");
            dialog->addField("Draw steps:", "", data.settings.n_draw_steps);

            dialog->addGroup("Dynamics");
            dialog->addField("Time span factor:", "", data.settings.time_span_factor);
            dialog->addField("Time step factor:", "", data.settings.time_step_factor);
            dialog->addField("Sampling rate:", "Hz", data.settings.sampling_rate);

            return dialog;
        });

        auto item_parameters = new QTreeWidgetItem(this, {"Parameters"});
        item_parameters->setIcon(0, QIcon(":/icons/model-tree/parameters"));

        new TreeItem(item_parameters, data.profile, "Profile", QIcon(":/icons/model-tree/profile"), [&]
        {
            auto dialog = new ProfileDialog(this, data);
            return dialog;
        });

        new TreeItem(item_parameters, data.width, "Width", QIcon(":/icons/model-tree/width"), [&]
        {
            auto dialog = new WidthDialog(this, data);
            return dialog;
        });

        new TreeItem(item_parameters, data.height, "Height", QIcon(":/icons/model-tree/height"), [&]
        {
            auto dialog = new HeightDialog(this, data);
            return dialog;
        });

        new TreeItem(item_parameters, data.material, "Material", QIcon(":/icons/model-tree/material"), [&]
        {
            auto dialog = new NumberDialog(this, "Material");
            dialog->addField("rho:", "kg/m³", data.material.rho);    // Todo: Use unicode character (\u2374). Problem: Windows
            dialog->addField("E:", "N/m²", data.material.E);
            return dialog;
        });

        new TreeItem(item_parameters, data.string, "String", QIcon(":/icons/model-tree/string"), [&]
        {
            auto dialog = new NumberDialog(this, "String");
            dialog->addField("Strand stiffness:", "N/100%", data.string.strand_stiffness);
            dialog->addField("Strand density:", "kg/m", data.string.strand_density);
            dialog->addField("Number of strands:", "", data.string.n_strands);
            return dialog;
        });

        new TreeItem(item_parameters, data.masses, "Masses", QIcon(":/icons/model-tree/masses"), [&]
        {
            auto dialog = new NumberDialog(this, "Masses");
            dialog->addField("String center:", "kg", data.masses.string_center);
            dialog->addField("String tip:", "kg", data.masses.string_tip);
            dialog->addField("Limb tip:", "kg", data.masses.limb_tip);
            return dialog;
        });

        new TreeItem(item_parameters, data.operation, "Operation", QIcon(":/icons/model-tree/operation"), [&]
        {
            auto dialog = new NumberDialog(this, "Operation");
            dialog->addField("Brace height:", "m", data.operation.brace_height);
            dialog->addField("Draw length:", "m", data.operation.draw_length);
            dialog->addField("Arrow mass:", "kg", data.operation.mass_arrow);
            return dialog;
        });
        */

        QObject::connect(this, &QTreeWidget::itemActivated, [](QTreeWidgetItem* base_item, int column)
        {
            auto item = dynamic_cast<TreeItem*>(base_item);
            if(item)
            {
                item->performAction();
            }
        });

        this->setHeaderLabel("Model Tree");
        this->expandAll();
        this->setItemsExpandable(false);    // Todo: Why is the expansion symbol still visible? (on KDE Desktop at least)
    }

private:
    InputData2 input;

    LayerDialog* layers = nullptr;
    CommentDialog* comments = nullptr;
};
