#include "TypeChecker.hpp"

bool TypeChecker::isNumericKind(DataType type) const {
    return type == DataType::INTEGER || type == DataType::REAL;
}

bool TypeChecker::isOrdinalKind(DataType type) const {
    return type == DataType::INTEGER || type == DataType::CHAR ||
           type == DataType::BOOLEAN || type == DataType::RANGE ||
           type == DataType::ENUMERATED;
}

bool TypeChecker::isAssignmentCompatible(DataType targetType, DataType valueType) const {
    if (targetType == valueType) {
        return true;
    }
    return targetType == DataType::REAL && valueType == DataType::INTEGER;
}
