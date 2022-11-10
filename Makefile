LDFLAGS=-lpthread
LDDEBUG=-g -O0 -v -da -Q

uscan: uscan.c dahua.c list.c json.c xml.c wsdiscovery.c ssdp.c
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: debug
debug: uscan_debug
uscan_debug: uscan.c dahua.c list.c json.c xml.c wsdiscovery.c ssdp.c
	$(CC) $(LDFLAGS) $(LDDEBUG) -o $@ $^

.PHONY: clean
clean:
	$(RM) uscan

.PHONY: clean_debug
clean_debug:
	$(RM) *r.alignments
	$(RM) *r.asmcons
	$(RM) *r.barriers
	$(RM) *r.dfinish
	$(RM) *r.dfinit
	$(RM) *r.dwarf2
	$(RM) *r.expand
	$(RM) *r.final
	$(RM) *r.into_cfglayout
	$(RM) *r.ira
	$(RM) *r.jump
	$(RM) *r.jump2
	$(RM) *r.mach
	$(RM) *r.nothrow
	$(RM) *r.outof_cfglayout
	$(RM) *r.pro_and_epilogue
	$(RM) *r.reginfo
	$(RM) *r.reload
	$(RM) *r.shorten
	$(RM) *r.split1
	$(RM) *r.split5
	$(RM) *r.vregs
