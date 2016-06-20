#ifndef UDPSOCK_H
#define UDPSOCK_H

#include <QObject>
#include <QRunnable>
#include "pcap.h"

class UDPSock : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit UDPSock(QObject *parent = 0);
    ~UDPSock();
    static void call(uchar *self, const pcap_pkthdr *header, const uchar *packet);

signals:
    void sendSignal();

public slots:
    void updateInterface();

    void packReady(const uchar *packet, int len);
public:
    void run();

private:
    QString fname;
    int position;
    int cnt;
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_program filter;

    void listen();
    bool close();
    bool setFilter( const QString &filterexp );
    bool isValid();
    bool openConnection( const QString &dev, int snaplen, bool promisc );
    bool enough;
};

#endif // UDPSOCK_H
