all: bin test

extlibraries:
	@echo Compiling external libraries...
	$(MAKE) -C lib/yaml-cpp/
	$(MAKE) -C lib/gtest/

bin: extlibraries
	@echo Compiling binaries...
	$(MAKE) -C src/

test: extlibraries bin
	@echo Compiling tests...
	$(MAKE) -C test/unit/
	$(MAKE) -C test/reader/
	$(MAKE) -C debug/secure_socket/

debug: clean extlibraries
	@echo DEBUG mode enabled
	$(MAKE) -C src/ DEBUG=true
	$(MAKE) test

documentation:
	doxygen doc/doxygen/config

clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	$(MAKE) clean -C src/
	$(MAKE) clean -C lib/yaml-cpp/
	$(MAKE) clean -C lib/gtest/
	$(MAKE) clean -C test/unit/
	$(MAKE) clean -C test/reader/
	$(MAKE) clean -C debug/secure_socket/
