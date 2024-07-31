/*
    Calculate - Version 2.1.1rc10
    Last modified 2018/09/03
    Released under MIT license
    Copyright (c) 2016-2018 Alberto Lorenzo <alorenzo.md@gmail.com>
*/


#ifndef __CALCULATE_WRAPPER_HPP__
#define __CALCULATE_WRAPPER_HPP__

#include <utility>

#include "util.hpp"


namespace calculate {

template<typename Type, typename Source>
struct WrapperConcept {
    virtual ~WrapperConcept() = default;
    virtual std::shared_ptr<WrapperConcept> clone() const = 0;
    virtual std::size_t argc() const noexcept = 0;
    virtual Type eval(const std::vector<Source>&) const = 0;
    virtual Type call(const std::vector<Type>&) const = 0;
};


template<typename Type, typename Source = Type>
class Wrapper {
    static_assert(util::is_value_v<Type>, "Wrapper type is not a value");

    friend struct std::hash<Wrapper>;
    using WrapperConcept = calculate::WrapperConcept<Type, Source>;

    template<typename Callable, typename Adapter, std::size_t argcount>
    class WrapperModel final : public WrapperConcept {
        static_assert(
            util::is_same_v<
                util::args_tuple_t<Callable>,
                util::make_tuple_t<Type, util::argc_v<Callable>>
            >,
            "Wrong callable arguments types"
        );
        static_assert(
            util::is_same_v<util::result_t<Callable>, Type>,
            "Wrong callable return type"
        );
        static_assert(util::is_const_v<Callable>, "Callable may have side effects");

        static_assert(
            util::is_same_v<
                util::args_tuple_t<Adapter>,
                util::make_tuple_t<Source, 1>
            >,
            "Wrong adapter arguments types"
        );
        static_assert(
            util::is_same_v<util::result_t<Adapter>, Type>,
            "Wrong adapter return type"
        );
        static_assert(util::is_const_v<Adapter>, "Adapter may have side effects");

        Callable _callable;
        Adapter _adapter;

        template<std::size_t... indices>
        Type _eval(const std::vector<Source>& args, std::index_sequence<indices...>) const {
            if (args.size() != argcount)
                throw ArgumentsMismatch{argcount, args.size()};
            return _callable(_adapter(args[indices])...);
        }

        template<std::size_t... indices>
        Type _call(const std::vector<Type>& args, std::index_sequence<indices...>) const {
            if (args.size() != argcount)
                throw ArgumentsMismatch{argcount, args.size()};
            return _callable(args[indices]...);
        }

    public:
        WrapperModel(const Callable& callable, const Adapter& adapter) :
                _callable{callable},
                _adapter{adapter}
        {}

        WrapperModel(Callable&& callable, const Adapter& adapter) :
                _callable{std::move_if_noexcept(callable)},
                _adapter{adapter}
        {}

        WrapperModel(const Callable& callable, Adapter&& adapter) :
                _callable{callable},
                _adapter{std::move_if_noexcept(adapter)}
        {}

        WrapperModel(Callable&& callable, Adapter&& adapter) :
                _callable{std::move_if_noexcept(callable)},
                _adapter{std::move_if_noexcept(adapter)}
        {}

        std::shared_ptr<WrapperConcept> clone() const override {
            return std::make_shared<WrapperModel>(*this);
        }

        std::size_t argc() const noexcept override { return argcount; }

        Type eval(const std::vector<Source>& args) const override {
            return _eval(args, std::make_index_sequence<argcount>{});
        }

        Type call(const std::vector<Type>& args) const override {
            return _call(args, std::make_index_sequence<argcount>{});
        }
    };

    template<typename Callable, typename Adapter>
    using ModelType = WrapperModel<
        std::decay_t<Callable>,
        std::decay_t<Adapter>,
        util::argc_v<Callable>
    >;

    std::shared_ptr<WrapperConcept> _wrapper;

    Wrapper(std::shared_ptr<WrapperConcept>&& wrapper) : _wrapper{std::move(wrapper)} {}

protected:
    Type eval(const std::vector<Source>& args) const { return _wrapper->eval(args); }

public:
    template<typename Callable, typename Adapter>
    Wrapper(Callable&& callable, Adapter&& adapter) :
            _wrapper{std::make_shared<ModelType<Callable, Adapter>>(
                std::forward<Callable>(callable),
                std::forward<Adapter>(adapter)
            )}
    {}

    template<
        typename Callable,
        typename = std::enable_if_t<util::not_same_v<Callable, Wrapper>>,
        typename = std::enable_if_t<!util::is_base_of_v<WrapperConcept, Callable>>
    >
    Wrapper(Callable&& callable) :
            Wrapper{
                std::forward<Callable>(callable),
                [](const Source& x) { return static_cast<Type>(x); }
            }
    {}

    template<
        typename Model,
        typename = std::enable_if_t<util::is_base_of_v<WrapperConcept, Model>>
    >
    Wrapper(Model&& model) : _wrapper{std::make_shared<Model>(std::forward<Model>(model))} {}

    bool operator==(const Wrapper& other) const noexcept { return _wrapper == other._wrapper; }

    bool operator!=(const Wrapper& other) const noexcept { return !operator==(other); }

    template<typename... Args>
    Type operator()(Args&&... args) const {
        return _wrapper->call(util::to_vector<Type>(std::forward<Args>(args)...));
    }

    template<typename... Args>
    Type eval(Args&&... args) const {
        return _wrapper->eval(util::to_vector<Source>(std::forward<Args>(args)...));
    }

    std::size_t argc() const noexcept { return _wrapper->argc(); }

    Wrapper clone() const { return Wrapper{_wrapper->clone()}; }

    bool valid() const noexcept { return static_cast<bool>(_wrapper); }
};

}


namespace std {

template<typename Type, typename Source>
struct hash<calculate::Wrapper<Type, Source>> {
    size_t operator()(const calculate::Wrapper<Type, Source>& wrapper) const {
        return hash<std::shared_ptr<calculate::WrapperConcept<Type, Source>>>{}(wrapper._wrapper);
    }
};

}

#endif
