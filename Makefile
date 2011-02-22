all: extlibraries
	$(MAKE) -C src/

extlibraries:
	@echo Compiling external libraries...
	$(MAKE) -C lib/

documentation:
	doxygen doc/doxygen/config

clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	$(MAKE) clean -C src/
	$(MAKE) clean -C lib/
