TARGET   = pl11x_drv
REQUIRES = pl11x platform_pbxa9
SRC_CC   = main.cc video_memory.cc
LIBS     = cxx env server
INC_DIR += $(PRG_DIR)/..

vpath %.cc $(PRG_DIR)/..
