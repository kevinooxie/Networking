#include "Login.h"

#include <string>
#include <QtWidgets/QMessageBox>

#include "ClientWindow.h"
#include "Client.h"

Login::Login(QWidget* parent)
	: QWidget(parent)
{
	ui_.setupUi(this);

	connect(ui_.loginBtn_, SIGNAL(clicked()), this, SLOT(onLoginBtn()));
}

Login::~Login()
{

}

void Login::onLoginBtn()
{
	std::string name = ui_.nameEdit_->text().toStdString();
	std::string address = ui_.ipEdit_->text().toStdString();
	int port = ui_.portEdit_->text().toInt();

	login(name, address, port);
}

bool Login::login(std::string name, std::string ipaddress, int port)
{
	ClientWindow* cwin = new ClientWindow(0, name, ipaddress, port);
	return true;
}