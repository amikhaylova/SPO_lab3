cmake_minimum_required(VERSION 3.20)
project(SPO_lab3 C)

set(CMAKE_C_STANDARD 99)

add_executable(SPO_lab3 main.c terminal/terminal.c terminal/terminal.h server/server.c server/server.h client/client.c client/client.h gui/gui.c gui/gui.h model/model.c model/model.h util/util.c util/util.h)
