#include "NetworkChat.h"
#include "Login.h"

NetworkChat::NetworkChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui_.setupUi(this);

	connect(ui_.loginBtn_, SIGNAL(clicked()), this, SLOT(onLoginBtn()));
	connect(ui_.exitBtn_, SIGNAL(clicked()), this, SLOT(onExitBtn()));
}


void NetworkChat::onLoginBtn()
{ 
	Login* logwin = new Login();
	logwin->show();
}

void NetworkChat::onExitBtn()
{
	this->close();
}