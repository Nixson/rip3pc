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
    void Process(QByteArray &data);

public slots:
    void updateInterface();
    bool close();

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
    bool setFilter( const QString &filterexp );
    bool isValid();
    bool openConnection( const QString &dev, int snaplen, bool promisc );
    bool enough;

    QString dev;
    int rlsPort;
    QString rlsHost;
};

#endif // UDPSOCK_H
