#pragma once

#include <QWidget>
#include "ui_ClientWindow.h"
#include "Client.h"
#include "boost/thread.hpp"

class ClientWindow : public QWidget
{
	Q_OBJECT

public:
	ClientWindow(QWidget *parent = Q_NULLPTR, std::string name = "", std::string ipaddress = "", int port = -1);
	~ClientWindow();

	void run();

signals:
	void widgetClosed();

public slots:
	void console(QString& message);
	void onSendBtn();
	void windowClosing();
	void showUsers();

protected:
	void closeEvent(QCloseEvent * event);

private:
	Ui::ClientWindow ui_;

	boost::thread run_, listen_;
	Client* client_;

	bool running_;

	std::vector<std::string> allusers_;

	void listen();
	void listenThread();
};
