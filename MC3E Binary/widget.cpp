#include "widget.h"
#include "ui_widget.h"

#include <QNetworkInterface>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initMainWindow();
    initStatusBar();
    initWindow();
    initCheckBoxGroup();
    init2CheckBoxGroup();
}

Widget::~Widget()
{
    delete ui;
}

void stringTohexString(QString& str, QString& hexStr)
{

}
enum type
{
    R,
    W
};
int hexStringToString(QString& hexStr, QString& str)
{
    int ret = 0;
    bool ok;
    QByteArray retByte;
    hexStr = hexStr.trimmed();
    hexStr = hexStr.simplified();
    QStringList sl = hexStr.split(" ");

    foreach (QString s, sl)
    {
        if(!s.isEmpty())
        {
            char c = (s.toInt(&ok,16))&0xFF;
            if (ok)
            {
                retByte.append(c);
            }
            else
            {
                ret = -1;
            }
        }
    }

    str = retByte;

    return ret;
}

int hexStringToHexArray(QString& hexStr, QByteArray& arr)
{
    int ret = 0;
    bool ok;
    hexStr = hexStr.trimmed();
    hexStr = hexStr.simplified();
    QStringList sl = hexStr.split(" ");

    foreach (QString s, sl)
    {
        if(!s.isEmpty())
        {
            char c = (s.toInt(&ok,16))&0xFF;
            if (ok)
            {
                arr.append(c);
            }
            else
            {
                ret = -1;
            }
        }
    }

    return ret;
}

int hexArrayToString(QByteArray& hexArr, QString& str)
{
    int ret = 0;
    str = hexArr.toHex(' ').toLower();
    return ret;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_stsExit) // 程序退出
    {
        //判断事件
        if(event->type() == QEvent::MouseButtonPress)
        {
            // TODO：直接退出还是发信号？
            emit sig_exit();
            return true; // 事件处理完毕
        }
        else
        {
            return false;
        }
    }
    else if (watched == m_stsPinned) // 置顶
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            static bool isTop = true;
            Qt::WindowFlags winFlags  = Qt::Dialog;
            winFlags = winFlags | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
            if (isTop)
            {
                winFlags |= Qt::WindowStaysOnTopHint;
                setWindowFlags(winFlags);
                showNormal();
                QPixmap pinnedIcon(":/images/pinned.bmp");
                m_stsPinned->setPixmap(pinnedIcon);
                isTop = false;
            }
            else
            {
                winFlags  &= ~Qt::WindowStaysOnTopHint;
                setWindowFlags(winFlags);
                showNormal();
                QPixmap pinnedIcon(":/images/unpinned.bmp");
                m_stsPinned->setPixmap(pinnedIcon);
                isTop = true;
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (watched == m_stsResetCnt)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            m_stsRx->setText("RX: 0");
            m_stsTx->setText("TX: 0");
            m_rxCnt = m_txCnt = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return QWidget::eventFilter(watched, event);
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    //当鼠标左键点击时.
    if (event->button() == Qt::LeftButton)
    {
        //记录鼠标的世界坐标
        m_startPos = event->globalPos();
        //记录窗体的世界坐标
        m_windowPos = this->frameGeometry().topLeft();
        m_pressMouse = 1;
    }
    else if(event->button() == Qt::RightButton)
    {

    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (m_pressMouse)
        {
        //移动中的鼠标位置相对于初始位置的相对位置
        QPoint relativePos = event->globalPos() - m_startPos;
        //然后移动窗体即可
        this->move(m_windowPos + relativePos );

        }
    }
    else if(event->button() == Qt::RightButton)
    {

    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressMouse = 0;
}

// 对主窗口的初始化
void Widget::initMainWindow()
{
    setWindowTitle(tr("MC协议3E Binary V0.01"));
    setMinimumSize(480, 320);

    Qt::WindowFlags winFlags  = Qt::Dialog;
    winFlags = winFlags | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;

    setWindowFlags(winFlags);
}

void Widget::initWindow()
{
    m_pressMouse = 0;
    m_recvHex = 1;
    m_sendHex = 1;
    m_sendasc2=0;
    m_recvasc2=0;
    m_sendTimer = 0;
    m_sendTimerId = 0;

    m_showTimestamp = 0;
    m_sendNewline = 0;

    m_rxCnt = 0;
    m_txCnt = 0;

    m_cliConnected = 0;
    m_svrConnected = 0;

    /////////////////////////////////////////////////////////////

    ui->btnClearRecv->setFlat(true);
   // ui->btnClearSend->setFlat(true);

    ui->txtRecv->setPlaceholderText("Receive Data here");
   // ui->txtSend->setPlaceholderText("Send Data here");
   // ui->txtSend->setPlainText("hello world");

    QFont qFont;
    qFont.setBold(true);

   // ui->btnOpen->setFont(qFont);
   // ui->btnOpen->setText(tr("监听端口"));
   // ui->btnOpen->setIconSize(ui->btnOpen->rect().size());
   // ui->btnOpen->setIcon(QIcon(":images/notopened.ico"));
    ui->btnConnect->setFont(qFont);
    ui->btnConnect->setText(tr("连接"));
    ui->btnSend->setFont(qFont);
    ui->btnSend->setText(tr("读取"));
    //ui->btnConnect->setIconSize(ui->btnOpen->rect().size());
    //ui->btnConnect->setIcon(QIcon(":images/notopened.ico"));

    ui->txtInterval->setText("1000");
    /////////////////////////////////////////////////////////////
    QStringList list;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, ipAddressesList)
    {
        if (addr.toIPv4Address())
        {
            //qDebug() << addr.toString();
            list.append(addr.toString());
        }
    }
    foreach (QHostAddress addr, ipAddressesList)
    {
        if (addr.toIPv4Address())
        {
            //qDebug() << addr.toString();
            list.append(addr.toString());
        }
    }

    //list.clear();
    //list << "127.0.0.1";
   // ui->cbLocalIP->addItems(list);

    list.clear();
    list << "8000";
    //ui->cbLocalPort->addItems(list);

    list.clear();
    list << "192.168.0.10";
    ui->cbRemoteIP->addItems(list);

    list.clear();
    list << "5000";
    ui->cbRemotePort->addItems(list);

    ui->txtRecv->setPlaceholderText("Receive data here");
   // ui->txtSend->setPlaceholderText("Send data here");
    //ui->txtSend->setPlainText("hello world");

    // client
    m_tcpCliSocket = new QTcpSocket();
    connect(m_tcpCliSocket, SIGNAL(connected()), this, SLOT(cli_connected())); // 客户端连接
    connect(m_tcpCliSocket, SIGNAL(disconnected()), this, SLOT(cli_disconnected())); // 客户端断开连接
    connect(m_tcpCliSocket, SIGNAL(readyRead()), this, SLOT(cli_receiveData())); // 客户端接收数据

    // server
    m_tcpServer = new QTcpServer();
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(svr_newConnect()));
}


void Widget::initStatusBar()
{

   // ui->statusbar->setMinimumHeight(22);
    //ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 不显示边框
   // ui->statusbar->setSizeGripEnabled(false);//去掉状态栏右下角的三角

    m_stsPinned = new QLabel();
    m_stsDebugInfo = new QLabel();
    m_stsRx = new QLabel();
    m_stsTx = new QLabel();
    m_stsResetCnt = new QLabel();
    m_stsCopyright = new QLabel();
    m_stsExit = new QLabel();

    // 置顶图标
    m_stsPinned->installEventFilter(this); // 安装事件过滤，以便获取其单击事件
    m_stsPinned->setMinimumWidth(20);
    // 贴图
    QPixmap pinnedIcon(":/images/unpinned.bmp");
    m_stsPinned->setPixmap(pinnedIcon);
  //  ui->statusbar->addWidget(m_stsPinned);


    m_stsDebugInfo->setMinimumWidth(this->width()/2);
  //  ui->statusbar->addWidget(m_stsDebugInfo);

    m_stsRx->setMinimumWidth(64);
   // ui->statusbar->addWidget(m_stsRx);
    m_stsRx->setText("RX: 0");

    m_stsTx->setMinimumWidth(64);
    //ui->statusbar->addWidget(m_stsTx);
    m_stsTx->setText("TX: 0");

    m_stsResetCnt->installEventFilter(this);
    m_stsResetCnt->setFrameStyle(QFrame::Plain);
    m_stsResetCnt->setText("复位计数");
    m_stsResetCnt->setMinimumWidth(32);
   // ui->statusbar->addWidget(m_stsResetCnt);

    printDebugInfo("欢迎使用");


    // 退出图标
    m_stsExit->installEventFilter(this); // 安装事件过滤，以便获取其单击事件
    m_stsExit->setToolTip("Exit App");
    m_stsExit->setMinimumWidth(32);

    connect(this, &Widget::sig_exit, qApp, &QApplication::quit); // 直接关联到全局的退出槽

}

void Widget::printDebugInfo(QString str)
{
    m_stsDebugInfo->setText(str);
}

void Widget::printDebugInfo(const char* str)
{
    QString tmp = str;
    m_stsDebugInfo->setText(tmp);
}

void Widget::timerEvent(QTimerEvent *event)
{
    //qDebug() << "Timer ID:" << event->timerId();
    sendData();
}
 QByteArray Widget::createCommand(int type, int dreg, int data)
      {


          switch (type)
          {

              case 1:
                  {
                      sendBuf.resize(21);

                      //副标题
                      sendBuf[0] = 0x50;
                      sendBuf[1] = 0x00;
                      //网络号
                      sendBuf[2] = 0x00;
                      //PLC号
                      sendBuf[3] = 0xFF;
                      //IO
                      sendBuf[4] = 0xFF;
                      sendBuf[5] = 0x03;
                      //站号
                      sendBuf[6] = 0x00;
                      //字节数
                      sendBuf[7] = 0x0C;
                      sendBuf[8] = 0x00;
                      //等待长度
                      sendBuf[9] = 0x10;
                      sendBuf[10] = 0x00;
                      //命令
                      sendBuf[11] = 0x01;
                      sendBuf[12] = 0x04;
                      //子命令
                      sendBuf[13] = 0x00;
                      sendBuf[14] = 0x00;
                      //起始寄存器地址
                      sendBuf[15] =(dreg & 0xff);
                      sendBuf[16] = ((dreg & 0xff00) >> 8);
                      sendBuf[17] = ((dreg & 0xff0000) >> 16);
                      if (ui->cbxRegType->currentText()==("R"))
                      {
                          //寄存器类型-R类
                          sendBuf[18] = 0xAF;
                      }
                      if (ui->cbxRegType->currentText()==("ZR"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0xB0;
                      }
                      if (ui->cbxRegType->currentText()==("M"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x90;
                      }
                      if (ui->cbxRegType->currentText()==("X"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x9C;
                      }
                      if (ui->cbxRegType->currentText()==("Y"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x9D;
                      }
                      if (ui->cbxRegType->currentText()==("D"))
                      {
                          //寄存器类型-D类
                          sendBuf[18] = 0xA8;
                      }

                      //寄存器连续长度
                      sendBuf[19] = (data & 0xff);
                      sendBuf[20] = ((data & 0xff00) >> 8);
                      return sendBuf;
                  }
              case 2:
                  {

                      sendBuf.resize(23);
                      //副标题
                      sendBuf[0] = 0x50;
                      sendBuf[1] = 0x00;
                      //网络号
                      sendBuf[2] = 0x00;
                      //PLC号
                      sendBuf[3] = 0xFF;
                      //IO
                      sendBuf[4] = 0xFF;
                      sendBuf[5] = 0x03;
                      //站号
                      sendBuf[6] = 0x00;
                      //字节数
                     // sendBuf[7] = 0x12;
                      sendBuf[7] = 0x0E;
                      sendBuf[8] = 0x00;
                      //等待长度
                      sendBuf[9] = 0x10;
                      sendBuf[10] = 0x00;
                      //命令
                      sendBuf[11] = 0x01;
                      sendBuf[12] = 0x14;
                      //子命令
                      sendBuf[13] = 0x00;
                      sendBuf[14] = 0x00;
                      //起始寄存器地址
                      sendBuf[15] = (dreg & 0xff);
                      sendBuf[16] = ((dreg & 0xff00) >> 8);
                      sendBuf[17] = ((dreg & 0xff0000) >> 16);
                      //if (this.cbxRegType.Text.Equals("R"))
                      //{
                      //    //寄存器类型-R类
                      //    sendBuf[18] = 0xAF;
                      //}
                      //if (this.cbxRegType.Text.Equals("D"))
                      //{
                      //    //寄存器类型-D类
                      //    sendBuf[18] = 0xA8;
                      //}
                      if (ui->cbxRegType->currentText()==("R"))
                      {
                          //寄存器类型-R类
                          sendBuf[18] = 0xAF;
                      }
                      if (ui->cbxRegType->currentText()==("ZR"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0xB0;
                      }
                      if (ui->cbxRegType->currentText()==("M"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x90;
                      }
                      if (ui->cbxRegType->currentText()==("X"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x9C;
                      }
                      if (ui->cbxRegType->currentText()==("Y"))
                      {
                          //寄存器类型-ZR类
                          sendBuf[18] = 0x9D;
                      }
                      if (ui->cbxRegType->currentText()==("D"))
                      {
                          //寄存器类型-D类
                          sendBuf[18] = 0xA8;
                      }
                      //寄存器连续长度

                      sendBuf[19] = (data & 0xff);
                      sendBuf[20] = ((data & 0xff00) >> 8);
                      //寄存器写入的值
                      //int p = int.Parse(txtValueWrite.Text);
                      int p,v;
                     //int.TryParse(txtValueWrite.Text, out v);
                     // if (v >= 32767)
                     // {
                     //     MessageBox.Show("数据超出范围!");
                     //     return null;
                     // }

                      //if (isNumberic(txtValueWrite.Text, out v))
                      //{
                      //    int.TryParse(txtValueWrite.Text, out p);

                      //}
                      //else
                      //{
                          p= ui->txtValueWrite->text().toInt();
                      // }
                      if (p >65535)
                      {
                          QMessageBox::information(nullptr, "Title", "数据超出范围");
                          return nullptr;
                      }

                      sendBuf[21] = (p & 0xff);
                      sendBuf[22] = ((p & 0xff00) >> 8);
                      return sendBuf;
                  }
              default: return nullptr;
          }

      }

void Widget::sendData()
{
    QByteArray sData;
    if(function_code=="0401")//asc2读
    {
        if (m_sendasc2 == 1)
        {
            line = "500000FF03FF00";
            time="0010";
            different_code="0000";
            //ad=QString("%1").arg(nAddr);
            ad = QString("%1").arg(nAddr, 6, 10, QLatin1Char('0'));
           // count=ui->txtCount->text();
            count=QString("%1").arg(k, 4, 10, QLatin1Char('0')).toUpper();

            if (element=="M")
            {
                //寄存器类型-R类
                element= "M*";

            }
            if (element=="R")
            {
                //寄存器类型-ZR类
               element= "R*";
            }
            if (element=="ZR")
            {
                //寄存器类型-ZR类
               element= "ZR";
            }
            if (element=="M")
            {
                //寄存器类型-ZR类
                element= "M*";
            }
            if (element=="X")
            {
                //寄存器类型-ZR类
                element= "X*";
            }
            if (element=="Y")
            {
                //寄存器类型-ZR类
               element= "Y*";
            }
            if (element=="D")
            {
                //寄存器类型-D类
                element= "D*";
            }
            length=QString("%1").arg((time.length() + function_code.length() + different_code.length() + element.length() + ad.length() + count.length()), 4, 16, QLatin1Char('0'));

            line += length;
            line += time;
            line += function_code;
            line += different_code;
            line += element;
            line +=ad;
            line += count;
        }

    }
    if(function_code=="0104")//16进制读
    {

        if (m_sendHex == 1)
        {

            start = ui->txtMorDStart->text();
            int k = ui->txtCount->text().toInt();
            sData = createCommand(1, start.toInt(), k);

        }
    }
    if(function_code=="1401")//asc2写
    {
        if (m_sendasc2 == 1)
        {
           line = "500000FF03FF00";
           time="0010";
           different_code="0000";
            //ad=QString("%1").arg(nAddr);
           ad = QString("%1").arg(nAddr, 6, 10, QLatin1Char('0'));
           // count=ui->txtCount->text();
           count=QString("%1").arg(k, 4, 16, QLatin1Char('0')).toUpper();
           QString sumend = "";
           QString sumtemp = "";
           int k1 = ui->txtValueWrite->text().length();
           QString sum = ui->txtValueWrite->text();
           int DifferenceValue = 4 * k - k1;
           if (DifferenceValue == 0)
           {
               sumend = sum;
           }
           else if (DifferenceValue > 0)
           {
               sumend =QString("%1").arg(sum.toInt(), 4 * k, 10, QLatin1Char('0'));
           }
            if (element=="M")
            {
                //寄存器类型-R类
                element= "M*";

            }
            if (element=="R")
            {
                //寄存器类型-ZR类
               element= "R*";
            }
            if (element=="ZR")
            {
                //寄存器类型-ZR类
               element= "ZR";
            }
            if (element=="M")
            {
                //寄存器类型-ZR类
                element= "M*";
            }
            if (element=="X")
            {
                //寄存器类型-ZR类
                element= "X*";
            }
            if (element=="Y")
            {
                //寄存器类型-ZR类
               element= "Y*";
            }
            if (element=="D")
            {
                //寄存器类型-D类
                element= "D*";
            }
            length=QString("%1").arg((time.length() + function_code.length() + different_code.length() + element.length() + ad.length() + count.length()+k * 4), 4, 16, QLatin1Char('0')).toUpper();

            line += length;
            line += time;
            line += function_code;
            line += different_code;
            line += element;
            line +=ad;
            line += count;
            line += sumend;
        }
    }
    if(function_code=="0114")//16进制写
    {
        if (m_sendHex == 1)
        {
            start = ui->txtMorDStart->text();
            int k = ui->txtCount->text().toInt();
            sData = createCommand(2, start.toInt(), k);
        }
    }

    cmdData = line;

    QString sendStr = cmdData.toLatin1();
    QByteArray sendData;
    QString showStr;

    if (m_sendHex == 1)
    {
       // hexStringToHexArray(sendStr, sendData);
        sendData=sData;
    }
    else if (m_sendasc2 == 1)
    {
        sendData = sendStr.toLatin1();
    }
    if (m_sendNewline == 1)
    {
        sendData.append(0x0d);
        sendData.append(0x0a);
    }
    //qDebug() << sendData;
//    char*  ch;
//    //QByteArray ba = ui->cliText->text().toLatin1(); // must
//    ch=ba.data();

    QTcpSocket* tmpSocket = NULL;
    if (m_svrConnected && m_cliConnected)
    {
        tmpSocket = m_tcpCliSocket;
    }
    else if (m_cliConnected)
    {
        tmpSocket = m_tcpCliSocket;
    }
    else if (m_svrConnected)
    {
        tmpSocket = m_clientList.last();
    }

    tmpSocket->write(sendData, sendData.size());

//    m_txCnt += sendData.size();
//    m_stsTx->setText("TX: " + QString::number(m_txCnt));
}

void Widget::sendHexData(QString& tips, uint8_t* ibuf, uint8_t ilen)
{
    QByteArray sendData;

    for (int i = 0; i < ilen; i++)
    {
        sendData[i] = ibuf[i];
    }

    QString tmp = sendData.toHex(' ').toLower();

    //ui->txtSend->appendPlainText(tips + tmp);
}

void Widget::showRecvData(QString tips, QByteArray& buffer)
{
    QString info;
    QString tmpStr;
    QString timeStr = "";  
    QString infoShow;
    QString pinfoShow;
    QVector <int> sum;
    pShow.clear();
    mShow.clear();
    bool ok;
    if (m_recvHex == 1)
    {
        info = buffer.toHex(' ').data();


         nShow = info.split(" ");

         for (int i=11;i<nShow.count();i++)
         {
             pShow.push_back(nShow[i]);
         }
         for (int i=0;i<pShow.count()/2;i++)
         {
             mShow.push_back(pShow[2*i+1]+pShow[2*i]);

             infoShow+=mShow[i].toUpper();

             infoShow+=" ";
         }
         ui->txtRecv->appendHtml(infoShow);
    }
    else if (m_recvDex == 1)
    {
        info = buffer.toHex(' ').data();


         nShow = info.split(" ");

         for (int i=11;i<nShow.count();i++)
         {
             pShow.push_back(nShow[i]);
         }
         for (int i=0;i<pShow.count()/2;i++)
         {
             mShow.push_back((pShow[2*i+1]+pShow[2*i]));

             infoShow+=mShow[i].toUpper();

             infoShow+=" ";
         }
         sShow = infoShow.split(" ");
         for (int i=0;i<sShow.count()-1;i++)
         {
             sum.push_back(sShow[i].toInt(&ok,16));
             pinfoShow+=QString("%1").arg(sum[i]);
             pinfoShow+=" ";
         }
         ui->txtRecv->appendHtml(pinfoShow);

    }
    else if (m_recvasc2 == 1)
    {

        info = QString(buffer);
        ui->txtRecv->appendHtml(info);
    }
   // ui->txtRecv->appendHtml(info);
    //ui->txtRecv->appendHtml(infoShow);

    uint8_t *data = (uint8_t*)buffer.data();

    // 根据值判断做逻辑处理，可做成函数
    if (data[0] == 0xaa && data[1] == 0x55)
    {

    }
}

void Widget::readyRead()
{
    QTcpSocket *tcpSocket = static_cast<QTcpSocket *>(QObject::sender());
    QByteArray buffer = tcpSocket->readAll();
    showRecvData("SERVER> ", buffer);
}

void Widget::svr_newConnect()
{
    printDebugInfo("get new connect");
    QTcpSocket *tcpSocket = m_tcpServer->nextPendingConnection();//新的客户端发起的连接
    QHostAddress clientIp = tcpSocket->peerAddress();//客户端的IP
    quint16 port = tcpSocket->peerPort();//客户端的端口
    if(m_clientList.contains(tcpSocket))
    {
        printDebugInfo(QString("%1:%2 already connected").arg(clientIp.toString()).arg(port));
    }
    else
    {
        printDebugInfo(QString("new connect from %1:%2").arg(clientIp.toString()).arg(port));
        m_clientList.append(tcpSocket);//记录下客户端发起的连接
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(svr_disconnect()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead())); // 数据接收

    }
}

void Widget::svr_disconnect(void)
{
    printDebugInfo(tr("disconnect"));
    QTcpSocket *tcpSocket = static_cast<QTcpSocket *>( QObject::sender() );
    m_clientList.removeOne(tcpSocket);
}

/////////////////////////////////////
void Widget::cli_connected()
{
    printDebugInfo("client connected");
}

void Widget::cli_disconnected()
{
    printDebugInfo("client disconnect");
    m_cliConnected = 0;
    if (m_tcpCliSocket->isOpen())
        m_tcpCliSocket->close();
    ui->btnConnect->setText(tr("连接"));
    ui->btnConnect->setIcon(QIcon(":images/notopened.ico"));
}

void Widget::cli_receiveData()
{
    QByteArray buffer;
    buffer = m_tcpCliSocket->readAll();
    showRecvData("", buffer);
}

//void Widget::on_btnOpen_clicked()
//{
//    if(ui->btnOpen->text()==QString("监听端口"))
//    {
//        QString str=ui->cbLocalIP->currentText();
//        uint16_t port=ui->cbLocalPort->currentText().toUShort();
//        m_tcpServer->close();

//        m_tcpServer->listen(QHostAddress::Any, port);
//        printDebugInfo(QString("listening on %1").arg(port));

//        m_svrConnected = 1;
//       // ui->btnOpen->setText(tr("关闭监听"));
//       // ui->btnOpen->setIcon(QIcon(":images/opened.ico"));
//    }
//    else
//    {
//        m_svrConnected = 0;
//        m_tcpServer->close();
//        uint16_t port=ui->cbLocalPort->currentText().toUShort();
//        printDebugInfo(QString("close port %1").arg(port));
//       // ui->btnOpen->setText(tr("监听端口"));
//        //ui->btnOpen->setIcon(QIcon(":images/notopened.ico"));
//    }
//}

void Widget::on_btnSaveRecv_clicked()
{

}
void Widget:: initCheckBoxGroup()
{
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    pButtonGroup->addButton(ui->ckRecvHex,1);
   // pButtonGroup->addButton(ui->ckRecvasc2, 2);
    pButtonGroup->addButton(ui->ckRecvDex, 3);

}
void Widget:: init2CheckBoxGroup()
{
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    pButtonGroup->addButton(ui->chSendHex,1);
    //pButtonGroup->addButton(ui->chSendAsc2, 2);
    //pButtonGroup->addButton(ui->ckRecvDex, 3);

}

void Widget::on_btnClearRecv_clicked()
{
    ui->txtRecv->clear();
}

void Widget::on_ckRecvHex_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_recvHex = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_recvHex = 0;
    }
}
void Widget::on_ckRecvDex_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_recvDex = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_recvDex = 0;
    }
}
void Widget::on_ckRecvasc2_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_recvasc2 = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_recvasc2 = 0;
    }
}

void Widget::on_ckTimestamp_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_showTimestamp = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_showTimestamp = 0;
    }
}

void Widget::on_btnSend_clicked()
{

     QWidget *currentFocusWidget = this->focusWidget();//QApplication::focusWidget();能够获取聚焦控件
     QString name=currentFocusWidget->objectName();
      k = ui->txtCount->text().toInt();
   element=ui->cbxRegType->currentText();

    //k = ui->txtCount->text().toInt();
   int strNew=ui->txtMorDStart->text().toInt();
   nAddr = QString("%1").arg(strNew, 6, 10, QLatin1Char('0')).toInt();
   if(name=="btnSend")
   {
       if (m_sendasc2 == 1)
       {
            function_code="0401";
       }
       else if (m_sendHex == 1)
       {
            function_code="0104";
       }

   }
   else if(name=="btnWrite")
   {
       if (m_sendasc2 == 1)
       {
            function_code="1401";
       }
       else if (m_sendHex == 1)
       {
            function_code="0114";
       }
   }
    //if (!m_tcpCliSocket->isOpen())
    if (!m_cliConnected && !m_svrConnected)
    {
        printDebugInfo("not connected");
        return;
    }

    if (m_sendTimer)
    {
        if(ui->btnSend->text()==QString("发送"))
        {
            if (m_sendTimerId == 0)
                m_sendTimerId = startTimer(ui->txtInterval->text().toInt());
            ui->btnSend->setText(tr("停止发送"));
        }
        else
        {
            if (m_sendTimerId)
            {
                killTimer(m_sendTimerId);
                m_sendTimerId = 0;
            }
            ui->btnSend->setText(tr("发送"));
        }
    }
    else
    {
        sendData();
    }
}


void Widget::on_btnClearSend_clicked()
{
    //ui->txtSend->clear();
}

void Widget::on_chSendHex_stateChanged(int arg1)
{
//    QString sendStr = ui->txtSend->toPlainText().toLatin1().toLower();
//    QByteArray sendData = sendStr.toLatin1();//ui->txtSend->toPlainText().toLatin1();
//    QString tmpStr;
//    QString showStr;
    if (arg1 == Qt::Checked)
    {
        m_sendHex = 1;
      //  showStr = sendData.toHex(' ').data();
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendHex = 0;
       // hexStringToString(sendStr, showStr); // 用string来转
    }
    //ui->txtSend->setPlainText(showStr);
}
void Widget::on_chSendAsc2_stateChanged(int arg1)
{
    // QString sendStr = ui->txtSend->toPlainText().toLatin1().toLower();
    // QByteArray sendData = sendStr.toLatin1();//ui->txtSend->toPlainText().toLatin1();
     QString tmpStr;
     QString showStr;
     if (arg1 == Qt::Checked)
     {
         m_sendasc2 = 1;
       //  showStr = sendData.toHex(' ').data();
     }
     else if (arg1 == Qt::Unchecked)
     {
         m_sendasc2 = 0;
        // hexStringToString(sendStr, showStr); // 用string来转
     }
     //ui->txtSend->setPlainText(showStr);
}

void Widget::on_ckSendTimer_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_sendTimer = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendTimer = 0;
        if (m_sendTimerId)
        {
            killTimer(m_sendTimerId);
            m_sendTimerId = 0;
            ui->btnSend->setText("发送");
        }
    }
}

void Widget::on_chSendNewline_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        m_sendNewline = 1;
    }
    else if (arg1 == Qt::Unchecked)
    {
        m_sendNewline = 0;
    }
}

void Widget::on_btnConnect_clicked()
{
    if(ui->btnConnect->text()==QString("连接"))
    {
        QHostAddress serverIp;
        if(!serverIp.setAddress(ui->cbRemoteIP->currentText()))
        {
           printDebugInfo("ip error");
           return;
        }

        if (m_tcpCliSocket->isOpen())
            m_tcpCliSocket->close();
        uint16_t port=ui->cbRemotePort->currentText().toUShort();
        m_tcpCliSocket->connectToHost(serverIp, port);
        if (!m_tcpCliSocket->waitForConnected(600))
        {
            printDebugInfo("connect failed");
            return;
        }

        m_cliConnected = 1;
        ui->btnConnect->setText(tr("关闭连接"));
        ui->btnConnect->setIcon(QIcon(":images/opened.ico"));
    }
    else
    {
        m_cliConnected = 0;
        m_tcpCliSocket->close();
        ui->btnConnect->setText(tr("连接"));
        ui->btnConnect->setIcon(QIcon(":images/notopened.ico"));
    }
}

void Widget::on_btnClearSend_2_clicked()
{

}

void Widget::on_btnClearRecv_2_clicked()
{

}





