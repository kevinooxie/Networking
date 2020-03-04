#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NetworkChat.h"

class NetworkChat : public QMainWindow
{
	Q_OBJECT

public:
	NetworkChat(QWidget *parent = Q_NULLPTR);

public slots:
	void onLoginBtn();
	void onExitBtn();

private:
	Ui::NetworkChatClass ui_;
};
