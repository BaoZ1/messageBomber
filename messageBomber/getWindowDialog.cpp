#include "getWindowDialog.h"

getWindowDialog::getWindowDialog(QWidget* parent)
	: QWidget(parent)
{
	classL = new QLabel(this);
	nameL = new QLabel(this);
	warning = new QLabel(this);
	classLE = new QLineEdit(this);
	nameLE = new QLineEdit(this);
	enterB = new QPushButton(this);

	setFixedSize(400, 150);
	setWindowTitle(tr("select window"));
	setWindowIcon(QIcon(":/messageBomber/Resources/messageBomber.ico"));

	nameL->setGeometry(20, 10, 170, 30);
	nameL->setText(tr("window name"));

	nameLE->setGeometry(20, 40, 170, 30);
	nameLE->setFocus();
	connect(nameLE, &QLineEdit::textChanged, [&] {warning->setText(NULL); });

	classL->setGeometry(210, 10, 170, 30);
	classL->setText(tr("window class"));

	classLE->setGeometry(210, 40, 170, 30);
	connect(classLE, &QLineEdit::textChanged, [&] {warning->setText(NULL); });

	warning->setGeometry(20, 70, 360, 30);

	enterB->setGeometry(210, 100, 170, 30);
	enterB->setText(tr("get handle"));
	enterB->setShortcut(Qt::Key_Return);
	connect(enterB, &QPushButton::clicked, [&] {buttonPressed(); });
}

getWindowDialog::~getWindowDialog()
{
}

void getWindowDialog::buttonPressed()
{
	if (classLE->text().isEmpty() && nameLE->text().isEmpty())
	{
		warning->setText("Fill at least one label");

		return;
	}

	WCHAR* classC = NULL, * nameC = NULL;

	if (!classLE->text().isEmpty())
	{
		classC = new WCHAR[classLE->text().length() + 1];
		classLE->text().toWCharArray(classC);
		classC[classLE->text().length()] = '\0';//似乎要有这个\0才能让系统知道在哪里结尾
	}

	if (!nameLE->text().isEmpty())
	{
		nameC = new WCHAR[nameLE->text().length() + 1];
		nameLE->text().toWCharArray(nameC);
		nameC[nameLE->text().length()] = '\0';
	}

	HWND hwnd = FindWindow(classC, nameC);

	if (hwnd == NULL)
	{
		warning->setText("The specified window was not found");

		return;
	}

	emit getHandle(hwnd);

	classLE->setText(NULL);
	nameLE->setText(NULL);

	hide();

	return;
}