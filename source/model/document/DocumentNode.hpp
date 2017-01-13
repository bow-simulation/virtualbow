#pragma once
#include "Value.hpp"
#include <map>
#include <vector>
#include <boost/variant.hpp>

class DocumentNode: public JsonInterface
{
public:
    using NodeMap = std::map<std::string, DocumentNode>;
    using NodeVec = std::vector<DocumentNode>;

    // Constructors

    DocumentNode()
    {

    }

    template<typename T>
    DocumentNode(Value<T> value): data(value)
    {

    }

    DocumentNode(std::initializer_list<NodeMap::value_type> nodes): data(NodeMap(nodes))
    {

    }

    DocumentNode(std::initializer_list<NodeVec::value_type> nodes): data(NodeVec(nodes))
    {

    }

    // Object node methods

    DocumentNode& operator[](const std::string& key)
    {
        return boost::get<NodeMap>(data)[key];
    }

    // Array node methods


    // Value access

    template<typename T>
    T& as()
    {
        return boost::get<T>(data);
    }

    // Inherited methods

    virtual void load(const jsoncons::ojson& obj) override
    {

    }

    virtual void save(jsoncons::ojson& obj) const override
    {

    }

private:
    boost::variant<NodeMap, NodeVec, Integer, Double, Vector, String> data;
};
