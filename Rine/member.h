#pragma once
#include "section.h"
class section;

#include <iostream>
#include <QString>
#include <QList>
#include "member_item.h"

#ifndef MEMBER_H
#define MEMBER_H
class member{
private:
    QString key;
    QString value;
    //section *value_ptr;
    member_item *mem_item_ptr;
public:
    member();
    ~member();
    void connect_next();
    void edit();
    void draw();
};

#endif // MEMBER_H
