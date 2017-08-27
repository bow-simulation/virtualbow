#pragma once
#include <QThread>
#include <functional>

#include "model/BowModel.hpp"

class Thread: public QThread
{
public:
    Thread(const std::function<void()>& function)
        : function(function)
    {
        setTerminationEnabled();
    }

    virtual void run() override
    {
        function();
    }

private:
    const std::function<void()>& function;
};
