/*******************************************************************************
 * test_eazyai.c
 *
 * History:
 *  2022/06/28  - [Yang Cao] created
 *
 *
 * Copyright (c) 2022 Ambarella International LP
 *
 * This file and its contents ( "Software" ) are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <signal.h>
#include <getopt.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <eazyai.h>
#include <nn_arm.h>

#include "nn_cvflow.h"
#include "nn_input.h"
#include "post_thread.h"

#define INTERVAL_PRINT_PROCESS_TIME 30
#define TO_FILE_POSTPROCESS_NAME "to_file"
#define DEFAULT_FPS_COUNT_PERIOD 100

#define CANVAS
#ifdef CANVAS
#define DEFAULT_CANVAS_ID 1
#else
#define DEFAULT_CANVAS_ID -1
#endif

#define VOUT
#ifdef VOUT
#define DEFAULT_VOUT_ID EA_DISPLAY_ANALOG_VOUT
#else
#define DEFAULT_VOUT_ID -1
#endif

#ifdef STREAM
#define DEFAULT_STREAM_ID 0
#else
#define DEFAULT_STREAM_ID -1
#endif

typedef struct live_params_s {
	//Input parameters, include input from iav or jpg or raw file, run mode.
	int canvas_id;
	int pyramid[2];
	int mode;
	int multi_in_num;
	char multi_in_params[NN_MAX_PORT_NUM][MAX_STR_LEN + 1];
	int yuv_flag;
	int use_pyramid;
	int feature;

	//Preprocess parameters, include color conversion, roi.
	int rgb;
	ea_roi_t roi;

	//Inference parameters, include model path, label path, inference device, etc.
	const char *model_path;
	const char *ades_cmd_file;
	int acinf_gpu_id;

	//Model postprocessing parameters, include name, lua file path, etc.
	const char *arm_nn_name;
	int queue_size;
	const char *lua_file_path;
	int thread_num;
	char extra_input[MAX_STR_LEN + 1];
	const char *label_path;
	int class_num;
	int enable_hold_img_flag;

	//Showing results parameters, include showing results on stream or jpg file, saving result to file.
	int stream_id;
	int enable_fsync_flag;
	int overlay_buffer_offset;
	char *result_f_path;
	int vout_id;
	int draw_mode;
	char output_dir[MAX_STR_LEN + 1];

	//Miscellaneous
	int support;
	int log_level;

} live_params_t;

typedef struct live_ctx_s {
	nn_cvflow_t nn_cvflow;
	post_thread_ctx_t thread_ctx;
	ea_display_t *display;
	int sig_flag;
	unsigned int seq;
	nn_input_context_type_t nn_input_ctx;
	int loop_count;
	int f_result;
} live_ctx_t;

EA_LOG_DECLARE_LOCAL(EA_LOG_LEVEL_NOTICE);
EA_MEASURE_TIME_DECLARE();
static live_ctx_t live_ctx;

#define	NO_ARG	0
#define	HAS_ARG	1

typedef enum live_numeric_short_options_e {
	OPTION_MODEL_PATH,
	OPTION_LABEL_PATH,
	OPTION_OUTPUT_DIR,
	OPTION_LUA_FILE_PATH,
	OPTION_QUEUE_SIZE,
	OPTION_THREAD_NUM,
	OPTION_CLASS_NUM,
	OPTION_ADES_CMD_FILE,
	OPTION_ACINF_GPU_ID,
	OPTION_OVERLAY_BUF_OFFSET,
	OPTION_YUV,
	OPTION_MULTI_IN,
	OPTION_IN_ROI,
	OPTION_EXTRA_INPUT,
	OPTION_SUPPORT_LIST,
	OPTION_VOUT_ID,
	OPTION_FSYNC_OFF,
	OPTION_RESULT_TO_TXT,
	OPTION_HOLD_IMG,
} live_numeric_short_options_t;

#define INPUT_OPTIONS \
	{"canvas_id", HAS_ARG, 0, 'c'}, \
	{"pyramid_id", HAS_ARG, 0, 'p'}, \
	{"mode", HAS_ARG, 0, 'm'}, \
	{"isrc", HAS_ARG, 0, OPTION_MULTI_IN}, \
	{"yuv", NO_ARG, 0, OPTION_YUV}

#define PREPROCESS_OPTIONS \
	{"rgb", NO_ARG, 0, 'r'}, \
	{"roi", HAS_ARG, 0, OPTION_IN_ROI}

#define INFERENCE_OPTIONS \
	{"model_path", HAS_ARG, 0, OPTION_MODEL_PATH}, \
	{"ades_cmd_file", HAS_ARG, 0, OPTION_ADES_CMD_FILE}, \
	{"acinf_gpu_id", HAS_ARG, 0, OPTION_ACINF_GPU_ID}

#define POSTPROCESS_OPTIONS \
	{"nn_arm_name", HAS_ARG, 0, 'n'}, \
	{"queue_size", HAS_ARG, 0, OPTION_QUEUE_SIZE}, \
	{"lua_file", HAS_ARG, 0, OPTION_LUA_FILE_PATH}, \
	{"thread_num", HAS_ARG, 0, OPTION_THREAD_NUM}, \
	{"extra_input", HAS_ARG, 0, OPTION_EXTRA_INPUT}, \
	{"label_path", HAS_ARG, 0, OPTION_LABEL_PATH}, \
	{"class_num", HAS_ARG, 0, OPTION_CLASS_NUM}, \
	{"hold_img", NO_ARG, 0, OPTION_HOLD_IMG}

#define SHOW_RESULTS_OPTIONS \
	{"stream_id", HAS_ARG, 0, 's'}, \
	{"fsync_off", NO_ARG, 0, OPTION_FSYNC_OFF}, \
	{"stream_offset", HAS_ARG, 0, OPTION_OVERLAY_BUF_OFFSET}, \
	{"to_txt", HAS_ARG, 0, OPTION_RESULT_TO_TXT}, \
	{"vout_id", HAS_ARG, 0, OPTION_VOUT_ID}, \
	{"draw_mode", HAS_ARG, 0, 'd'}, \
	{"output_dir", HAS_ARG, 0, OPTION_OUTPUT_DIR}

#define MISCELLANEOUS_OPTIONS \
	{"support", NO_ARG, 0, OPTION_SUPPORT_LIST}, \
	{"log_level", HAS_ARG, 0, 'v'}, \
	{0, 0, 0, 0}

static struct option long_options[] = {
	INPUT_OPTIONS,
	PREPROCESS_OPTIONS,
	INFERENCE_OPTIONS,
	POSTPROCESS_OPTIONS,
	SHOW_RESULTS_OPTIONS,
	MISCELLANEOUS_OPTIONS,
};

static const char *short_options = "m:v:d:n:rs:c:p:";

struct hint_s {
	const char *arg;
	const char *str;
};

static const struct hint_s hint[] = {
	{"", "\t\tcanvas id. Default is 1"},
	{"", "\t\tpyramid id."},
	{"", "\t\trun mode"},
	{"", "\t\tmulti input, e.g. -isrc \"i:data=image|t:jpeg|c:rgb|r:0,0,0,0|d:cpu\". Only for file mode."},
	{"", "\t\tenable yuv input from iav, default is disable."},
	{"", "\t\tset color type to rgb_planar, default is bgr_planar. Only for live mode."},
	{"", "\t\troi of image, default is full image, order of roi parameters: x,y,h,w. Only for live mode."},
	{"", "\t\tpath of cavalry bin file."},
	{"", "\t\ades command file path. Run Ades if specified, otherwise run ACINF."},
	{"", "\tacinf gpu id, default is -1(CPU). Only for Acinference."},
	{"", "\tnn arm task name."},
	{"", "\t\tqueue size, default is 1."},
	{"", "\t\tlua file name."},
	{"", "\t\tthread number, default is 1."},
	{"", "\textra input for network."},
	{"", "\t\tpath of class names file."},
	{"", "\t\tclass number, for detection case should be set."},
	{"", "\t\tenable or disable to hold image data for postprocess, default is disable. Only for live mode."},
	{"", "\t\tstream ID to draw. Default is -1, means app doesn't use stream to draw."},
	{"", "\t\tenable or disable frame sync, default is enable."},
	{"", "\toverlay buffer offset for multi-stream display."},
	{"", "\t\tsave detection results to txt file. Only for file mode."},
	{"", "\t\tvout id for display, default is 1, means to use HDMI/CVBS."},
	{"", "\t\tdraw mode, 0=draw bbox, 1=draw img."},
	{"", "\t\tpath to contain output file."},
	{"", "\t\tshow support list."},
	{"", "\t\tlog level 0=None, 1=Error, 2=Notice, 3=Debug, 4=Verbose."},
};

static void sort(nn_task_name_t* array, int size)
{
	int j, i;
	char tmp[MAX_STR_LEN - 1];
	for (j = 0; j < (size - 1); j++) {
		for(i = 0; i < (size - j - 1); i++) {
			if(array[i].label[0] > array[i+1].label[0]) {
				strncpy(tmp, array[i].label, MAX_STR_LEN - 1);
				tmp[MAX_STR_LEN - 2] = '\0';
				strncpy(array[i].label, array[i+1].label, MAX_STR_LEN);
				array[i].label[MAX_STR_LEN - 1] = '\0';
				strncpy(array[i+1].label, tmp, MAX_STR_LEN - 1);
				array[i+1].label[MAX_STR_LEN - 1] = '\0';
			}
		}
	}
}

static void support_list()
{
	int nn_arm_task_num;
	nn_task_name_t *nn_task = NULL;
	int j;
	nn_arm_task_num = nn_arm_task_get_support_list(&nn_task);
	if (nn_arm_task_num > 1) {
		for (j = 0; j < nn_arm_task_num; j++) {
			if (strcmp(nn_task[j].label, TO_FILE_POSTPROCESS_NAME) == 0) {
				break;
			}
		}
		if (j != 0) {
			strncpy(nn_task[j].label, nn_task[0].label, MAX_STR_LEN);
			nn_task[j].label[MAX_STR_LEN - 1] = '\0';
			strncpy(nn_task[0].label, TO_FILE_POSTPROCESS_NAME, MAX_STR_LEN);
			nn_task[0].label[MAX_STR_LEN - 1] = '\0';
		}
	}
	if (nn_arm_task_num > 1) {
		sort(&nn_task[1], nn_arm_task_num - 1);
	}
	printf("support %d arm postprocess tasks:\n", nn_arm_task_num);
	for (j = 0; j < nn_arm_task_num; j++) {
		printf("\t%s\n", nn_task[j].label);
	}
	printf("\n");
	free(nn_task);
	nn_task = NULL;
}

static void usage(void)
{
	char mode[MAX_STR_LEN] = {0};
	nn_input_get_list(mode, MAX_STR_LEN);
	printf("test_eazyai usage:\n");
	for (size_t i = 0; i < sizeof(long_options) / sizeof(long_options[0]) - 1; i++) {
		if (isalpha(long_options[i].val)) {
			printf("-%c ", long_options[i].val);
		} else {
			printf("   ");
		}
		printf("--%s", long_options[i].name);
		if (hint[i].arg[0] != 0)
			printf(" [%s]", hint[i].arg);
		if (long_options[i].val == 'm') {
			printf("\t%s: %s.\n", hint[i].str, mode);
		} else {
			printf("\t%s\n", hint[i].str);
		}
	}
	support_list();
}

static int parse_param(int argc, char **argv, live_params_t *params)
{
	int rval = EA_SUCCESS;
	int ch;
	int option_index = 0;
	int value;

	do {
		opterr = 0;
		while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
		switch (ch) {
			case 'm':
				params->mode = atoi(optarg);
				if (params->mode < RUN_LIVE_MODE || params->mode >= MAX_RUN_MODE) {
					EA_LOG_ERROR("mode is wrong, %d\n", params->mode);
					rval = EA_FAIL;
					break;
				}
				break;
			case 'v':
				params->log_level = atoi(optarg);
				if (params->log_level < EA_LOG_LEVEL_NONE || params->log_level >= EA_LOG_LEVEL_INVALID) {
					EA_LOG_ERROR("log_level is wrong, %d\n", params->log_level);
					rval = EA_FAIL;
					break;
				}
				break;
			case OPTION_OUTPUT_DIR:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("output_dir is empty\n");
					rval = EA_FAIL;
					break;
				}
				if (value > MAX_STR_LEN) {
					EA_LOG_ERROR("output_dir should be no more than %d characters, %s\n", MAX_STR_LEN, optarg);
					rval = EA_FAIL;
					break;
				}
				strncpy(params->output_dir, optarg, MAX_STR_LEN);
				if (optarg[value - 1] != '/') {
					strncat(params->output_dir, "/", MAX_STR_LEN - strlen(params->output_dir));
				}
				params->feature |= (OUT_TYPE_JPEG & (~OUT_TYPE_VOUT));
				break;
			case OPTION_EXTRA_INPUT:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("path for extra input is empty\n");
					rval = EA_FAIL;
					break;
				}
				if (value > MAX_STR_LEN) {
					EA_LOG_ERROR("path for extra input should be no more than %d characters, %s\n", MAX_STR_LEN, optarg);
					rval = EA_FAIL;
					break;
				}
				strncpy(params->extra_input, optarg, MAX_STR_LEN);
				break;
			case OPTION_SUPPORT_LIST:
				params->support = 1;
				break;
			case 'd':
				value = atoi(optarg);
				if (value < DRAW_BBOX_TEXTBOX || value > DRAW_256_COLORS_IMAGE) {
					EA_LOG_ERROR("draw mode is wrong, %d\n", params->draw_mode);
					rval = EA_FAIL;
					break;
				}
				params->draw_mode = value;

				break;
			case 's':
				params->stream_id = atoi(optarg);
				if (params->stream_id < 0) {
					EA_LOG_ERROR("stream ID is wrong, %d\n", params->stream_id);
					rval = EA_FAIL;
					break;
				}
				params->feature |= OUT_TYPE_STREAM;
				break;
			case 'c':
				params->canvas_id = atoi(optarg);
				if (params->canvas_id < 0) {
					EA_LOG_ERROR("canvas ID is wrong, %d\n", params->canvas_id);
					rval = EA_FAIL;
					break;
				}
				params->feature |= IN_TYPE_CANVAS_BUFFER;
				break;
			case 'p':
				value = sscanf(optarg, "%d,%d",
					&params->pyramid[0], &params->pyramid[1]);
				if (value != 2) {
					printf("pyramid parameters are wrong, %s\n", optarg);
					rval = EA_FAIL;
					break;
				}
				params->use_pyramid = IN_SRC_ON;
				params->feature |= IN_TYPE_PYRAMID_BUFFER;
				break;
			case OPTION_MULTI_IN:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("multi in is empyt\n");
					rval = EA_FAIL;
					break;
				}
				strncpy(params->multi_in_params[params->multi_in_num],
					optarg, MAX_STR_LEN);
				params->multi_in_num++;
				if (params->multi_in_num > NN_MAX_PORT_NUM) {
					EA_LOG_ERROR("network input number more than %d\n", NN_MAX_PORT_NUM);
					rval = EA_FAIL;
					break;
				}
				break;
			case OPTION_MODEL_PATH:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("model_path is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->model_path = optarg;
				break;
			case OPTION_LABEL_PATH:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("label_path is empty\n");
					rval = EA_FAIL;
					break;
				}

				params->label_path = optarg;
				break;
			case 'n':
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("model name is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->arm_nn_name = optarg;
				break;
			case 'r':
				params->rgb = RGB_PLANAR;
				break;
			case OPTION_LUA_FILE_PATH:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("lua file is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->lua_file_path = optarg;
				break;
			case OPTION_QUEUE_SIZE:
				value = atoi(optarg);
				if (value <= 0) {
					EA_LOG_ERROR("queue is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->queue_size = value;
				break;
			case OPTION_CLASS_NUM:
				value = atoi(optarg);
				if (value < 0) {
					EA_LOG_ERROR("class num should > 0\n");
					rval = EA_FAIL;
					break;
				}
				params->class_num = value;
				break;
			case OPTION_THREAD_NUM:
				value = atoi(optarg);
				if (value <= 0) {
					EA_LOG_ERROR("thread num should > 0, default is 1\n");
					rval = EA_FAIL;
					break;
				}
				params->thread_num = value;
				break;
			case OPTION_ADES_CMD_FILE:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("ades command file is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->ades_cmd_file = optarg;
				break;
			case OPTION_ACINF_GPU_ID:
				value = atoi(optarg);
				if (value < -1)  {
					EA_LOG_ERROR("acinf_gpu id should be >= -1");
					rval = EA_FAIL;
					break;
				}
				params->acinf_gpu_id = value;
				break;
			case OPTION_OVERLAY_BUF_OFFSET:
				params->overlay_buffer_offset = atoi(optarg);
				break;
			case OPTION_YUV:
				params->yuv_flag = IN_SRC_ON;
				params->multi_in_num = 2;
				break;
			case OPTION_FSYNC_OFF:
				params->enable_fsync_flag = IN_SRC_OFF;
				break;
			case OPTION_HOLD_IMG:
				params->enable_hold_img_flag = IN_SRC_ON;
				break;
			case OPTION_IN_ROI:
				value = sscanf(optarg, "%d,%d,%d,%d",
					&(params->roi.x), &(params->roi.y),
					&(params->roi.h), &(params->roi.w));
				if (value != 4) {
					printf("roi parameters are wrong, %s\n", optarg);
					rval = EA_FAIL;
					break;
				}
				break;
			case OPTION_VOUT_ID:
				value = atoi(optarg);
				if (value < EA_DISPLAY_DIGITAL_VOUT ||
					value >= EA_DISPLAY_VOUT_NUM)  {
					EA_LOG_ERROR("vout id parameter is wrong, %d\n", value);
					rval = EA_FAIL;
					break;
				}
				params->vout_id = value;
				params->feature |= OUT_TYPE_VOUT;
				break;
			case OPTION_RESULT_TO_TXT:
				value = strlen(optarg);
				if (value == 0) {
					EA_LOG_ERROR("The path of txt file to save result is empty\n");
					rval = EA_FAIL;
					break;
				}
				params->result_f_path = optarg;
				break;
			default:
				EA_LOG_ERROR("unknown option found: %c\n", ch);
				rval = EA_FAIL;
				break;
			}
		}
	} while (0);

	return rval;
}

static int check_params(live_params_t *params)
{
	int rval = EA_SUCCESS;
	do {
		if (((params->feature & OUT_TYPE_LIVE) == OUT_TYPE_LIVE ||
			((params->feature & OUT_TYPE_LIVE) == 0 &&
			params->stream_id >= 0 && params->vout_id >= 0)) &&
			params->mode == RUN_LIVE_MODE) {
			EA_LOG_ERROR("Stream and Vout are set simultaneously. Only support one of them in live mode.\n");
			rval = EA_FAIL;
			break;
		}
		if ((params->feature & IN_TYPE_LIVE) == IN_TYPE_LIVE &&
			params->mode == RUN_LIVE_MODE) {
			EA_LOG_ERROR("Canvas and Pyramid buffer are set simultaneously. Only support one of them in live mode.\n");
			rval = EA_FAIL;
			break;
		}
	} while (0);

	return rval;
}

static int init_param(int argc, char **argv, live_params_t *params)
{
	int rval = EA_SUCCESS;

	memset(params, 0, sizeof(live_params_t));

	params->mode = RUN_LIVE_MODE;
	params->log_level = EA_LOG_LEVEL_NOTICE;
	params->draw_mode = DRAW_BBOX_TEXTBOX;
	params->rgb = BGR_PLANAR;

	params->yuv_flag = IN_SRC_OFF;
	params->use_pyramid = IN_SRC_OFF;
	params->enable_fsync_flag = IN_SRC_ON;

	params->canvas_id = DEFAULT_CANVAS_ID;
	params->vout_id = DEFAULT_VOUT_ID;
	params->stream_id = DEFAULT_STREAM_ID;

	params->queue_size = 1;
	params->thread_num = 1;
	params->acinf_gpu_id = -1;
	params->overlay_buffer_offset = -1;

	do {
		if (argc < 2) {
			usage();
			exit(0);
		} else {
			RVAL_OK(parse_param(argc, argv, params));
		}
		if (!!params->support) {
			support_list();
			exit(0);
		}

		RVAL_OK(check_params(params));
		if (params->mode == RUN_LIVE_MODE) {
			if (params->stream_id < 0) {
				params->feature |= OUT_TYPE_VOUT;
				if (params->vout_id < 0) {
					params->vout_id = EA_DISPLAY_ANALOG_VOUT;
				}
			} else if (params->stream_id >= 0 && params->vout_id < 0) {
				params->feature |= OUT_TYPE_STREAM;
			}

			if (params->use_pyramid == IN_SRC_OFF) {
				params->feature |= IN_TYPE_CANVAS_BUFFER;
				if (params->canvas_id < 0) {
					params->canvas_id = 1;
				}
			}
		}

		EA_LOG_SET_LOCAL(params->log_level);
		EA_LOG_NOTICE("live parameters:\n");
		EA_LOG_NOTICE("\tmode: %d\n", params->mode);
		EA_LOG_NOTICE("\toutput_dir path: %s\n", params->output_dir);
		EA_LOG_NOTICE("\tlog level: %d\n", params->log_level);
		EA_LOG_NOTICE("\tdraw mode: %d\n", params->draw_mode);
		EA_LOG_NOTICE("\tstream ID: %d\n", params->stream_id);
		EA_LOG_NOTICE("\tVOUT ID: %d\n", params->vout_id);
		EA_LOG_NOTICE("\tmodel path: %s\n", params->model_path);
		EA_LOG_NOTICE("\tlabel path: %s\n", params->label_path);
		EA_LOG_NOTICE("\tmodel name: %s\n", params->arm_nn_name);
		EA_LOG_NOTICE("\tqueue size: %d\n", params->queue_size);
		EA_LOG_NOTICE("\trgb type: %d\n", params->rgb);
		EA_LOG_NOTICE("\tcanvas id: %d\n", params->canvas_id);
		EA_LOG_NOTICE("\tfile name of saving result to txt: %s\n",
			params->result_f_path);

		if (params->draw_mode == DRAW_BBOX_TEXTBOX) {
			params->draw_mode = EA_DISPLAY_BBOX_TEXTBOX;
		} else if (params->draw_mode == DRAW_256_COLORS_IMAGE) {
			params->draw_mode = EA_DISPLAY_256_COLORS;
		}

	} while (0);

	return rval;
}
static void live_set_post_thread_params(live_params_t *params,
	post_thread_params_t *post_params)
{
	post_params->label_path = params->label_path;
	post_params->class_num = params->class_num;
	post_params->arm_nn_name = params->arm_nn_name;
	post_params->queue_size = params->queue_size;
	post_params->thread_num = params->thread_num;
	post_params->lua_file_path = params->lua_file_path;
	post_params->in_mode = params->mode;
	post_params->log_level = params->log_level;
	post_params->output_dir = params->output_dir;
	post_params->extra_input = params->extra_input;
	post_params->use_pyramid = params->use_pyramid;
	post_params->total_count = INTERVAL_PRINT_PROCESS_TIME;
	post_params->enable_fsync_flag = params->enable_fsync_flag;
	if (params->use_pyramid == IN_SRC_ON &&
		params->feature & LIVE_MODE_ALL) {
		post_params->img_src_id = params->pyramid[1];
	} else {
		post_params->img_src_id = 0;
	}
}

static int cv_env_init(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	nn_input_ops_type_t *ops = NULL;
	nn_input_context_type_t *ctx;
	int i;
	do {
		ctx = &live_ctx->nn_input_ctx;
		ctx->canvas_id = params->canvas_id;
		ctx->stream_id = params->stream_id;
		ctx->use_pyramid = params->use_pyramid;
		ctx->feature = params->feature;
		ctx->yuv_flag = params->yuv_flag;
		ctx->pyramid[0] = params->pyramid[0];
		ctx->pyramid[1] = params->pyramid[1];
		ctx->multi_in_num = params->multi_in_num;
		ctx->rgb = params->rgb;
		ctx->draw_mode = params->draw_mode;
		ctx->output_dir = params->output_dir;
		ctx->overlay_buffer_offset = params->overlay_buffer_offset;
		ctx->roi.x = params->roi.x;
		ctx->roi.y = params->roi.y;
		ctx->roi.h = params->roi.h;
		ctx->roi.w = params->roi.w;
		ctx->vout_id = params->vout_id;
		for (i = 0; i < params->multi_in_num; i++) {
			strncpy(ctx->multi_in_params[i],
				params->multi_in_params[i], MAX_STR_LEN);
		}
		ctx->ops = nn_intput_get_ops(params->mode);
		RVAL_ASSERT(ctx->ops != NULL);
		ops = ctx->ops;
		RVAL_ASSERT(ops->nn_input_init != NULL);
		RVAL_OK(ops->nn_input_init(ctx));
	} while (0);
	return rval;
}

static void notifier(void *arg)
{
	*((int *)arg) = 1;
}

static int live_init(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	nn_cvflow_params_t net_params;
	nn_input_ops_type_t *ops = NULL;
	do {
		RVAL_ASSERT(live_ctx != NULL);
		RVAL_ASSERT(params != NULL);
		live_ctx->loop_count = INTERVAL_PRINT_PROCESS_TIME;
		live_ctx->f_result = -1;
		if (params->result_f_path &&
			params->mode == RUN_FILE_MODE) {
			live_ctx->f_result = open(params->result_f_path, O_CREAT | O_RDWR | O_TRUNC, 0644);
			RVAL_ASSERT(live_ctx->f_result != -1);
		}
		RVAL_OK(cv_env_init(live_ctx, params));
		ops = live_ctx->nn_input_ctx.ops;
		RVAL_ASSERT(params->model_path != NULL);
		memset(&net_params, 0, sizeof(nn_cvflow_params_t));
		net_params.log_level = params->log_level;
		net_params.model_path = params->model_path;
		net_params.ades_cmd_file = params->ades_cmd_file;
		net_params.acinf_gpu_id = params->acinf_gpu_id;
		RVAL_OK(nn_cvflow_init(&live_ctx->nn_cvflow, &net_params));
		live_ctx->nn_input_ctx.net = (live_ctx->nn_cvflow.net);
		RVAL_ASSERT(ops->nn_input_check_params != NULL);
		RVAL_OK(ops->nn_input_check_params(&live_ctx->nn_input_ctx));
		live_ctx->display = live_ctx->nn_input_ctx.display;
		if (params->mode != RUN_DUMMY_MODE) {
			live_set_post_thread_params(params, &live_ctx->thread_ctx.params);
			live_ctx->thread_ctx.display = live_ctx->display;
			live_ctx->thread_ctx.input_ctx = &live_ctx->nn_input_ctx;
			live_ctx->thread_ctx.f_result = live_ctx->f_result;
			RVAL_OK(post_thread_init(&live_ctx->thread_ctx, &live_ctx->nn_cvflow));
			RVAL_OK(post_thread_set_notifier(&live_ctx->thread_ctx, notifier, &live_ctx->sig_flag));
		}
	} while (0);
	return rval;
}

static void cv_env_deinit(live_ctx_t *live_ctx)
{
	nn_input_ops_type_t *ops = NULL;
	ops = live_ctx->nn_input_ctx.ops;
	if (ops) {
		EA_LOG_ASSERT(ops->nn_input_deinit != NULL);
		ops->nn_input_deinit(&live_ctx->nn_input_ctx);
	}
}

static void live_deinit(live_ctx_t *live_ctx, live_params_t *params)
{
	post_thread_deinit(&live_ctx->thread_ctx, &live_ctx->nn_cvflow);
	nn_cvflow_deinit(&live_ctx->nn_cvflow);
	cv_env_deinit(live_ctx);
	if (live_ctx->f_result > -1) {
		close(live_ctx->f_result);
		live_ctx->f_result = -1;
	}
}

static int live_update_net_output(live_ctx_t *live_ctx,
	vp_output_t **vp_output)
{
	int rval = EA_SUCCESS;
	ea_queue_t *queue = NULL;
	int i;
	vp_output_t *tmp;
	do {
		queue = post_thread_queue(&live_ctx->thread_ctx);
		*vp_output = (vp_output_t *)ea_queue_request_carrier(queue);
		RVAL_ASSERT(*vp_output != NULL);
		tmp = *vp_output;
		tmp->out_num = live_ctx->nn_cvflow.out_num;
		for (i = 0; i < tmp->out_num; i++) {
			ea_net_update_output(live_ctx->nn_cvflow.net, tmp->out[i].tensor_name,
				tmp->out[i].out);
		}
	} while (0);
	return rval;
}

static int live_run_loop_dummy(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	ea_calc_fps_ctx_t calc_fps_ctx;
	float fps;
	ea_img_resource_data_t data;
	memset(&calc_fps_ctx, 0, sizeof(ea_calc_fps_ctx_t));
	calc_fps_ctx.count_period = DEFAULT_FPS_COUNT_PERIOD;
	int i;
	nn_input_ops_type_t *ops = NULL;

	do {
		RVAL_ASSERT(live_ctx != NULL);
		ops = live_ctx->nn_input_ctx.ops;
		RVAL_ASSERT(ops->nn_input_hold_data != NULL);

		for (i = 0; i < live_ctx->nn_cvflow.in_num; i++) {
			RVAL_OK(ops->nn_input_hold_data(&live_ctx->nn_input_ctx,
				i, nn_cvflow_input(&live_ctx->nn_cvflow, i), &data));
		}

		do {
			EA_MEASURE_TIME_START();
			RVAL_OK(nn_cvflow_inference(&live_ctx->nn_cvflow));
			EA_MEASURE_TIME_END("network forward time: ");
			fps = ea_calc_fps(&calc_fps_ctx);
			if (fps > 0) {
				EA_LOG_NOTICE("fps %.1f\n", fps);
			}
		} while (live_ctx->sig_flag == 0);

		for (i = 0; i < live_ctx->nn_cvflow.in_num; i++) {
			RVAL_OK(ops->nn_input_release_data(&live_ctx->nn_input_ctx,
			&data, i));
		}
	} while (0);

	return rval;
}

static int live_convert_yuv_data_to_bgr_data_for_postprocess(live_params_t *params, img_set_t *img_set)
{
	int rval = EA_SUCCESS;
	size_t shape[EA_DIM] = {0};
	int img_src_id = 0;

	do {
		if (params->use_pyramid == IN_SRC_ON) {
			img_src_id = params->pyramid[1];
		}

		if (img_set->bgr == NULL) {
			shape[EA_N] = ea_tensor_shape(img_set->img[0].tensor_group[img_src_id])[EA_N];
			shape[EA_C] = 3;
			shape[EA_H] = ea_tensor_shape(img_set->img[0].tensor_group[img_src_id])[EA_H];
			shape[EA_W] = ea_tensor_shape(img_set->img[0].tensor_group[img_src_id])[EA_W];
			if (ea_tensor_related(img_set->img[0].tensor_group[img_src_id]) == NULL) {
				shape[EA_H] = shape[EA_H] * 2 / 3;
			}
			img_set->bgr = ea_tensor_new(EA_U8, shape, 0);
			RVAL_ASSERT(img_set->bgr != NULL);
		}
		RVAL_OK(ea_cvt_color_resize(img_set->img[0].tensor_group[img_src_id],
			img_set->bgr, EA_COLOR_YUV2BGR_NV12, EA_VP));
	} while (0);

	return rval;
}

static int live_run_loop_without_dummy(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	int i = 0;
	ea_calc_fps_ctx_t calc_fps_ctx;
	float fps;
	ea_queue_t *queue = NULL;
	vp_output_t *vp_output = NULL;
	calc_fps_ctx.count_period = DEFAULT_FPS_COUNT_PERIOD;
	img_set_t *img_set;
	nn_input_ops_type_t *ops = NULL;
	memset(&calc_fps_ctx, 0, sizeof(ea_calc_fps_ctx_t));
	int fps_notice_flag = 0;

	do {
		RVAL_ASSERT(live_ctx != NULL);
		ops = live_ctx->nn_input_ctx.ops;
		RVAL_ASSERT(ops->nn_input_hold_data != NULL);
		RVAL_OK(live_update_net_output(live_ctx, &vp_output));
		img_set = post_thread_get_img_set(&live_ctx->thread_ctx, live_ctx->seq);
		live_ctx->seq++;
		for (i = 0; i < live_ctx->nn_cvflow.in_num; i++) {
			RVAL_OK(ops->nn_input_hold_data(&live_ctx->nn_input_ctx,
				i, nn_cvflow_input(&live_ctx->nn_cvflow, i), &(img_set->img[i])));
			if (img_set->img[i].tensor_group == NULL) {
				EA_LOG_NOTICE("All files are handled\n");
				live_ctx->sig_flag = 1;
				break;
			}
		}
		RVAL_BREAK();
		if (live_ctx->sig_flag) {
			break;
		}
		if (params->mode == RUN_LIVE_MODE &&
			params->enable_hold_img_flag == IN_SRC_ON) {
			RVAL_OK(live_convert_yuv_data_to_bgr_data_for_postprocess(params, img_set));
		}
		vp_output->arg = img_set;
		EA_MEASURE_TIME_START();
		RVAL_OK(nn_cvflow_inference(&live_ctx->nn_cvflow));
		live_ctx->loop_count--;
		if (live_ctx->loop_count == 0) {
			EA_MEASURE_TIME_END("network forward time: ");
			live_ctx->loop_count = INTERVAL_PRINT_PROCESS_TIME;
		}

		if (params->mode == RUN_LIVE_MODE) {
			fps = ea_calc_fps(&calc_fps_ctx);
			if (fps > 0) {
				if (fps_notice_flag == 0) {
					EA_LOG_NOTICE("!!! FPS based on frame query, preprocess, and inference.");
					fps_notice_flag = 1;
				}
				EA_LOG_NOTICE("fps %.1f\n", fps);
			}
		}
		for (i = 0; i < vp_output->out_num; i++) {
			RVAL_OK(ea_tensor_sync_cache(vp_output->out[i].out, EA_VP, EA_CPU));
		}
		RVAL_BREAK();
		queue = post_thread_queue(&live_ctx->thread_ctx);
		RVAL_OK(ea_queue_en(queue, vp_output));

	} while (live_ctx->sig_flag == 0);

	return rval;
}

static int live_run_loop(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	do {
		if (params->mode == RUN_DUMMY_MODE) {
			RVAL_OK(live_run_loop_dummy(live_ctx, params));
		} else {
			RVAL_OK(live_run_loop_without_dummy(live_ctx, params));
		}
	} while (0);
	return rval;
}

static void sig_stop(int a)
{
	(void)a;
	live_ctx.sig_flag = 1;
}

int main(int argc, char **argv)
{
	int rval = 0;
	live_params_t params;

	signal(SIGINT, sig_stop);
	signal(SIGQUIT, sig_stop);
	signal(SIGTERM, sig_stop);

	memset(&live_ctx, 0, sizeof(live_ctx_t));
	do {
		RVAL_OK(init_param(argc, argv, &params));
		RVAL_OK(live_init(&live_ctx, &params));
		RVAL_OK(live_run_loop(&live_ctx, &params));
	} while (0);
	live_deinit(&live_ctx, &params);

	return rval;
}
