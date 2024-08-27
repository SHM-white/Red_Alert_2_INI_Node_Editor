#include "Information.h"

Information::Information(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

Information::~Information()
{}

void Information::on_pushButton_close_clicked()
{
    this->close();
}

