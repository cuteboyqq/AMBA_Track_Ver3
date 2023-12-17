################################################################################
##  eazyai/unit_test/make.inc
##
## History:
##    2022/05/10 - [Yang Cao] Created file
##
## Copyright (c) 2022 Ambarella International LP
##
## This file and its contents ( "Software" ) are protected by intellectual
## property rights including, without limitation, U.S. and/or foreign
## copyrights. This Software is also the confidential and proprietary
## information of Ambarella International LP and its licensors. You may not use, reproduce,
## disclose, distribute, modify, or otherwise prepare derivative works of this
## Software or any portion thereof except pursuant to a signed license agreement
## or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
## In the absence of such an agreement, you agree to promptly notify and return
## this Software to Ambarella International LP.
##
## This file includes sample code and is only for internal testing and evaluation.  If you
## distribute this sample code (whether in source, object, or binary code form), it will be
## without any warranty or indemnity protection from Ambarella International LP or its affiliates.
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
## MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##
################################################################################

LOCAL_PATH	:= $(call my-dir)

### For quick_ipc
ifeq ($(BUILD_AMBARELLA_EAZYAI_QUICK_IPC_SCRIPTS), y)

include $(CLEAR_VARS)

.PHONY: quick_ipc

QUICK_IPC_SRCS := $(LOCAL_PATH)/quick_ipc/quick_ipc.sh \
		$(LOCAL_PATH)/quick_ipc/create_lua.sh \
		$(LOCAL_PATH)/quick_ipc/common_func_for_quick_ipc.sh
QUICK_IPC_SRCS_NAME := $(notdir $(QUICK_IPC_SRCS))
QUICK_IPC_SRCS_INSTALL := $(patsubst %,$(UNIT_TEST_PATH)/%, $(QUICK_IPC_SRCS_NAME))

quick_ipc:
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $(QUICK_IPC_SRCS) $(UNIT_TEST_PATH)/
	@chmod +x $(QUICK_IPC_SRCS_INSTALL)

$(call add-target-into-build, quick_ipc)

endif
### End for quick_ipc


### For CNNGUI
ifeq ($(BUILD_AMBARELLA_EAZYAI_CNNGUI_CONFIG), y)

include $(CLEAR_VARS)

.PHONY: cnngui

CNNGUI_CONFIG_SRCS := $(LOCAL_PATH)/cnngui/cnngui.json
CNNGUI_CONFIG_SRCS_NAME := $(notdir $(CNNGUI_CONFIG_SRCS))
CNNGUI_CONFIG_SRCS_INSTALL := $(patsubst %,$(UNIT_TEST_PATH)/%, $(CNNGUI_CONFIG_SRCS_NAME))

cnngui:
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $(CNNGUI_CONFIG_SRCS) $(UNIT_TEST_PATH)/
	@chmod +x $(CNNGUI_CONFIG_SRCS_INSTALL)

$(call add-target-into-build, cnngui)

endif
### End for CNNGUI


### For EAZYAI_UNIT_TEST
ifeq ($(BUILD_AMBARELLA_EAZYAI_UNIT_TEST), y)

################
include $(CLEAR_VARS)
LUA_DIR		:= $(LOCAL_PATH)/resource/lua

OPENCV_PATH	:= $(PREBUILD_OSS_DIR)/opencv
LOCAL_CFLAGS += -I$(OPENCV_PATH)/include/opencv4

LOCAL_TARGET	:= test_yolov8
LOCAL_SRCS	:= $(LOCAL_PATH)/test_yolov8.cpp \
		$(LOCAL_PATH)/nn_cvflow_task/nn_cvflow.c \
		$(LOCAL_PATH)/nn_thread/post_thread.c

LOCAL_SRCS	+= $(LOCAL_PATH)/nn_input/nn_input.c \
		$(LOCAL_PATH)/nn_input/iav_input.c \
		$(LOCAL_PATH)/nn_input/file_input.c \
		$(LOCAL_PATH)/nn_input/dummy_input.c \
		$(LOCAL_PATH)/yolov8_utils/bounding_box.c \
		$(LOCAL_PATH)/yolov8_utils/config_reader.c \
		$(LOCAL_PATH)/yolov8_utils/dla_config.c \
		$(LOCAL_PATH)/yolov8_utils/gms_matcher.c \
		$(LOCAL_PATH)/yolov8_utils/img_util.c \
		$(LOCAL_PATH)/yolov8_utils/logger.c \
		$(LOCAL_PATH)/yolov8_utils/matching2D.c \
		$(LOCAL_PATH)/yolov8_utils/object.c \
		$(LOCAL_PATH)/yolov8_utils/object_tracker.c \
		$(LOCAL_PATH)/yolov8_utils/point.c \
		$(LOCAL_PATH)/yolov8_utils/trajectory.c \
		$(LOCAL_PATH)/yolov8_utils/utils.c \
		$(LOCAL_PATH)/yolov8_utils/vision_tracker.c \
		$(LOCAL_PATH)/yolov8_utils/yolov8_class.c
		

LOCAL_CFLAGS 	+= -I$(AMB_TOPDIR)/packages/eazyai/inc \
		-I$(LOCAL_PATH)/nn_cvflow_task \
		-I$(LOCAL_PATH)/nn_arm_task \
		-I$(LOCAL_PATH)/nn_thread

LOCAL_CFLAGS	+= -I$(LOCAL_PATH)/nn_input

LOCAL_CFLAGS    += -DEIGEN_MPL2_ONLY  # For Eigen library to use MPL2 license related part only

LOCAL_LDFLAGS   := -L$(PREBUILD_OSS_DIR)/lua/usr/lib -llua

LOCAL_LIBS		:= libeazyai.so libeazyai_utils.so libeazyai_comm.so libeazyai_arm_postprocess.so

OPENCV_PATH		:= $(PREBUILD_OSS_DIR)/opencv
LOCAL_LDFLAGS	+= -lpthread -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_features2d \
	-L$(OPENCV_PATH)/usr/lib \
	-Wl,-rpath-link=$(FAKEROOT_DIR)/usr/lib \
	-Wl,-rpath-link=$(OPENCV_PATH)/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/libjpeg-turbo/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/tbb/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/freetype/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/libpng/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/zlib/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/bzip2/usr/lib \
	-lstdc++

ifeq ($(BUILD_AMBARELLA_CONFIG_NN_ARM_TASK_QRCODE), y)
LOCAL_LDFLAGS	+= -lopencv_objdetect
endif

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $< $(UNIT_TEST_PATH)/
	@mkdir -p $(FAKEROOT_DIR)/usr/share/ambarella/eazyai/lua/
	@cp -dpRf $(LUA_DIR)/* $(FAKEROOT_DIR)/usr/share/ambarella/eazyai/lua/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

##########
include $(CLEAR_VARS)

LOCAL_TARGET	:= test_eazyai_display
LOCAL_SRCS		:= $(LOCAL_PATH)/test_eazyai_display.c

LOCAL_CFLAGS	:= -I$(AMB_TOPDIR)/packages/eazyai/inc

LOCAL_LIBS		:= libeazyai.so libeazyai_utils.so

OPENCV_PATH		:= $(PREBUILD_OSS_DIR)/opencv
LOCAL_LDFLAGS	+= 	-Wl,-rpath-link=$(FAKEROOT_DIR)/usr/lib \
	-Wl,-rpath-link=$(OPENCV_PATH)/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/libjpeg-turbo/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/tbb/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/freetype/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/libpng/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/zlib/usr/lib \
	-Wl,-rpath-link=$(PREBUILD_OSS_DIR)/bzip2/usr/lib \
	-lstdc++

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))
##########

endif
### End for EAZYAI_UNIT_TEST

include $(call all-makefiles-under, $(LOCAL_PATH))
