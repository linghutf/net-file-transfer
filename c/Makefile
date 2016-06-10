CC=gcc
CFLAGS=-O2
LDFLAGS=-levent -lcrypto -lpthread

main: echos fileserv filecli filetest

echos: echo_serv.c
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

fileserv: file_serv.c interface.c fileinfo.c simplog.c config.c
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

filecli: file_client.c interface.c fileinfo.c simplog.c config.c
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

filetest: filetest.c fileinfo.c
	$(CC) $^ -o $@ -lcrypto

test: filetest
	./fileinfo

clean:
	@rm -f echos fileserv filecli filetest
