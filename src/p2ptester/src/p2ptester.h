#ifndef P2PTESTER_H
#define P2PTESTER_H

#include "ui_p2ptester.h"

#define NO_PTYPES
#include "../../stund/StunClient.h"

// Main application class -----------------------

class P2PTester: public QMainWindow, private Ui::P2PTester {
Q_OBJECT

public:
	P2PTester(QWidget *parent = 0);

private slots:
	void on_pushButton_clicked();

private:
	void sendResult(Stun::StunType type, bool hairpin, bool presport);
};

#endif

