#pragma once

#include <QtWidgets/QWidget>
#include "ui_Login.h"

class Login : public QWidget
{
	Q_OBJECT

public:

	Login(QWidget* parent = 0);
	virtual ~Login();

public slots:
	void onLoginBtn();

private:
	Ui::LoginWin ui_;

	bool login(std::string name, std::string ipaddress, int port);
};