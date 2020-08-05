#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <windows.h>

class getWindowDialog : public QWidget
{
	Q_OBJECT

public:
	getWindowDialog(QWidget* parent = 0);
	~getWindowDialog();

protected:
	void buttonPressed();

private:
	QLabel* classL, * nameL, * warning;
	QLineEdit* classLE, * nameLE;
	QPushButton* enterB;

signals:
	void getHandle(HWND);
};
