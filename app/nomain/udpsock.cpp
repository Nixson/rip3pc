#include "udpsock.h"
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include "memory.h"

#include <iostream>
using namespace std;

static const int PCAP_TIMEOUT = 10000;


UDPSock::UDPSock(QObject *parent) : QObject(parent){}
UDPSock::~UDPSock(){}
void UDPSock::run(){
    fname = "";
    position = 0;
    updateInterface();
}
bool UDPSock::openConnection( const QString &dev, int snaplen, bool promisc )
{
    handle = pcap_open_live( dev.toLocal8Bit().constData(),
                                snaplen,
                                (int)promisc,
                                1000,
                                errbuf );
    return isValid();
}
bool UDPSock::isValid(){
    //cout << "isValid: " << handle << endl;
    return (0 != handle);
}

bool UDPSock::setFilter( const QString &filterexp )
{
    int status = pcap_compile(handle, &filter, filterexp.toLocal8Bit().constData(), 0, 0);
    if (status != 0)
        return false;
    cout << "pcap_compile: " << filterexp.toStdString() << ": " << status << endl;

    status = pcap_setfilter(handle, &filter);
    cout << "pcap_setfilter: " << status << endl;
    if (status != 0)
        return false;
    return true;
}
bool UDPSock::close()
{
    if (!isValid())
        return false;
    pcap_close(handle);
    handle = 0;
    return true;
}

void UDPSock::listen(){
    pcap_pkthdr *pktheader;
    const u_char *pktdata;
    int res = 0;
    if(isValid()){
            while(isValid()){
                    res = pcap_next_ex(handle, &pktheader, &pktdata);
                    if(res == 1){
                        cout << pktheader->len << endl;
                        packReady(pktdata,pktheader->len);
                    }
            }
    }
    else {
        cout << "stop" << endl;
    }
}
void UDPSock::call(uchar *self, const pcap_pkthdr *header, const uchar *packet){
    Q_UNUSED(self);
    Q_UNUSED(packet);
    //UDPSock *usock = (UDPSock *)self;
    cout << header->len << endl;
    //usock->packReady(packet);
}


void UDPSock::updateInterface(){
    if(isValid()){
        close();
        cout << "close" << endl;
        QTimer::singleShot(1500,[=](){
            this->updateInterface();
        });
        return;
    }
    QString dev = Memory::get("device","").toString();
    int rlsPort = Memory::get("rlsPort",30583).toInt();
    QString rlsHost = Memory::get("rlsIP","127.0.0.1").toString();
    QString filter ="ip src host "+rlsHost+" and udp dst port "+QString::number(rlsPort);
    if(dev.size() > 0){
        cout << filter.toStdString() << endl;
        if(openConnection( dev, 65535, true )){
            setFilter( filter );
//            setFilter( QString("udp") );
            listen();
        }
        cout << isValid() << endl;
    }
}
void UDPSock::packReady(const uchar *packet, int len){
    QByteArray ba((const char *)packet,len);
    cout << ba.toStdString() << endl;
}

