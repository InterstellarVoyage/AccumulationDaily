#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QButtonGroup>

#include <QTcpServer>
#include <QTcpSocket>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
public:

    void initMainWindow();
    void initWindow();
    void initStatusBar();
    void initCheckBoxGroup();
    void init2CheckBoxGroup();
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event); // 鼠标按下
    void mouseMoveEvent(QMouseEvent *event); // 鼠标移动
    void mouseReleaseEvent(QMouseEvent *event); // 鼠标释放
    QByteArray createCommand( int type, int dreg, int data);
    void timerEvent(QTimerEvent *event);

    void sendData();
    void sendHexData(QString& tips, uint8_t* ibuf, uint8_t ilen);

    void printDebugInfo(QString str);
    void printDebugInfo(const char* str);

    void showRecvData(QString tips, QByteArray& buffer);


    QTcpServer *m_tcpServer;

    QList<QTcpSocket*> m_clientList;
    QTcpSocket *m_tcpCliSocket;

    int m_pressMouse;
    QPoint m_startPos;
    QPoint m_windowPos;
    int nAddr;
    QString count;
    QString ad;
    QString element;
    QStringList  arr ;
    QString line;
    QString time;
    QString length;
    QString function_code;
    QString different_code;
    QString format;
    QString start;

    QStringList  nShow ;
    QStringList  pShow ;
    QStringList  mShow ;
    QStringList  sShow ;

    int k;
    int m_recvHex;

    int m_recvDex;

    int m_recvasc2;

    int m_sendHex;

    int m_sendasc2;

    int m_sendTimer;
    QByteArray sendBuf;
    QString  cmdData;
    int m_showTimestamp;
    int m_sendNewline;

    int m_sendTimerId;

    int m_rxCnt;
    int m_txCnt;

    int m_cliConnected;
    int m_svrConnected;

    // 状态栏相关
    QLabel* m_stsPinned;
    QLabel* m_stsDebugInfo;
    QLabel* m_stsRx;
    QLabel* m_stsTx;
    QLabel* m_stsResetCnt;
    QLabel* m_stsCopyright;

    QLabel* m_stsExit;

signals:
    void sig_exit();

private slots:
    void readyRead();

    void svr_newConnect(void);
    void svr_disconnect(void);
   // void initCheckBoxGroup();
    void cli_connected(void);
    void cli_disconnected(void);
    void cli_receiveData(void);

    //void on_btnOpen_clicked();

    void on_btnSaveRecv_clicked();

    void on_btnClearRecv_clicked();

    void on_ckRecvHex_stateChanged(int arg1);

    void on_ckRecvDex_stateChanged(int arg1);

    void on_ckRecvasc2_stateChanged(int arg1);

    void on_ckTimestamp_stateChanged(int arg1);

    void on_btnSend_clicked();

    void on_btnClearSend_clicked();



    void on_ckSendTimer_stateChanged(int arg1);

    void on_chSendNewline_stateChanged(int arg1);

    void on_btnConnect_clicked();

    void on_btnClearSend_2_clicked();
    void on_btnClearRecv_2_clicked();

    void on_chSendHex_stateChanged(int arg1);

    void on_chSendAsc2_stateChanged(int arg1);



private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
