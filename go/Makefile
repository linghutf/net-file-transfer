goc=go
LDFLAGS=-ldflags="-s -w"

main: serv cli

serv: serv.go
	$(goc) build $(LDFLAGS) $^

cli: cli.go
	$(goc) build $(LDFLAGS) $^

test:
	@cp serv ../ && ../serv &
	./cli -f cli

clean:
	@rm -f serv cli
