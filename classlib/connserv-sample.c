/*
 *	SKK server support
 *
 *	by A.ITO	3 Jul, 1992
 */
#include "config.h"
#include "fep.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SKK_PORT_NUMBER	1178
#define SKK_SERVICENAME	"skkserv"

#ifdef BCOPY
#define bincopy(a,b,n)	bcopy(a,b,n)
#define fillzero(a,n)	bzero(a,n)
#else
#define bincopy(a,b,n)	memcpy(b,a,n)
#define fillzero(a,n)	memset(a,0,n)
#endif

int skkservsock = -1;
FILE *rserv,*wserv;
char *SKKServerHost = NULL;

char *getenv();

openSKKserv()
{
	int sock;
	int i;
	unsigned short port;
	struct sockaddr_in hostaddr;
	struct hostent *entry;
	struct servent *serv;
	struct protoent *proto;
	int a1,a2,a3,a4;
	unsigned long adr;
	char *hostname;
	
	if ((serv = getservbyname(SKK_SERVICENAME,"tcp")) == NULL) {
		serv = (struct servent*)malloc(sizeof(struct servent));
		serv->s_port = SKK_PORT_NUMBER;
	}
	fillzero((char*)&hostaddr,sizeof(struct sockaddr_in));
	if ((proto = getprotobyname("tcp")) == NULL) {
		return -1;
	}

	if ((sock = socket(AF_INET,SOCK_STREAM,proto->p_proto)) < 0) {
		return -1;
	}

	if (SKKServerHost)
		hostname = SKKServerHost;
	else if ((hostname = getenv("SKKSERVER")) == NULL) {
#ifdef SKK_SERVER_HOST
		hostname = SKK_SERVER_HOST;
#else
		return -1;
#endif
	}
	if ('0' <= *hostname && *hostname <= '9') {
		if (sscanf(hostname,"%d.%d.%d.%d",&a1,&a2,&a3,&a4) != 4) {
			return -1;
		}
		adr = htonl((a1<<12)|(a2<<8)|(a3<<4)|a4);
		bincopy(&adr, &hostaddr.sin_addr, 4);
	}
	else {
		if ((entry = gethostbyname(hostname)) == NULL) {
			return -1;
		}
		bincopy(entry->h_addr, &hostaddr.sin_addr, entry->h_length);
	}
        hostaddr.sin_family = AF_INET;
	hostaddr.sin_port = serv->s_port;
	if (connect(sock,&hostaddr,sizeof(struct sockaddr_in)) < 0) {
		return -1;
	}
	printf("SKKSERVER=%s\n\r",hostname);
	skkservsock = sock;
	rserv = fdopen(sock,"r");
	wserv = fdopen(sock,"w");
	return 0;
}

CandList
getCandFromServer(s)
char *s;
{
	char r;
	CandList res;

	fprintf(wserv,"1%s \n",s);
	fflush(wserv);
	read(skkservsock,&r,1);
	if (r == '1') {  /* succeeded */
		res = getCandList(rserv,NULL);
		return res;
	}
	else
		while (read(skkservsock,&r,1) > 0 && r != '\n');
	return NULL;
}

closeSKKserv()
{
	if (skkservsock >= 0) {
		fprintf(wserv,"0\n");
		fflush(wserv);
	}
}
