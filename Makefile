CXX ?= g++
CPPFLAGS ?= -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra

BUILD_DIR := build
DOMAIN_SOURCES := $(wildcard src/estrategias/*.cpp src/politicas/*.cpp src/services/*.cpp)
APP_SOURCES := main.cpp $(DOMAIN_SOURCES)
HEADERS := $(wildcard include/enums/*.hpp include/estrategias/*.hpp include/models/*.hpp include/politicas/*.hpp include/services/*.hpp)

.PHONY: all test clean

all: reservas

reservas: $(APP_SOURCES) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(APP_SOURCES) -o $@

test: $(BUILD_DIR)/testes_regras
	./$(BUILD_DIR)/testes_regras

$(BUILD_DIR)/testes_regras: tests/testes_regras.cpp $(DOMAIN_SOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/testes_regras.cpp $(DOMAIN_SOURCES) -o $@

clean:
	rm -rf $(BUILD_DIR) reservas
