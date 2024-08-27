#pragma once

#include <QDialog>
#include "ui_Information.h"

class Information : public QDialog
{
	Q_OBJECT

public:
	Information(QWidget *parent = nullptr);
	~Information();

private slots:
    void on_pushButton_close_clicked();

private:
	Ui::InformationClass ui;
};
