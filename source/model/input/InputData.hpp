#pragma once
#include "model/document/Document.hpp"
#include "Meta.hpp"
#include "Profile.hpp"
#include "Material.hpp"
#include "String.hpp"
#include "Masses.hpp"
#include "Operation.hpp"
#include "Settings.hpp"
#include <json.hpp>
#include <fstream>

#include "numerics/CubicSpline.hpp"
#include "model/LimbProperties.hpp"

using nlohmann::json;

struct InputData: public DocumentNode
{
    Meta                   meta{*this};
    Settings           settings{*this};
    Profile             profile{*this};
    DocumentItem<Series>  width{*this, Series({0.0, 0.5, 1.0}, {0.04, 0.035, 0.01})};
    DocumentItem<Series> height{*this, Series({0.0, 1.0}, {0.0128, 0.008})};
    Material           material{*this};
    String               string{*this};
    Masses               masses{*this};
    Operation         operation{*this};

    InputData();
    void load(const std::string& path);
    void save(const std::string& path) const;

private:
    // Todo: Put constraint functions somewhere else, maybe in separate file next to document.

    template<typename T, typename F>
    void create_constraint(DocumentItem<T>& item, const std::string& message, const F& validator)
    {
        QObject::connect(this, &DocumentNode::value_changed, [&item, message, validator]()
        {
            if(validator(item))
            {
                item.remove_error(message);
            }
            else
            {
                item.add_error(message);
            }
        });
    }

    template<typename T1, typename T2, typename F>
    void create_constraint(DocumentItem<T1>& item1, DocumentItem<T2>& item2, const std::string& message, const F& validator)
    {
        QObject::connect(this, &DocumentNode::value_changed, [&item1, &item2, message, validator]()
        {
            if(validator(item1, item2))
            {
                item1.remove_error(message);
                item2.remove_error(message);
            }
            else
            {
                item1.add_error(message);
                item2.add_error(message);
            }
        });

        // Todo: Code duplication
        QObject::connect(this, &DocumentNode::value_changed, [&item1, &item2, message, validator]()
        {
            if(validator(item1, item2))
            {
                item1.remove_error(message);
                item2.remove_error(message);
            }
            else
            {
                item1.add_error(message);
                item2.add_error(message);
            }
        });
    }
};

static void to_json(json& obj, const InputData& value)
{
    to_json(obj["meta"], value.meta);
    to_json(obj["settings"], value.settings);
    to_json(obj["profile"], value.profile);
    to_json(obj["width"], value.width);
    to_json(obj["height"], value.height);
    to_json(obj["material"], value.material);
    to_json(obj["string"], value.string);
    to_json(obj["masses"], value.masses);
    to_json(obj["operation"], value.operation);
}

static void from_json(const json& obj, InputData& value)
{
    from_json(obj["meta"], value.meta);
    from_json(obj["settings"], value.settings);
    from_json(obj["profile"], value.profile);
    from_json(obj["width"], value.width);
    from_json(obj["height"], value.height);
    from_json(obj["material"], value.material);
    from_json(obj["string"], value.string);
    from_json(obj["masses"], value.masses);
    from_json(obj["operation"], value.operation);
}
