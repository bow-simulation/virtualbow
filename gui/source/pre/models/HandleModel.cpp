#include "HandleModel.hpp"
#include "solver/BowModel.hpp"

HandleModel::HandleModel(Handle& handle) {
    HANDLE = addCustom(handle);
}
