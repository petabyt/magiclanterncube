# Force format
__A := $(shell astyle --style=allman cube.c)

TOP_DIR=../..
MODULE_NAME=cube
MODULE_OBJS=cube.o
include $(TOP_DIR)/modules/Makefile.modules
