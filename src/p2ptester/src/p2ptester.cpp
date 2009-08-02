#include <stdio.h>
#include <stdlib.h>

#include <QApplication>
#include <QMessageBox>

#include "p2ptester.h"
#include "downloadagent.h"

P2PTester::P2PTester(QWidget *parent): QMainWindow(parent)
{
	setupUi(this);
	
	comboBox->addItem("stun.ivao.aero");
	comboBox->addItem("xivap.linuxpunk.org");
	
	textEdit->setHtml("<strong><center>" + tr("Run the test to see a result!") + "</center></strong>");
	
	statusBar()->showMessage(tr("Ready"));
}

void P2PTester::on_pushButton_clicked()
{
	QString server = tr("Running test on %1...").arg(comboBox->currentText());
	QString status = "??";
	QString mode = "??";
	QString conclusion = tr("P2P test returned invalid result");
	QString description = tr("Something went wrong, you should never see this message!");
	
	Stun::StunClient stunClient;
	stunClient.SetServer(comboBox->currentText().toStdString().c_str());
	Stun::StunType stunType = stunClient.RunTest();
	switch(stunType) {
		case Stun::StunError:
			mode = "ERROR";
			status = mode;
			conclusion = tr("P2P test failed");
			description = tr("STUN client test failed, make sure your server address is correct!");
			break;
			
		case Stun::StunOpen:
			mode = "Open";
			description = tr("You have a fully P2P compatible network configuration.");
			
		case Stun::StunIndependentFilter:
			mode = "Independent Filter";
			description = tr("You are behind a Cone NAT device.");

			status = "<font color='green'>OK</font>";
			conclusion = tr("You can use P2P without problems.");

			break;
			
		case Stun::StunDependentFilter:
			mode = "Dependent Filter";
			description = tr("You are behind a Restricted NAT device.");
			
		case Stun::StunPortDependedFilter:
			mode = "Port Dependent Filter";
			description = tr("You are behind a Port Restricted NAT device.");
			
		case Stun::StunDependentMapping:
			mode = "Dependent Mapping";
			description = tr("You are behind a symmetric NAT device.");
			
			conclusion = tr("You can use P2P only when all others have status OPEN.");
			status = "<font color='yellow'>PASSIVE</font>";
			break;
			
		case Stun::StunFirewall:
			mode = "Firewall";
			description = tr("You are behind a restrictive firewall.");
			
		case Stun::StunBlocked:
			mode = "Blocked";
			description = tr("Either the STUN server was not online, or you are behind a very restrictive firewall.");
			
			conclusion = tr("You cannot use P2P");
			status = "<font color='red'>BLOCKED</font>";
			break;
	}
	
	QString hairport = "";
	if(stunType != Stun::StunError) {
		hairport = "<br/>";
		hairport += stunClient.Hairpin() ? "Does hairpin" : "No hairpinning";
		hairport += ", ";
		hairport += stunClient.PresPort() ? "preserves ports" : "does not preserve ports";
	}

	textEdit->setHtml(server + QString("<hr><table border='0'>"
									   "<tr>"
									     "<td bgcolor='gray'>"
									       "<font size='+1'><br/>&nbsp;%1&nbsp;<br/></font>"
									     "</td>"
									     "<td width='5'>&nbsp;</td>"
									     "<td valign='top'>"
									       "<font size='+1'><strong>%2</strong></font>%3<p/>%4"
									     "</td>"
									   "</tr></table><hr><div align='right'><i>%5</i></div>")
						.arg(status).arg(mode).arg(hairport).arg(description).arg(conclusion));
	
	statusBar()->showMessage("Test result: " + mode);
	
	if(checkBox->isChecked())
		sendResult(stunType, stunClient.Hairpin(), stunClient.PresPort());
}

void P2PTester::sendResult(Stun::StunType type, bool hairpin, bool presport)
{
	QString url = QString("http://xivap.linuxpunk.org/p2pstats.php?type=%1&hairpin=%2&presport=%3")
					.arg(type).arg(hairpin).arg(presport);
	//QMessageBox::information(this, "data", url);
	downloadAgent->download(url, NULL, NULL);
	//QMessageBox::information(this, tr("Sending information"),
	//						 tr("Collected follwoing information:<hr>"
	//							"P2P type: %1 Hairpinning: %2 Port preserving: %3<hr>").arg(type).arg(hairpin).arg(presport));	
}

// MAIN -------------------------------------------------

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	DownloadAgent agent;
	
	QMainWindow *window = new P2PTester;
	window->show();
	return app.exec();
}

