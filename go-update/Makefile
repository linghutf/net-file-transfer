cgo=go
LDFLAGS=-ldflags="-s -w"

main: serv cli

serv: serv.go
	$(cgo) build $(LDFLAGS) $<

cli: cli.go
	$(cgo) build $(LDFLAGS) $<

clean:
	@rm -f serv cli
