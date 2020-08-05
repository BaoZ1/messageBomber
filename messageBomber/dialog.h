#pragma once

#include <QDialog>
#include "ui_dialog.h"

class dialog : public QDialog
{
	Q_OBJECT

public:
	dialog(QWidget *parent = Q_NULLPTR);
	~dialog();

private:
	Ui::dialog ui;
};
