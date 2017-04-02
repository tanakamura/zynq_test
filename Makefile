all:
	$(MAKE) -C hw
	$(MAKE) -C sw

run:
	cd sw; xsdb run.tcl