#!/bin/bash

# Define colors for better readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Clear the screen
clear

# Print header
echo -e "${BLUE}=========================================${NC}"
echo -e "${GREEN}           SHADER BUILDER MENU          ${NC}"
echo -e "${BLUE}=========================================${NC}"
echo ""

# Print options
echo -e "${YELLOW}Please select a builder:${NC}"
echo ""
echo -e "  ${GREEN}1)${NC} Legacy OpenGL Shader Builder"
echo -e "     ${BLUE}→ Uses compatibility profile with legacy GLSL syntax${NC}"
echo -e "     ${BLUE}→ Located at: build-legacy/build.sh${NC}"
echo ""
echo -e "  ${GREEN}2)${NC} ShaderToy Builder"
echo -e "     ${BLUE}→ Uses modern OpenGL with ShaderToy-compatible code${NC}"
echo -e "     ${BLUE}→ Located at: build-shadertoy/build.sh${NC}"
echo ""
echo -e "  ${RED}q)${NC} Quit"
echo ""

# Function to check if a file exists and is executable
check_file() {
    if [ ! -f "$1" ]; then
        echo -e "${RED}Error: File '$1' does not exist!${NC}"
        return 1
    fi
    if [ ! -x "$1" ]; then
        echo -e "${YELLOW}Warning: File '$1' is not executable. Attempting to make it executable...${NC}"
        chmod +x "$1"
        if [ $? -ne 0 ]; then
            echo -e "${RED}Error: Could not make '$1' executable!${NC}"
            return 1
        fi
        echo -e "${GREEN}Successfully made '$1' executable.${NC}"
    fi
    return 0
}

# Function to run the selected builder
run_builder() {
    local builder_path="$1"
    local builder_name="$2"
    
    echo -e "${GREEN}Running ${builder_name}...${NC}"
    if check_file "$builder_path"; then
        # Save the current directory
        local current_dir=$(pwd)
        
        # Change to the builder directory and run the script
        cd "$(dirname "$builder_path")" && ./$(basename "$builder_path")
        
        # Return to the original directory
        cd "$current_dir"
    else
        echo -e "${RED}Could not run the ${builder_name}.${NC}"
    fi
}

# Check if an argument was provided
if [ $# -eq 1 ]; then
    choice=$1
else
    # Get user input
    echo -e "${YELLOW}Enter your choice (1, 2, or q):${NC} "
    read choice
fi

# Process the choice
case $choice in
    1)
        run_builder "build-legacy/build.sh" "Legacy OpenGL Shader Builder"
        ;;
    2)
        run_builder "build-shadertoy/build.sh" "ShaderToy Builder"
        ;;
    q|Q)
        echo -e "${YELLOW}Exiting...${NC}"
        exit 0
        ;;
    *)
        echo -e "${RED}Invalid option. Please try again.${NC}"
        ;;
esac

echo ""
echo -e "${BLUE}=========================================${NC}"
echo -e "${GREEN}           BUILDER COMPLETED           ${NC}"
echo -e "${BLUE}=========================================${NC}"