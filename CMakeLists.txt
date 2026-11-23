cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Build the offline course and the components it uses.
set(COMPONENTS main)
project(FoloToy-AI-Passport)
