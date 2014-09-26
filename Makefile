default: all

.DEFAULT:
	cd src/sp-test && $(MAKE) $@
	cd src/perf-test && $(MAKE) $@

