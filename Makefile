all: bin test

libraries:
	@echo Compiling external libraries...
	$(MAKE) -C lib/yaml-cpp/
	$(MAKE) -C lib/gtest/

bin: libraries
	@echo Compiling binaries...
	$(MAKE) -C src/

test: libraries bin
	@echo Compiling tests...
	$(MAKE) -C test/unit/
	$(MAKE) -C test/reader/
	$(MAKE) -C debug/secure_socket/

debug: clean_bin libraries
	@echo DEBUG mode enabled
	$(MAKE) -C src/ DEBUG=true
	$(MAKE) test

documentation:
	doxygen doc/doxygen/config
	$(MAKE) -C doc/


clean_bin:
	$(MAKE) clean -C src/
	$(MAKE) clean -C test/unit/
	$(MAKE) clean -C test/reader/
	$(MAKE) clean -C debug/secure_socket/

clean:
	rm -rf doc/doxygen/html doc/doxygen/latex
	$(MAKE) clean -C doc/
	$(MAKE) clean_bin
	$(MAKE) clean -C lib/yaml-cpp/
	$(MAKE) clean -C lib/gtest/
