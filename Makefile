# Exemple de Makefile pour le projet
# Organisation:
# créer un répertoire src/ contenant les .cpp
# créer un répertoite include/ contenant les .h
#
# Le Makefile détecte automatiquement les fichiers source ($(wildcard
# $(SRC_DIR)/*.cpp)) pour ne pas avoir à les spécifier manuellement
# Les chemins sont transformés automatiquement: $(patsubst) convertit les
# .cpp en .o dans le dossier build/ qui est créé automatiquement
# Les headers sont inclus via le flag -Iinclude


CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter -std=c++17 -Iinclude
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

TARGET = $(BUILD_DIR)/dominion

SRCS     := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@

.PHONY: run clean

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
