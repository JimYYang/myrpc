CMAKE_BUILD_TYPE := Release
CMAKE_EXPORT_COMPILE_COMMANDS := 1
GENERATOR := Ninja  # 这里选择 Ninja 作为生成器


.PHONY: build
build: configure
	cmake --build build --parallel 8

.PHONY: configure
configure:
	cmake -B build \
	-G $(GENERATOR) \
	-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=${CMAKE_EXPORT_COMPILE_COMMANDS}

.PHONY: run
run:
	./build/rpc/tests/test_config_load

.PHONY: test
test:
	ctest --test-dir build -R "^rpc."

.PHONY: clean
clean:
	rm -rf build

#
# 可以使用 Make 来更方便地调用 CMake 命令：
#
#     make build WOLFRAM_APPID=xxx
#     make test
#     make run
#     make clean
#