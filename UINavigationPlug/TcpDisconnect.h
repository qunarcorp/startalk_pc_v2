#ifndef _TCPDISCONNECT_H_
#define _TCPDISCONNECT_H_

#include <QFrame>

class NavigationMianPanel;
class QLabel;
class TcpDisconnect : public QFrame
{
	Q_OBJECT

public:
	TcpDisconnect(NavigationMianPanel* pMainPanel, QWidget *parent = nullptr);
	~TcpDisconnect();

public:
	void onRetryConnected();
	void setText(const QString& text);

private:
	void mousePressEvent(QMouseEvent *e);

private:
	NavigationMianPanel* _pMainPanel;
	QLabel*              _pTextLabel;
	bool                 _isRetryConnect;

};

#endif // _TCPDISCONNECT_H_

