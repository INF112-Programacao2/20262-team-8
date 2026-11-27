CXX ?= g++
CPPFLAGS ?= -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra

BUILD_DIR := build
DOMAIN_SOURCES := $(wildcard src/estrategias/*.cpp src/politicas/*.cpp src/services/*.cpp)
GUI_SOURCES := src/gui/main.cpp src/gui/app.cpp
CLI_SOURCES := main.cpp
HEADERS := $(wildcard include/enums/*.hpp include/estrategias/*.hpp include/models/*.hpp include/politicas/*.hpp include/services/*.hpp include/gui/*.hpp)
SFML_LIBS ?= -lsfml-graphics -lsfml-window -lsfml-system

.PHONY: all cli test clean

all: reservas

reservas: $(GUI_SOURCES) $(DOMAIN_SOURCES) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GUI_SOURCES) $(DOMAIN_SOURCES) -o $@ $(SFML_LIBS)

cli: reservas_cli

reservas_cli: $(CLI_SOURCES) $(DOMAIN_SOURCES) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CLI_SOURCES) $(DOMAIN_SOURCES) -o $@

test: $(BUILD_DIR)/testes_regras
	./$(BUILD_DIR)/testes_regras

$(BUILD_DIR)/testes_regras: tests/testes_regras.cpp $(DOMAIN_SOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/testes_regras.cpp $(DOMAIN_SOURCES) -o $@

clean:
	rm -rf $(BUILD_DIR) reservas reservas_cli
