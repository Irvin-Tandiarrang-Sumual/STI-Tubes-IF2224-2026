#pragma once

#include "../symboltable/DataType.hpp"

class TypeChecker {
public:
    TypeChecker() = default;
    ~TypeChecker() = default;

    bool isNumericKind(DataType type) const;
    bool isOrdinalKind(DataType type) const;
    bool isAssignmentCompatible(DataType targetType, DataType valueType) const;
};
