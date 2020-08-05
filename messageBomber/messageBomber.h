#pragma once

#include <QtWidgets/QWidget>
#include <QTimer>
#include <QClipBoard>
#include "getWindowDialog.h"
#include "ui_messageBomber.h"

class messageBomber : public QWidget
{
	Q_OBJECT

public:
	messageBomber(QWidget* parent = Q_NULLPTR);

protected:
	void run();
	void wait();
	void send();
	void finish();

private:
	Ui::Form ui;

	bool running;
	bool defaultMsg;
	int count, len;
	double clickX, clickY;
	getWindowDialog* getWndDlg;
	QTimer* timer, * waitTimer;
	HWND wndHandle;
	RECT rect;
	LPARAM pressPos;
	QClipboard* clipBoard;

private slots:
	void initialization(HWND);
};
