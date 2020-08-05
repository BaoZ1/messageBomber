#include "messageBomber.h"
#include <QPainter>
#include <windows.h>
#include <ctime>

messageBomber::messageBomber(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/messageBomber/Resources/messageBomber.ico"));

	getWndDlg = new getWindowDialog();
	connect(getWndDlg, SIGNAL(getHandle(HWND)), this, SLOT(initialization(HWND)));

	getWndDlg->show();

	wndHandle = NULL;
	rect = { 0, 0, 0, 0 };

	timer = new QTimer(this);

	clipBoard = QApplication::clipboard();

	connect(timer, &QTimer::timeout, [&] {send(); });
	connect(ui.scheduleCB, &QCheckBox::stateChanged, [&] {ui.scheduledTime->setEnabled(ui.scheduleCB->isChecked()); });
	connect(ui.backBtn, &QPushButton::clicked, [&] {getWndDlg->show(); hide(); });
	connect(ui.startBtn, &QPushButton::clicked, [&] {if (ui.scheduleCB->isChecked())
	{
		wait();
	}
	else
	{
		run();
	}});
	connect(ui.cancelBtn, &QPushButton::clicked, [&] {finish(); });

	ui.pauseBtn->hide();
	ui.cancelBtn->hide();
	ui.progressL->hide();
	ui.countL->hide();
	ui.progress->hide();
	ui.waitL->hide();
	ui.timeL->hide();
	ui.nowL->hide();
	ui.timeCancelBtn->hide();
}

void messageBomber::initialization(HWND hwnd)
{
	running = false;

	wndHandle = hwnd;

	WCHAR wndClassW[50] = { '\0' };
	GetClassName(hwnd, wndClassW, 50);
	ui.wndClass->setText(tr("window class: ") + QString::fromWCharArray(wndClassW));

	WCHAR wndTitleW[50] = { '\0' };
	GetWindowText(hwnd, wndTitleW, 50);
	ui.wndTitle->setText(tr("window title: ") + QString::fromWCharArray(wndTitleW));

	show();
}

void messageBomber::run()
{
	running = true;
	count = 0;
	if (ui.msgTextLE->text() == NULL)
	{
		defaultMsg = true;
	}
	else
	{
		defaultMsg = false;
		clipBoard->setText(ui.msgTextLE->text());
	}

	clickX = ui.clickXS->value();
	clickY = ui.clickYS->value();

	ui.progress->setMaximum(ui.msgNumS->value());
	ui.progress->setValue(0);
	ui.countL->setText(QString::number(count) + QString("/") + QString::number(ui.msgNumS->value()));

	ui.msgTextLE->setDisabled(true);
	ui.msgNumS->setDisabled(true);
	ui.pauseTimeS->setDisabled(true);

	ui.clickXL->hide();
	ui.clickXS->hide();
	ui.clickYL->hide();
	ui.clickYS->hide();
	ui.scheduleCB->hide();
	ui.scheduledTime->hide();
	ui.startBtn->hide();
	ui.pauseBtn->show();
	ui.cancelBtn->show();
	ui.progress->show();
	ui.progressL->show();
	ui.countL->show();

	ShowWindow(wndHandle, SW_SHOWNORMAL);

	SetForegroundWindow(wndHandle);

	SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	GetWindowRect(wndHandle, &rect);
	pressPos = MAKELPARAM(clickX * (rect.right - rect.left), clickY * (rect.bottom - rect.top));

	connect(ui.pauseBtn, &QPushButton::clicked, [&] {
		if (timer->isActive())
		{
			timer->stop();
			ui.pauseBtn->setText(tr("continue"));
			SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			SendMessage(wndHandle, WM_LBUTTONDOWN, MK_LBUTTON, pressPos);
			SendMessage(wndHandle, WM_LBUTTONUP, MK_LBUTTON, pressPos);
			timer->start();
			ui.pauseBtn->setText(tr("pause"));
			SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
			}
	);

	SendMessage(wndHandle, WM_LBUTTONDOWN, MK_LBUTTON, pressPos);
	SendMessage(wndHandle, WM_LBUTTONUP, MK_LBUTTON, pressPos);

	timer->setInterval(ui.pauseTimeS->value());
	timer->start();
}

void messageBomber::wait()
{
	ui.msgTextLE->setDisabled(true);
	ui.msgNumS->setDisabled(true);
	ui.pauseTimeS->setDisabled(true);

	ui.clickXL->hide();
	ui.clickXS->hide();
	ui.clickYL->hide();
	ui.clickYS->hide();
	ui.scheduleCB->hide();
	ui.scheduledTime->hide();
	ui.startBtn->hide();

	const time_t nowTime = time(0);
	tm* ltime = localtime(&nowTime);
	ui.nowL->setText(QString("now: %1:%2:%3").arg(ltime->tm_hour, 2, 10, QChar('0'))
					 .arg(ltime->tm_min, 2, 10, QChar('0'))
					 .arg(ltime->tm_sec, 2, 10, QChar('0')));

	ui.waitL->show();
	ui.timeL->show();
	ui.nowL->show();
	ui.timeCancelBtn->show();

	ui.timeL->setText(ui.scheduledTime->text());

	waitTimer = new QTimer(this);
	waitTimer->setInterval(500);
	connect(ui.timeCancelBtn, &QPushButton::clicked, [&] {
		waitTimer->stop();

		ui.msgTextLE->setDisabled(false);
		ui.msgNumS->setDisabled(false);
		ui.pauseTimeS->setDisabled(false);

		ui.clickXL->show();
		ui.clickXS->show();
		ui.clickYL->show();
		ui.clickYS->show();
		ui.scheduleCB->show();
		ui.scheduledTime->show();
		ui.startBtn->show();

		ui.waitL->hide();
		ui.timeL->hide();
		ui.nowL->hide();
		ui.timeCancelBtn->hide();
			}
	);
	connect(waitTimer, &QTimer::timeout, [&] {
		const time_t now = time(0);
		tm* ltm = localtime(&now);
		if (ltm->tm_hour == ui.scheduledTime->time().hour()
			&& ltm->tm_min == ui.scheduledTime->time().minute()
			&& ltm->tm_sec == ui.scheduledTime->time().second())
		{
			waitTimer->stop();

			ui.waitL->hide();
			ui.timeL->hide();
			ui.nowL->hide();
			ui.timeCancelBtn->hide();

			run();

			return;
		}
		else
		{
			ui.nowL->setText(QString("now: %1:%2:%3").arg(ltm->tm_hour, 2, 10, QChar('0'))
							 .arg(ltm->tm_min, 2, 10, QChar('0'))
							 .arg(ltm->tm_sec, 2, 10, QChar('0')));
		}
			}
	);
	waitTimer->start();
}

void messageBomber::send()
{
	if (defaultMsg)
	{
		clipBoard->setText(QString::number(count));
	}

	SendMessage(wndHandle, WM_PASTE, 0, 0);

	SendMessage(wndHandle, WM_KEYDOWN, VK_RETURN, 0);
	SendMessage(wndHandle, WM_KEYUP, VK_RETURN, 0);

	ui.progress->setValue(++count);

	ui.countL->setText(QString::number(count) + QString("/") + QString::number(ui.msgNumS->value()));

	if (count == ui.msgNumS->value())
	{
		timer->stop();

		ui.pauseBtn->hide();

		ui.progressL->setText(tr("done"));
		ui.cancelBtn->setText(tr("finish"));

		SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void messageBomber::finish()
{
	timer->stop();

	SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	running = false;
	count = 0;

	ui.progressL->setText(tr("progress:"));
	ui.cancelBtn->setText(tr("cancel"));

	ui.msgNumS->setDisabled(false);
	ui.pauseTimeS->setDisabled(false);
	ui.msgTextLE->setDisabled(false);

	ui.clickXL->show();
	ui.clickXS->show();
	ui.clickYL->show();
	ui.clickYS->show();
	ui.scheduleCB->show();
	ui.scheduledTime->show();
	ui.startBtn->show();
	ui.pauseBtn->hide();
	ui.cancelBtn->hide();
	ui.progress->hide();
	ui.progressL->hide();
	ui.countL->hide();
}