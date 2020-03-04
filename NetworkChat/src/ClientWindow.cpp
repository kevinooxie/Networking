#include "ClientWindow.h"

#include <qscrollbar.h>
#include <qmenubar.h>
#include <qlistwidget.h>

ClientWindow::ClientWindow(QWidget *parent, std::string name, std::string ipaddress, int port)
	: QWidget(parent)
{
	ui_.setupUi(this);
	ui_.textEdit_->setFocus();
	QMenuBar* menubar = new QMenuBar();
	QMenu* filemenu = new QMenu("File");
	menubar->addMenu(filemenu);
	QAction* onlineusrAction = filemenu->addAction("Online Users", this, SLOT(showUsers()));
	this->layout()->setMenuBar(menubar);

	connect(ui_.sendBtn_, SIGNAL(clicked()), this, SLOT(onSendBtn()));
	connect(this, SIGNAL(widgetClosed()), this, SLOT(windowClosing()));
	this->show();

	console(QString("Attempting a connection to %1:%2").arg(QString::fromStdString(ipaddress)).arg(port));
	client_ = new Client(name, ipaddress, port);
	if (!client_->openStatus()) {
		console(QString::fromStdString(client_->message()));
	}

	running_ = false;
	run_ = boost::thread(&ClientWindow::run, this);
}

ClientWindow::~ClientWindow()
{
	if (run_.joinable())
		run_.join();
	if (listen_.joinable())
		listen_.join();
}

void ClientWindow::console(QString& message)
{
	ui_.textBrowser_->append(message);
}

void ClientWindow::onSendBtn()
{
	QString message = ui_.textEdit_->toPlainText();
	if (message.size() != 0) {
		message = QString::fromStdString(client_->name() + ": ") + message;
		client_->send("/m/" + message.toStdString());
		ui_.textEdit_->clear();
		QScrollBar* sb = ui_.textBrowser_->verticalScrollBar();
		sb->setValue(sb->maximum());
	}
	ui_.textEdit_->setFocus();
}

void ClientWindow::run()
{
	running_ = true;
	listen();
}

void ClientWindow::listen()
{
	listen_ = boost::thread(&ClientWindow::listenThread, this);
}

void ClientWindow::listenThread()
{
	while (running_) {
		std::string message = client_->receive();
		//console(QString::fromStdString(message));
 		if (message.size() > 3) {
			std::string prefix = message.substr(0, 3);
			if (prefix == "/c/") {   // connection confirmed packet
				int endpos = message.find("/e/");  // locate the end suffix
				std::string body(message, 3, endpos - 3);
				int id = std::stoi(body);
				client_->setId(id);
				console(QString("Successfully connect to the Server! ID: %1").arg(client_->id()));
			}
			else if (prefix == "/m/")
			{
				int endpos = message.find("/e/");
				std::string body(message, 3, endpos - 3);
				console(QString::fromStdString(body));
			}
			else if (prefix == "/i/")
			{
				std::string text = "/i/" + std::to_string(client_->id()) + "/e/";
				client_->send(text);
			}
			else if (prefix == "/u/") {    //   /u/Li,Yan,Jack/e/
				allusers_.clear();
				std::size_t endpos = message.find("/e/");   // locate the suffix
				std::size_t start = 3;
				std::size_t found = message.find_first_of(",", start);
				while (found < endpos && found != std::string::npos) { 
					std::string name(message, start, found-start);
					allusers_.push_back(name);
					start = found + 1;
					found = message.find_first_of(",", start);
				}
				if (start < endpos) {   // dont forget the last one  or  if only have one user
					std::string name(message, start, endpos-start);
					allusers_.push_back(name);
				}
			}
		}
	}
}

void ClientWindow::closeEvent(QCloseEvent* event)
{
	emit widgetClosed();
	QWidget::closeEvent(event);
}

void ClientWindow::windowClosing()
{
	std::string disconnect = "/d/" + std::to_string(client_->id()) + "/e/";
	client_->send(disconnect);
	running_ = false;
	client_->close();
}

void ClientWindow::showUsers()
{
	QListWidget* userswid = new QListWidget();
	userswid->setAttribute(Qt::WA_DeleteOnClose);
	userswid->setWindowTitle("Online Users");
	for (int i = 0; i < allusers_.size(); ++i) {
		new QListWidgetItem(QString::fromStdString(allusers_[i]), userswid);
	}
	userswid->show();
}