#pragma once
#include <jsoncons/json.hpp>

#include "Validator.hpp"

#include <QtCore>   // Todo: Use std library for file handling, remove this

class JsonInterface
{
    virtual void load(const jsoncons::ojson& obj) = 0;
    virtual void save(jsoncons::ojson& obj) const = 0;

    virtual void load(const QString& path)
    {
        QFile file(path);
        if(!file.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::runtime_error("Could not open file");    // Todo: Better message with filename
        }

        std::string str = QTextStream(&file).readAll().toStdString();
        jsoncons::ojson obj = jsoncons::ojson::parse_string(str);

        load(obj);
    }

    virtual void save(const QString& path)
    {
        jsoncons::ojson obj;
        save(obj);

        QFile file(path);
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
            throw std::runtime_error("Could not open file");    // Todo: Better message with filename
        }

        std::ostringstream oss;
        oss << jsoncons::pretty_print(obj);
        QTextStream(&file) << QString::fromStdString(oss.str());
    }
};

template<typename T>
class Value: public JsonInterface
{
public:
    Value(Validator<T> validator)
        : validator(validator)
    {

    }

    // Value access

    const T& get()
    {
        return value;
    }

    void set(const T& v)
    {
        value = v;
    }

    // Json Interface

    virtual void load(const jsoncons::ojson& obj) override
    {
        value = obj.as<T>();
    }

    virtual void save(jsoncons::ojson& obj) const override
    {
        obj = value;
    }

private:
    T value;
    Validator<T> validator;
};

using Integer = Value<int>;
using Double = Value<double>;
using Vector = Value<std::vector<double>>;
using String = Value<std::string>;
