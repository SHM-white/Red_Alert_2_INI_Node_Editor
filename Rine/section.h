#pragma once
#include "member.h"
class member;

#include <iostream>
#include <QString>
#include <QList>
#include <QPoint>

#include <section_item.h>

#ifndef SECTION_H
#define SECTION_H
class section{
private:
    QString section_head;
    QList<member> rules_member;
    QList<member> arts_member;
    QPoint position;
    QString Type;
    section_item *item_ptr;
public:
    section();
    ~section();
    void add_member();
    QString get_head();
    QString get_type();
    QString get_pos();
    void draw();
    void set_main_section();
    void update_section();
};
#endif // SECTION_H
