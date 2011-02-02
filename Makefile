all:
	$(MAKE) -C src/

documentation:
	doxygen doc/doxygen/config

clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	$(MAKE) clean -C src/
