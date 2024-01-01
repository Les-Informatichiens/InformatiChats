//
// Created by thierrycd on 2023-12-31.
//

#pragma once

class Validator
{
public:
    virtual bool validate(const ConfigType& value) = 0;
};
