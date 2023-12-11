/*******************************************************************************
 * test_yolov8.h
 *
 ******************************************************************************/
#include "yolov8_class.h"
#include <stdio.h> 
#include <stdlib.h>
#include <eazyai.h>
#include <time.h>
//#include "nn_lua.h"
#include "nn_arm.h"
#include <string.h>
// #include "yolov8_utils/object.hpp"
#include "point.hpp"
#include "pthread.h"

// #include "yolov8_utils/bounding_box.hpp"
// using namespace cv;
// #define atoa(x)
YoloV8_Class::YoloV8_Class(Config_S *config,int argc, char **argv)
{
 int rval = 0; 
  cout<<"new live_params_t"<<endl;
  params = new live_params_t;
  cout<<"new live_ctx_t"<<endl;
  live_ctx = new live_ctx_t;

  cout<<"Start AMBA init_param"<<endl;
  rval = init_param(argc, argv, params, config); // 2023-12-11 Alister add
  // rval = init_param(argc, argv, params);
  cout<<"End AMBA init_param"<<endl;
  cout<<"Start AMBA live_init"<<endl;
  rval = live_init(live_ctx, params);
  cout<<"End AMBA live_init"<<endl;
  std::string dlcFilePath = config->modelPath;
  std::string rumtimeStr = config->runtime;
  // Runtime
  bool usingInitCaching = false;
  bool staticQuantization = false;
  bool useUserSuppliedBuffers = false;

  // STEP4: Init Model Input/Output Tensor
  printf("[YOLOv8::YOLOv8(Config_S *config)] Start   _initModelIO()\n");
  _initModelIO();
  printf("[YOLOv8::YOLOv8(Config_S *config)] End   _initModelIO()\n");
  // Output Decoder

}

YoloV8_Class::YoloV8_Class(int argc, char **argv)
{
	int rval = 0;

	// params = new live_params_t;
	// live_ctx = new live_ctx_t;

	rval = init_param(argc, argv, params);
	rval = live_init(live_ctx, params);
}

YoloV8_Class::YoloV8_Class(int argc, char **argv, live_params_t *params, live_ctx_t *live_ctx)
{
	// params = new live_params_t;
	// live_ctx = new live_ctx_t;
	int rval = 0;
	rval = init_param(argc, argv, params);
	rval = live_init(live_ctx, params);
}
// YoloV8_Class::~YoloV8_Class()
// {
	

// 	post_thread_deinit( &live_ctx->thread_ctx, &live_ctx->nn_cvflow);
// 	nn_cvflow_deinit(&live_ctx->nn_cvflow);
// 	cv_env_deinit(live_ctx);
// 	if (live_ctx->f_result > -1) {
// 		close(live_ctx->f_result);
// 		live_ctx->f_result = -1;
// 	}

// 	delete params;
// 	delete live_ctx;

// 	params = nullptr;
// 	live_ctx = nullptr;
// }


int YoloV8_Class::test_yolov8_init(int argc, char **argv, live_params_t *params, live_ctx_t *live_ctx)
{
    int rval = 0;
	rval = init_param(argc, argv, params);
	rval = live_init(live_ctx,params);
	return rval;
}

int YoloV8_Class::init(int argc, char **argv, live_params_t *params, live_ctx_t *live_ctx)
{
	int rval = 0;
	rval = init_param(argc, argv, params);
	rval = live_init(live_ctx,params);
	return rval;
};

//Alister add 2023-12-07
bool YoloV8_Class::fileExists(const std::string& path) {
    FILE *fp;
    if (fp = fopen(path.c_str(), "r")) {
        fclose(fp);
        return true;
    }
    return false;
}


int YoloV8_Class::parse_param(int argc, char **argv, live_params_t *params)
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


int YoloV8_Class::check_params(live_params_t *params)
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
};

int YoloV8_Class::init_param(int argc, char **argv, live_params_t *params)
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
};


int YoloV8_Class::init_param(int argc, char **argv, live_params_t *params, Config_S *config)
{
	int rval = EA_SUCCESS;

	memset(params, 0, sizeof(live_params_t));

	params->mode = config->AMBAInitParamConfig.mode;
	params->log_level = config->AMBALuaConfig.log_level;
	params->draw_mode = config->AMBAInitParamConfig.draw_mode;
	params->rgb = config->AMBAInitParamConfig.input_color_type;

	// params->mode = RUN_LIVE_MODE;
	// params->log_level = EA_LOG_LEVEL_NOTICE;
	// params->draw_mode = DRAW_BBOX_TEXTBOX;
	// params->rgb = BGR_PLANAR;


	params->yuv_flag = IN_SRC_OFF;
	params->use_pyramid = IN_SRC_OFF;
	params->enable_fsync_flag = IN_SRC_ON;

	params->canvas_id = DEFAULT_CANVAS_ID;
	params->vout_id = DEFAULT_VOUT_ID;
	params->stream_id = DEFAULT_STREAM_ID;

	params->queue_size = config->AMBAInitParamConfig.queue_size;
	params->thread_num = config->AMBAInitParamConfig.thread_num;
	params->acinf_gpu_id = config->AMBAInitParamConfig.acinf_gpu_id;
	params->overlay_buffer_offset = config->AMBAInitParamConfig.overlay_buffer_offset;
	// params->queue_size = 1;
	// params->thread_num = 1;
	// params->acinf_gpu_id = -1;
	// params->overlay_buffer_offset = -1;


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
};


void YoloV8_Class::live_set_post_thread_params(live_params_t *params,
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
};

int YoloV8_Class::cv_env_init(live_ctx_t *live_ctx, live_params_t *params)
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
		ctx->ops = nn_intput_get_ops(run_mode_t(params->mode));
		RVAL_ASSERT(ctx->ops != NULL);
		ops = ctx->ops;
		RVAL_ASSERT(ops->nn_input_init != NULL);
		RVAL_OK(ops->nn_input_init(ctx)); //here cause error 2023-12-06
	} while (0);
	return rval;
};



int YoloV8_Class::live_init(live_ctx_t *live_ctx, live_params_t *params)
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
		RVAL_OK(cv_env_init(live_ctx, params)); //here cause error 2023-12-06
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


int YoloV8_Class::run()
{
	 
  // STEP1: load image to input tensor
//   if (!loadInput(imgFrame))
//   {

//     return false;
//   }
  // STEP2: run inference
  int sig_flag = YoloV8_Class::test_yolov8_run();
  if(sig_flag!=0)
  {
	return false;
  }

  // STEP3: post processing
  YoloV8_Class::_OD_postProcessing();
  
  return true;
}

int YoloV8_Class::test_yolov8_run_2(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	int sig_flag = 0;
	do {
		if (params->mode == RUN_DUMMY_MODE) {
			//printf("In params->mode == RUN_DUMMY_MODE\n");
			sig_flag = live_run_loop_dummy(live_ctx, params); //RVAL_OK
		} else {
			//printf("In else \n");
			sig_flag = live_run_loop_without_dummy(live_ctx, params); //RVAL_OK
		}
	} while (0);
	// return rval;
	return sig_flag;
}
int YoloV8_Class::test_yolov8_run()
{	
	printf("In test_yolov8_run\n");
    int rval = EA_SUCCESS;
	int sig_flag = 0;
	do {
		//printf("In test_yolov8_run , start if (params->mode == RUN_DUMMY_MODE) \n");
		if (params->mode == RUN_DUMMY_MODE) {
			printf("Start live_run_loop_dummy\n");
			sig_flag = live_run_loop_dummy(YoloV8_Class::live_ctx, YoloV8_Class::params); //RVAL_OK
		} else {
			printf("Start live_run_loop_without_dummy\n");
			sig_flag = live_run_loop_without_dummy(YoloV8_Class::live_ctx, YoloV8_Class::params); //RVAL_OK
			printf("End live_run_loop_without_dummy\n");
		}
	} while (0);
	// return rval;
	return sig_flag;
}

void YoloV8_Class::yolov8_thread_join()
{	
	int i, j;
	
	post_thread_t *thread = NULL;
	post_thread_params_t *params = NULL;
	thread = new post_thread_t;
	params = new post_thread_params_t;
	*params = live_ctx->thread_ctx.params;
	
	if (live_ctx->thread_ctx.thread) {
		for (i = 0; i < params->thread_num; i++) 
		{
			thread = &live_ctx->thread_ctx.thread[i];
			
			if (thread->thread_created) 
			{
				
				if (thread->exception_exit == 0) 
				{
					
					pthread_join(thread->tidp, NULL);
				}
			}
		}
	}

}

cv::Mat YoloV8_Class::Get_img()
{
	cv::Mat bgr = cv::Mat::zeros(1920,1080,CV_8UC3);
	int rval;
	// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	cout<<"Start if(live_ctx->thread_ctx.thread->nn_arm_ctx.bgr!=NULL)"<<endl;
	if(live_ctx->thread_ctx.thread->nn_arm_ctx.bgr!=NULL)
	{	
		// pthread_mutex_lock(&mutex);
		cout<<"Get_img : In if Start"<<endl;
		// usleep(100);
		ea_tensor_t *tensor = (ea_tensor_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.bgr;
		cout<<"Get_img : end tensor"<<endl;
		rval = tensor2mat_bgr2bgr(tensor, bgr);
		cout<<"Get_img : In if End"<<endl;
		// pthread_mutex_unlock(&mutex);
	}
	else
	{	
		bgr = cv::Mat::zeros(1920,1080,CV_8UC3);
	}

	return bgr;
}


std::vector<BoundingBox> YoloV8_Class::Get_yolov8_Bounding_Boxes(live_ctx_t *live_ctx, live_params_t *params,std::vector<BoundingBox> bboxList)
{
    //Object obj;
	printf("Start initial yolov8_result_t~~~\n");
	int i;
	for (i = 0; i < params->thread_num; i++) 
	{
		// thread = &thread_ctx->thread[i];
		// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
		yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread[i].nn_arm_ctx.result;
		printf("end initial yolov8_result_t~~~\n");
		int i = 0;
		printf("in tracker function~~~\n");
		for ( i = 0; i < yolov8_result->num; i++)
		{
			EA_LOG_DEBUG("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%s\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].label);
			
			printf("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%d\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].id);

			bboxList.push_back(BoundingBox(yolov8_result->bbox[i].x_start,
									yolov8_result->bbox[i].y_start,
									yolov8_result->bbox[i].x_end,
									yolov8_result->bbox[i].y_end,
									yolov8_result->bbox[i].id));	
		}
		printf("Show bboxList ~~~~~~\n");
		for (int i=0;i<bboxList.size();i++)
			{	
				
				printf("%f, %f, %f, %f, %d\n",bboxList[i].x1,
											bboxList[i].y1,
											bboxList[i].x2,
											bboxList[i].y2,
											bboxList[i].label);
			
			}

	}
    return bboxList;
}
int YoloV8_Class::Get_Yolov8_Bounding_Boxes_Ver2(v8xyxy bboxList[MAX_YOLO_BBX], int model_size)
{	
	int bbx_number = 0;
  //Object obj;
	printf("[Get_Yolov8_Bounding_Boxes]start initial yolov8_result~~~\n");
 	// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
	// printf("[Get_Yolov8_Bounding_Boxes]End initial yolov8_result~~~\n");
	int j;
	for (j = 0; j < params->thread_num; j++) 
	{
		// thread = &thread_ctx->thread[i];
		// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
		yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread[j].nn_arm_ctx.result;
		int i = 0;
		cout<<"yolov8_result->num = "<<yolov8_result->num<<endl;
		for ( i = 0; i < yolov8_result->num; i++)
		{
			EA_LOG_DEBUG("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%s\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].label);
			
			printf("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%s\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].label);

			// cout<<"[Get_Yolov8_Bounding_Boxes]img.rows = "<<img.rows<<endl;
			// cout<<"[Get_Yolov8_Bounding_Boxes]img.cols = "<<img.cols<<endl;
			int x1 = int(yolov8_result->bbox[i].x_start * 416);
			int y1 = int(yolov8_result->bbox[i].y_start * 416);
			int x2 = int(yolov8_result->bbox[i].x_end * 416);
			int y2 = int(yolov8_result->bbox[i].y_end * 416);
			int id = yolov8_result->bbox[i].id;
			float c_prob = yolov8_result->bbox[i].score;
			// int x1 = int(yolov8_result->bbox[i].x_start * img.cols);
			// int y1 = int(yolov8_result->bbox[i].y_start * img.rows);
			// int x2 = int(yolov8_result->bbox[i].x_end * img.cols);
			// int y2 = int(yolov8_result->bbox[i].y_end * img.rows);
			// float x1 = float(yolov8_result->bbox[i].x_start);
			// float y1 = float(yolov8_result->bbox[i].y_start);
			// float x2 = float(yolov8_result->bbox[i].x_end);
			// float y2 = float(yolov8_result->bbox[i].y_end);
			// cout<<"[Get_Yolov8_Bounding_Boxes] x1:"<<x1<<" y1:"<<y1<<" x2:"<<x2<<" y2:"<<y2<<endl;
			if(x1!=0 && x2!=0 && y1!=0 && y2!=0)
			{	
				struct v8xyxy bb;
				 	bb.x1 = x1;
					bb.y1 = y1;
					bb.x2 = x2;
					bb.y2 = y2;
					bb.c_prob = c_prob; // class probability, apply sigmoid()
					bb.c = id; // class
				
				bboxList[i] = bb;
				bbx_number+=1;
				// bboxList.push_back(bb);
			}
			

		}
		printf("[Get_Yolov8_Bounding_Boxes]print BB~~~~~~~~~~~~~~~~~~\n");
		printf("[Get_Yolov8_Bounding_Boxes]Show v8xyxy bboxList ~~~~~~\n");
		for (int i=0;i<bbx_number;i++)
			{
				if(bboxList[i].x1!=0 && bboxList[i].y1!=0 && bboxList[i].x2 && bboxList[i].y2)
				{
					printf("%d, %d, %d, %d, %f, %d\n",bboxList[i].x1,
													bboxList[i].y1,
													bboxList[i].x2,
													bboxList[i].y2,
													bboxList[i].c_prob,
													bboxList[i].c);
				}
				
				// printf("%f, %f, %f, %f, %d\n",bboxList[i].x1,
				// 							bboxList[i].y1,
				// 							bboxList[i].x2,
				// 							bboxList[i].y2,
				// 							bboxList[i].label);
			}
	}
		return bbx_number;
}

int YoloV8_Class::Get_Yolov8_Bounding_Boxes(std::vector<BoundingBox> &bboxList,cv::Mat img)
{
	//Object obj;
	printf("[Get_Yolov8_Bounding_Boxes]start initial yolov8_result~~~\n");
 	// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
	// printf("[Get_Yolov8_Bounding_Boxes]End initial yolov8_result~~~\n");
	int j;
	for (j = 0; j < params->thread_num; j++) 
	{
		// thread = &thread_ctx->thread[i];
		// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
		yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread[j].nn_arm_ctx.result;
		int i = 0;
		cout<<"yolov8_result->num = "<<yolov8_result->num<<endl;
		for ( i = 0; i < yolov8_result->num; i++)
		{
			EA_LOG_DEBUG("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%s\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].label);
			
			printf("num:%d, id:%d, x1:%f, y1:%f, x2:%f, y2:%f, score:%f, label:%s\n",
			yolov8_result->num,
			yolov8_result->bbox[i].id,
			yolov8_result->bbox[i].x_start,
			yolov8_result->bbox[i].y_start,
			yolov8_result->bbox[i].x_end,
			yolov8_result->bbox[i].y_end,
			yolov8_result->bbox[i].score,
			yolov8_result->bbox[i].label);

			// cout<<"[Get_Yolov8_Bounding_Boxes]img.rows = "<<img.rows<<endl;
			// cout<<"[Get_Yolov8_Bounding_Boxes]img.cols = "<<img.cols<<endl;
			int x1 = int(yolov8_result->bbox[i].x_start * 416);
			int y1 = int(yolov8_result->bbox[i].y_start * 416);
			int x2 = int(yolov8_result->bbox[i].x_end * 416);
			int y2 = int(yolov8_result->bbox[i].y_end * 416);
			// int x1 = int(yolov8_result->bbox[i].x_start * img.cols);
			// int y1 = int(yolov8_result->bbox[i].y_start * img.rows);
			// int x2 = int(yolov8_result->bbox[i].x_end * img.cols);
			// int y2 = int(yolov8_result->bbox[i].y_end * img.rows);
			// float x1 = float(yolov8_result->bbox[i].x_start);
			// float y1 = float(yolov8_result->bbox[i].y_start);
			// float x2 = float(yolov8_result->bbox[i].x_end);
			// float y2 = float(yolov8_result->bbox[i].y_end);
			// cout<<"[Get_Yolov8_Bounding_Boxes] x1:"<<x1<<" y1:"<<y1<<" x2:"<<x2<<" y2:"<<y2<<endl;
			if(x1!=0 && x2!=0 && y1!=0 && y2!=0)
			{
				bboxList.push_back(BoundingBox( x1,y1,x2,y2,
									yolov8_result->bbox[i].id));
			}
			

		}
		printf("[Get_Yolov8_Bounding_Boxes]print BB~~~~~~~~~~~~~~~~~~\n");
		printf("[Get_Yolov8_Bounding_Boxes]Show bboxList ~~~~~~\n");
		for (int i=0;i<bboxList.size();i++)
			{	
				printf("%d, %d, %d, %d, %d\n",bboxList[i].x1,
											bboxList[i].y1,
											bboxList[i].x2,
											bboxList[i].y2,
											bboxList[i].label);
				// printf("%f, %f, %f, %f, %d\n",bboxList[i].x1,
				// 							bboxList[i].y1,
				// 							bboxList[i].x2,
				// 							bboxList[i].y2,
				// 							bboxList[i].label);
			}
	}
		return true;
}


int YoloV8_Class::tensor2mat_bgr2bgr(ea_tensor_t *tensor, cv::Mat &bgr)
{
	int rval = EA_SUCCESS;
	printf("Start tensor2mat_bgr2bgr~~~~~~~\n");
	printf("Get *c1~*c3_data\n");
	void *c1_data = ea_tensor_data_for_read(tensor, EA_CPU);
	void *c2_data = (uint8_t *)ea_tensor_data_for_read(tensor, EA_CPU) + ea_tensor_shape(tensor)[2] * ea_tensor_pitch(tensor);
	void *c3_data = (uint8_t *)ea_tensor_data_for_read(tensor, EA_CPU) + ea_tensor_shape(tensor)[2] * ea_tensor_pitch(tensor) * 2;
	
	if (c1_data==NULL ||  c2_data==NULL || c3_data==NULL){
		if (c1_data == NULL){
			printf("c1_data is NULL \n");
		}

		if (c2_data == NULL){
			printf("c2_data is NULL \n");
		}


		if (c3_data == NULL){
			printf("c3_data is NULL \n");
		}
			
		return EA_FAIL;
	} 
	
	printf("[tensor2mat_bgr2bgr] Start Create cv::Mat c1 , c2 , c3 \n");
	cv::Mat c1(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c1_data, ea_tensor_pitch(tensor));
	cv::Mat c2(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c2_data, ea_tensor_pitch(tensor));
	cv::Mat c3(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c3_data, ea_tensor_pitch(tensor));
	printf("[tensor2mat_bgr2bgr] End Create cv::Mat c1 , c2 , c3 \n");
	std::vector<cv::Mat> channels;
	channels.clear();
	printf("[tensor2mat_bgr2bgr] start put c1~c3 into channels\n");
	do {
		if (ea_tensor_shape(tensor)[1] == 1) {
			channels.push_back(c1);
		}
		else if (ea_tensor_shape(tensor)[1] == 3)
		{
			channels.push_back(c1);
			channels.push_back(c2);
			channels.push_back(c3);
		}
		else {
			EA_LOG_ERROR("channel number %lu is not 1 or 3 for saving to jpeg\n", ea_tensor_shape(tensor)[1]);
			rval = EA_FAIL;
			break;
		}
		printf("[tensor2mat_bgr2bgr] Start merge channels into bgr image \n");
		cv::merge(channels, bgr);
		printf("[tensor2mat_bgr2bgr] End merge channels into bgr image \n");
	} while (0);
	printf("End of tensor2mat_bgr2bgr~~~~~~~\n");
	return rval;
}


int YoloV8_Class::tensor2mat_rgb2bgr(ea_tensor_t *tensor, cv::Mat &bgr)
{
	int rval = EA_SUCCESS;
	void *c1_data = ea_tensor_data_for_read(tensor, EA_CPU);
	void *c2_data = (uint8_t *)ea_tensor_data_for_read(tensor, EA_CPU) + ea_tensor_shape(tensor)[2] * ea_tensor_pitch(tensor);
	void *c3_data = (uint8_t *)ea_tensor_data_for_read(tensor, EA_CPU) + ea_tensor_shape(tensor)[2] * ea_tensor_pitch(tensor) * 2;
	cv::Mat c1(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c1_data, ea_tensor_pitch(tensor));
	cv::Mat c2(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c2_data, ea_tensor_pitch(tensor));
	cv::Mat c3(ea_tensor_shape(tensor)[2], ea_tensor_shape(tensor)[3], CV_8UC1, c3_data, ea_tensor_pitch(tensor));
	std::vector<cv::Mat> channels;

	do {
		if (ea_tensor_shape(tensor)[1] == 1) {
			channels.push_back(c1);
		}
		else if (ea_tensor_shape(tensor)[1] == 3)
		{
			channels.push_back(c1);
			channels.push_back(c2);
			channels.push_back(c3);
		}
		else {
			EA_LOG_ERROR("channel number is not 1 or 3 for saving to jpeg\n");
			rval = EA_FAIL;
			break;
		}

		std::swap(channels[0], channels[2]);
		cv::merge(channels, bgr);
	} while (0);

	return rval;
}

int YoloV8_Class::tensor2mat_yuv2bgr_nv12(ea_tensor_t *tensor, cv::Mat &bgr)
{
	int rval = EA_SUCCESS;
	cv::Mat nv12(ea_tensor_shape(tensor)[2] +
		(ea_tensor_related(tensor) == NULL ? 0 : ea_tensor_shape(ea_tensor_related(tensor))[2]),
		ea_tensor_shape(tensor)[3], CV_8UC1);
	uint8_t *p_src = NULL;
	uint8_t *p_dst = NULL;
	size_t h;

	do {
		EA_R_ASSERT(ea_tensor_shape(tensor)[1] == 1);

		p_src = (uint8_t *)ea_tensor_data_for_read(tensor, EA_CPU);
		p_dst = nv12.data;
		for (h = 0; h < ea_tensor_shape(tensor)[2]; h++) {
			memcpy(p_dst, p_src, ea_tensor_shape(tensor)[3]);
			p_src += ea_tensor_pitch(tensor);
			p_dst += ea_tensor_shape(tensor)[3];
		}

		if (ea_tensor_related(tensor)) {
			p_src = (uint8_t *)ea_tensor_data_for_read(ea_tensor_related(tensor), EA_CPU);
			for (h = 0; h < ea_tensor_shape(ea_tensor_related(tensor))[2]; h++) {
				memcpy(p_dst, p_src, ea_tensor_shape(ea_tensor_related(tensor))[3]);
				p_src += ea_tensor_pitch(ea_tensor_related(tensor));
				p_dst += ea_tensor_shape(ea_tensor_related(tensor))[3];
			}
		}

		#if CV_VERSION_MAJOR < 4
			cv::cvtColor(nv12, bgr, CV_YUV2BGR_NV12);
		#else
			cv::cvtColor(nv12, bgr, cv::COLOR_YUV2BGR_NV12);
		#endif
	} while (0);

	return rval;
}

void YoloV8_Class::Draw_Yolov8_Bounding_Boxes(std::vector<BoundingBox> &bboxList,int c, cv::Mat img){
	int rval = 0;
	printf("[Draw_Yolov8_Bounding_Boxes] Get dis_win_h and w\n");
	// nn_arm_context_t *ctx;
	//live_ctx->thread_ctx.thread->nn_arm_ctx.roi->w;
	// live_ctx->thread_ctx.thread->nn_arm_ctx.input

	// int dis_win_h = 1080; //= ea_tensor_shape(ctx->input)[EA_W];
	// int dis_win_w = 810; //=ea_tensor_shape(ctx->input)[EA_H];
	
	
	printf("[Draw_Yolov8_Bounding_Boxes] start initial img \n");
	// ea_tensor_t *tensor = live_ctx->thread_ctx.thread->nn_arm_ctx.bgr;
	cout<<"params->thread_num="<<params->thread_num<<endl;
	int j;
	for (j = 0; j < params->thread_num; j++) 
	{
		// thread = &thread_ctx->thread[i];
		// yolov8_result_t *yolov8_result = (yolov8_result_t *)live_ctx->thread_ctx.thread->nn_arm_ctx.result;
		// ea_tensor_t *tensor = live_ctx->thread_ctx.thread->nn_arm_ctx.bgr;
		// // ea_tensor_t *tensor = img_set[j].bgr; //segmentation fault
		// cv::Mat bgr;
		
		// rval = tensor2mat_bgr2bgr(tensor, bgr);
		//tensor2mat_yuv2bgr_nv12 
		//tensor2mat_bgr2bgr
		//tensor2mat_rgb2bgr
		int dis_win_h = img.rows;
		int dis_win_w = img.cols;
		printf("dis_win_h=%d,dis_win_w=%d\n ",dis_win_h,dis_win_w);
		// cv::Mat img(dis_win_h, dis_win_w, CV_8UC3,cv::Scalar::all(0));
		
		do{
			cout << "dbg e" << endl;
			cout<<"bboxList.size()="<<bboxList.size()<<endl;
			printf("[Draw_Yolov8_Bounding_Boxes] End initial img\n");
			for (int i=0;i<int(bboxList.size());i++)
			{
				// int bbox_start_x = bboxList[i].x1 * dis_win_w;
				// int bbox_start_y = bboxList[i].y1 * dis_win_h;
				// int bbox_end_x = bboxList[i].x2 * dis_win_w;
				// int bbox_end_y = bboxList[i].y2 * dis_win_h;
				int bbox_start_x = bboxList[i].x1 * 1;
				int bbox_start_y = bboxList[i].y1 * 1;
				int bbox_end_x = bboxList[i].x2 * 1;
				int bbox_end_y = bboxList[i].y2 * 1;
				//cv::rectangle()
				// cv::Point pt1(bbox_start_x, bbox_start_y);
				// cv::Point pt2(bbox_end_x, bbox_end_y );
				// cv::rectangle(img, pt1, pt2, cv::Scalar(255,127,0),4,1,0);
			}

		}while(0);
		printf("[Draw_Yolov8_Bounding_Boxes]imwrite \n");

		int x = c;
		// int length = snprintf( NULL, 0, "%d", x );
		// char* str = malloc( length + 1 );
		// snprintf( str, length + 1, "%d", x );

		char buf[64];
		int n = sprintf( buf, "%d", c);
		printf("%s %d\n", buf, n);

		char dest[64];
		memset(dest, 0, sizeof(dest));

		strcat(dest,"./");
		strcat(dest,buf);
		strcat(dest,".jpg");
		cv::imwrite(dest, img);
		//cv::imwrite("./2023-10-13.png", bgr);
	}
	// cv::imwrite("./2023-10-06-2017.jpg", bgr);
	// printf("[Draw_Yolov8_Bounding_Boxes]imshow \n");
	// cv::imshow("test", img);
    // cv::waitKey(0);
	
};
void YoloV8_Class::Draw_Yolov8_Bounding_Boxes(std::vector<BoundingBox> bboxList, live_ctx_t *live_ctx, live_params_t *params)
{
	printf("[Draw_Yolov8_Bounding_Boxes] Get dis_win_h and w\n");
	// nn_arm_context_t *ctx;
	//live_ctx->thread_ctx.thread->nn_arm_ctx.roi->w;
	// live_ctx->thread_ctx.thread->nn_arm_ctx.input
	// yolov8_ctx.input_w = ea_tensor_shape(live_ctx->thread_ctx.thread->nn_arm_ctx.input)[EA_W];
	// yolov8_ctx.input_h = ea_tensor_shape(live_ctx->thread_ctx.thread->nn_arm_ctx.input)[EA_H];
	// yolov8_ctx.input_w = ea_tensor_shape(live_ctx->thread_ctx.thread->nn_arm_ctx.input)[0];
	// yolov8_ctx.input_h = ea_tensor_shape(live_ctx->thread_ctx.thread->nn_arm_ctx.input)[1];
	int dis_win_h = 1080; //= ea_tensor_shape(ctx->input)[EA_W];
	int dis_win_w = 810; //=ea_tensor_shape(ctx->input)[EA_H];
	// int dis_win_h = live_ctx->thread_ctx.input_ctx->roi.h
	// int dis_win_w = 500;
	printf("dis_win_h=%d,dis_win_w=%d\n ",dis_win_h,dis_win_w);
	printf("[Draw_Yolov8_Bounding_Boxes] start initial img \n");
	// cv::Mat img(dis_win_h, dis_win_w, CV_8UC3,live_ctx->thread_ctx.input_queue );
	// cv::Mat img(dis_win_h, dis_win_w, CV_8UC3,live_ctx->thread_ctx.thread->nn_arm_ctx.input );
	// cv::Mat img(dis_win_h, dis_win_w, CV_8UC3,cv::Scalar::all(0));
	// live_ctx->thread_ctx.img_set->img
	cv::Mat img(dis_win_h, dis_win_w, CV_8UC3,live_ctx->thread_ctx.img_set->img);
	printf("[Draw_Yolov8_Bounding_Boxes] End initial img\n");
	for (int i=0;i<int(bboxList.size());i++)
		{
			int bbox_start_x = bboxList[i].x1 * dis_win_w;
			int bbox_start_y = bboxList[i].y1 * dis_win_h;
			int bbox_end_x = bboxList[i].x2 * dis_win_w;
			int bbox_end_y = bboxList[i].y2 * dis_win_h;

			//cv::rectangle()
			// cv::Point pt1(bbox_start_x, bbox_start_y);
			// cv::Point pt2(bbox_end_x, bbox_end_y );
			// cv::rectangle(img, pt1, pt2, cv::Scalar(255,127,0), -1, cv::LINE_4);
			
		}
	//printf("[Draw_Yolov8_Bounding_Boxes]imshow \n");
	// EA_LOG_NOTICE("sdfsdfsdfsdfsdwerrwerwerwerwefdsdfsd");
	//cv::imshow("test", img);
    // Wait for any keystroke
    //cv::waitKey(0);
	// EA_LOG_NOTICE("sdfsdfsdfsdfsdfdsdfsd");
	printf("[Draw_Yolov8_Bounding_Boxes]imwrite \n");
	cv::imwrite("./test_2023_10_04.jpg", img);
}


void YoloV8_Class::cv_env_deinit(live_ctx_t *live_ctx)
{
	nn_input_ops_type_t *ops = NULL;
	ops = live_ctx->nn_input_ctx.ops;
	if (ops) {
		EA_LOG_ASSERT(ops->nn_input_deinit != NULL);
		ops->nn_input_deinit(&live_ctx->nn_input_ctx);
	}
};

void YoloV8_Class::live_deinit(live_ctx_t *live_ctx, live_params_t *params)
{
	post_thread_deinit(&live_ctx->thread_ctx, &live_ctx->nn_cvflow);
	nn_cvflow_deinit(&live_ctx->nn_cvflow);
	cv_env_deinit(live_ctx);
	if (live_ctx->f_result > -1) {
		close(live_ctx->f_result);
		live_ctx->f_result = -1;
	}
}

void YoloV8_Class::test_yolov8_deinit()
{
		post_thread_deinit( &live_ctx->thread_ctx, &YoloV8_Class::live_ctx->nn_cvflow);
	nn_cvflow_deinit(&live_ctx->nn_cvflow);
	cv_env_deinit(live_ctx);
	if (live_ctx->f_result > -1) {
		close(live_ctx->f_result);
		live_ctx->f_result = -1;
	}
};

void YoloV8_Class::test_yolov8_deinit(live_ctx_t *live_ctx, live_params_t *params){
	post_thread_deinit(&live_ctx->thread_ctx, &live_ctx->nn_cvflow);
	nn_cvflow_deinit(&live_ctx->nn_cvflow);
	cv_env_deinit(live_ctx);
	if (live_ctx->f_result > -1) {
		close(live_ctx->f_result);
		live_ctx->f_result = -1;
	}
};

int YoloV8_Class::live_update_net_output(live_ctx_t *live_ctx,
	vp_output_t **vp_output)
{
	int rval = EA_SUCCESS;
	ea_queue_t *queue = NULL;
	int i;
	vp_output_t *tmp;
	cout<<"[live_update_net_output] Start do while"<<endl;
	do {
		cout<<"[live_update_net_output] Start post_thread_queue"<<endl;
		queue = post_thread_queue(&live_ctx->thread_ctx);
		cout<<"[live_update_net_output] End post_thread_queue"<<endl;

		cout<<"[live_update_net_output] Start Create ea_queue_request_carrier"<<endl;
		*vp_output = (vp_output_t *)ea_queue_request_carrier(queue);
		cout<<"[live_update_net_output] End Create ea_queue_request_carrier"<<endl;

		cout<<"Start check *vp_output != NULL"<<endl;
		RVAL_ASSERT(*vp_output != NULL);
		cout<<"End check *vp_output != NULL"<<endl;

		tmp = *vp_output;

		cout<<"[live_update_net_output] Start Assign tmp->out_num"<<endl;
		tmp->out_num = live_ctx->nn_cvflow.out_num;
		cout<<"[live_update_net_output] End Assign tmp->out_num"<<endl;

		cout<<"[live_update_net_output] Start for loop ea_net_update_output"<<endl;
		for (i = 0; i < tmp->out_num; i++) {
			ea_net_update_output(live_ctx->nn_cvflow.net, tmp->out[i].tensor_name,
				tmp->out[i].out);
		}
		cout<<"[live_update_net_output] End for loop ea_net_update_output"<<endl;
	} while (0);
	cout<<"[live_update_net_output] End do while"<<endl;
	return rval;
};

int YoloV8_Class::live_run_loop_dummy(live_ctx_t *live_ctx, live_params_t *params)
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

	//return rval;
	return live_ctx->sig_flag;
};

int YoloV8_Class::live_convert_yuv_data_to_bgr_data_for_postprocess(live_params_t *params, img_set_t *img_set)
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
};

int YoloV8_Class::live_run_loop_without_dummy(live_ctx_t *live_ctx, 
                                        live_params_t *params)
{	
	cout<<"===============In Function : live_run_loop_without_dummy======================="<<endl;
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
	cout<<"[live_run_loop_without_dummy] Start do while"<<endl;
	do {
		RVAL_ASSERT(live_ctx != NULL);
		ops = live_ctx->nn_input_ctx.ops;
		RVAL_ASSERT(ops->nn_input_hold_data != NULL);
		cout<<"Start live_update_net_output"<<endl;
		RVAL_OK(YoloV8_Class::live_update_net_output(live_ctx, &vp_output));
		cout<<"End live_update_net_output"<<endl;

		cout<<"Start post_thread_get_img_set"<<endl;
		img_set = post_thread_get_img_set(&live_ctx->thread_ctx, live_ctx->seq);
		cout<<"End post_thread_get_img_set"<<endl;
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
			RVAL_OK(YoloV8_Class::live_convert_yuv_data_to_bgr_data_for_postprocess(params, img_set));
		}
		vp_output->arg = img_set;
		EA_MEASURE_TIME_START();
		cout<<"Start nn_cvflow_inference"<<endl;
		RVAL_OK(nn_cvflow_inference(&live_ctx->nn_cvflow));
		cout<<"End nn_cvflow_inference"<<endl;
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
		cout<<"[live_run_loop_without_dummy] Start post_thread_queue"<<endl;
		queue = post_thread_queue(&live_ctx->thread_ctx);
		cout<<"[live_run_loop_without_dummy] End post_thread_queue"<<endl;
		RVAL_OK(ea_queue_en(queue, vp_output));
	} while (0);
	cout<<"[live_run_loop_without_dummy] End do while"<<endl;
	// return rval;
	return live_ctx->sig_flag;
};

int YoloV8_Class::live_run_loop(live_ctx_t *live_ctx, live_params_t *params)
{
	int rval = EA_SUCCESS;
	int sig_flag = 0;
	do {
		if (params->mode == RUN_DUMMY_MODE) {
			sig_flag = live_run_loop_dummy(live_ctx, params); //RVAL_OK
		} else {
			sig_flag = live_run_loop_without_dummy(live_ctx, params); //RVAL_OK
		}
	} while (0);
	// return rval;
	return sig_flag;
}

//=================================================================================================================================
// Alister add 2023-11-22
void YoloV8_Class::_OD_postProcessing(vector<BoundingBox> bboxlist)
{
  struct vector<v8xyxy> m_yoloOut;
  for (unsigned int i = 0; i < bboxlist.size(); i++)
  { 
    v8xyxy bb;
    bb.x1 = bboxlist[i].x1;
    bb.y1 = bboxlist[i].y1;
    bb.x2 = bboxlist[i].x2;
    bb.y2 = bboxlist[i].y2;
    bb.c  = bboxlist[i].label;
    bb.c_prob  = 1.0;
    m_yoloOut.push_back(bb);
  }
  m_numBox = m_yoloOut.size();
  // m_logger->debug("=> GET # of raw BBOX(es): {}", m_numBox);

  for(int i=0; i< m_numBox; i++)
  {
    struct v8xyxy b = m_yoloOut[i];
    // m_logger->debug("=> bbx {}: ({},{})-({},{}), c={}, conf={}", i, b.x1, b.y1, b.x2, b.y2, b.c, b.c_prob);
  }
}

void YoloV8_Class::_OD_postProcessing()
{
	// Alister add 2023-11-29
	// Get m_yoloOut 
		// std::vector<v8xyxy> m_yoloOut;
		int bbx_number = YoloV8_Class::Get_Yolov8_Bounding_Boxes_Ver2(m_yoloOut,m_inputWidth);
	// Get m_numBox
		m_numBox = bbx_number;
		cout<<"m_numBox : "<<m_numBox<<endl;

  //Original code from yolov8.cpp
//   m_numBox = m_decoder->decode((
//     float *)m_detectionBoxBuff, (float *)m_detectionConfBuff, (float *)m_detectionClsBuff, confidenceThreshold, iouThreshold, m_yoloOut);

}
//=========================2023-11-29 Copy from yolov8.cpp======================================================================================
// Object color
const std::vector<cv::Scalar> object_colors = {
  {255, 51, 123}, // Person
  {0, 0, 255},    // Bicycle
  {0, 255, 0},    // Vehicle
  {0, 255, 128},  // Motorbike
};


/////////////////////////
// public member functions
////////////////////////
YoloV8_Class::YoloV8_Class(Config_S *config)  // main function
{
  // auto m_logger = spdlog::stdout_color_mt("YOLOv8");
  // m_logger->set_pattern("[%n] [%^%l%$] %v");

  // if (config->stDebugConfig.yolov8)
  // {
  //   m_logger->set_level(spdlog::level::debug);
  // }
  // else
  // {
  //   m_logger->set_level(spdlog::level::info);
  // }
  cout<<"new live_params_t"<<endl;
  //params = new live_params_t;
  cout<<"new live_ctx_t"<<endl;
  //live_ctx = new live_ctx_t;

  std::string dlcFilePath = config->modelPath;
  std::string rumtimeStr = config->runtime;

  // Runtime
  // static zdl::DlSystem::Runtime_t runtime = zdl::DlSystem::Runtime_t::CPU;
  // static zdl::DlSystem::RuntimeList runtimeList;
  bool usingInitCaching = false;
  bool staticQuantization = false;
  bool useUserSuppliedBuffers = false;

  // m_logger->info("Creating YOLOv8 Model ...");

  // Check if both runtimelist and runtime are passed in
  // if (rumtimeStr == "gpu")
  // {
  //   runtime = zdl::DlSystem::Runtime_t::GPU;
  // }
  // else if (rumtimeStr == "aip")
  // {
  //   runtime = zdl::DlSystem::Runtime_t::AIP_FIXED8_TF;
  // }
  // else if (rumtimeStr == "dsp")
  // {
  //   runtime = zdl::DlSystem::Runtime_t::DSP;
  // }
  // else if (rumtimeStr == "cpu")
  // {
  //   runtime = zdl::DlSystem::Runtime_t::CPU;
  // }
  // else
  // {
  //   m_logger->warn("The runtime option provide is not valid. Defaulting to the CPU runtime.");
  // }

  // if(runtimeList.empty() == false)
  // {
  //   m_logger->error("Invalid option cannot mix runtime order -l with runtime -r ");
  //   std::exit(1);
  // }

  // STEP1: Get Available Runtime
  // m_logger->info("Rumtime = {}", rumtimeStr);
  // runtime = checkRuntime(runtime, staticQuantization);
  // runtimeList.add(runtime);

  // STEP2: Create Deep Learning Container and Load Network File
  // m_logger->info("DLC File Path = {}",  dlcFilePath);
  // std::unique_ptr<zdl::DlContainer::IDlContainer> container = loadContainerFromFile(dlcFilePath);
  // if (container == nullptr)
  // {
  //   m_logger->error("Error while opening the container file.");
  //   std::exit(1);
  // }

  // STEP3: Set Network Builder
  // zdl::DlSystem::PlatformConfig platformConfig;
  // m_snpe = setBuilderOptions(
  //   container, runtimeList, useUserSuppliedBuffers, m_outputTensorList, platformConfig, usingInitCaching);

  // if (m_snpe == nullptr)
  // {
  //   m_logger->error("Error while building SNPE object.");
  //   std::exit(1);
  // }
  // if (usingInitCaching)
  // {
  //   if (container->save(dlcFilePath))
  //   {
  //     m_logger->info("Saved container into archive successfully");
  //   }
  //   else
  //   {
  //     m_logger->warn("Failed to save container into archive");
  //   }
  // }

  // STEP4: Init Model Input/Output Tensor
  printf("[YOLOv8::YOLOv8(Config_S *config)] Start   _initModelIO()\n");
  _initModelIO();
  printf("[YOLOv8::YOLOv8(Config_S *config)] End   _initModelIO()\n");
  // Output Decoder~VisionTracker

};


YoloV8_Class::~YoloV8_Class()  // clear object memory
{


	post_thread_deinit( &live_ctx->thread_ctx, &live_ctx->nn_cvflow);
	nn_cvflow_deinit(&live_ctx->nn_cvflow);
	cv_env_deinit(live_ctx);
	if (live_ctx->f_result > -1) {
		close(live_ctx->f_result);
		live_ctx->f_result = -1;
	}

	delete params;
	delete live_ctx;

	params = nullptr;
	live_ctx = nullptr;
  // close();
};


// void YOLOv8::close()
// {
//   m_snpe.reset();
// }


// ============================================
//               Tensor Settings
// ============================================

bool YoloV8_Class::_initModelIO()
{
  // auto m_logger = spdlog::get("YOLOv8");
  // m_logger->info("[YOLOv8] => Create Model Input Tensor");
  // m_logger->info("-------------------------------------------");
  // m_inputTensorShape = m_snpe->getInputDimensions();
  // m_inputHeight = m_inputTensorShape.getDimensions()[1];
  // m_inputWidth = m_inputTensorShape.getDimensions()[2];
  // m_inputChannel = m_inputTensorShape.getDimensions()[3];
  // m_logger->info("Input H: {}", m_inputHeight);
  // m_logger->info("Input W: {}", m_inputWidth);
  // m_logger->info("Input C: {}", m_inputChannel);

  // Get input names and number
  // const auto& inputTensorNamesRef = m_snpe->getInputTensorNames();
  // if (!inputTensorNamesRef) throw std::runtime_error("Error obtaining Input tensor names");
  // const zdl::DlSystem::StringList& inputTensorNames = *inputTensorNamesRef;  // inputTensorNames refers to m_snpe->getInputTensorNames()'s returned variable

  // Make sure the network requires only a single input
  // assert (inputTensorNames.size() == 1);

  /* Create an input tensor that is correctly sized to hold the input of the network.
    Dimensions that have no fixed size will be represented with a value of 0. */
  // const auto &inputDims_opt = m_snpe->getInputDimensions(inputTensorNames.at(0));
  // const auto &inputShape = *inputDims_opt;  // 384 * 640

  /* Calculate the total number of elements that can be stored in the tensor
    so that we can check that the input contains the expected number of elements.
    With the input dimensions computed create a tensor to convey the input into the network. */
  // m_inputTensor = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);



  // m_logger->info("Create Model Output Buffers");
  // m_logger->info("-------------------------------------------");



  return true;
}


// bool YOLOv8::_getITensor(float *yoloOutputBuff, const zdl::DlSystem::ITensor* tensor)
// {
//   int batchChunk = tensor->getSize();

//   std::memcpy(
//     yoloOutputBuff,
//     &tensor->cbegin()[0],
//     batchChunk * sizeof(float));

//   return true;
// }


bool YoloV8_Class::_getOutputTensor()
{
  // auto m_logger = spdlog::get("YOLOv8");
  auto time_0 = std::chrono::high_resolution_clock::now();

  // zdl::DlSystem::StringList tensorNames = m_outputTensorMap.getTensorNames();

  // for (auto it=tensorNames.begin(); it!=tensorNames.end(); it++)
  //   cout << (*it) << endl;
  // auto detectionBox_tensorPtr = m_outputTensorMap.getTensor(m_outputTensorList[0].c_str());
  // auto detectionConf_tensorPtr = m_outputTensorMap.getTensor(m_outputTensorList[1].c_str());
  // auto detectionCls_tensorPtr = m_outputTensorMap.getTensor(m_outputTensorList[2].c_str());

  // if(!_getITensor(m_detectionBoxBuff, detectionBox_tensorPtr))
  // {
  //   // m_logger->error("Failed to get detection box tensor");
  //   return false;
  // }

  // if(!_getITensor(m_detectionConfBuff, detectionConf_tensorPtr))
  // {
  //   // m_logger->error("Failed to get detection conf tensor");
  //   return false;
  // }

  // if(!_getITensor(m_detectionClsBuff, detectionCls_tensorPtr))
  // {
  //   // m_logger->error("Failed to get detection cls tensor");
  //   return false;
  // }

  auto time_1 = std::chrono::high_resolution_clock::now();
  // m_logger->debug("[Get Output]: \t{} ms", \
    std::chrono::duration_cast<std::chrono::nanoseconds>(time_1 - time_0).count() / (1000.0 * 1000));
  return true;
}
void YoloV8_Class::_rescaleBoundingBox(
    int bbx_num,
    struct v8xyxy *out,
    struct v8xyxy *scaledOut,
    int inputW, int inputH,
    int frameW, int frameH)
{
  float w_ratio = (float)inputW / (float)frameW;
  float h_ratio = (float)inputH / (float)frameH;

  for(int i=0; i<bbx_num; i++)
  {
    scaledOut[i].c = out[i].c;
    scaledOut[i].c_prob = out[i].c_prob;
    scaledOut[i].x1 = (int)((float)out[i].x1 / w_ratio);
    scaledOut[i].y1 = (int)((float)out[i].y1 / h_ratio);
    scaledOut[i].x2 = (int)((float)out[i].x2 / w_ratio);
    scaledOut[i].y2 = (int)((float)out[i].y2 / h_ratio);

    // Expand Bounding Box
    int w = scaledOut[i].x2 - scaledOut[i].x1;
    int h = scaledOut[i].y2 - scaledOut[i].y1;

    int c_x = scaledOut[i].x1 + (int)((float)w / 2.0);
    int c_y = scaledOut[i].y1 + (int)((float)h / 2.0);
    w = w * (m_bboxExpandRatio+0.15);
    h = h * m_bboxExpandRatio;

    scaledOut[i].x1 = c_x - (int)((float)w / 2.0);
    scaledOut[i].y1 = c_y - (int)((float)h / 2.0);
    scaledOut[i].x2 = c_x + (int)((float)w / 2.0);
    scaledOut[i].y2 = c_y + (int)((float)h / 2.0);

    if (scaledOut[i].x1 < 0)
      scaledOut[i].x1 = 0;
    if (scaledOut[i].x2 > frameW-1)
      scaledOut[i].x2 = frameW-1;
    if (scaledOut[i].y1 < 0)
      scaledOut[i].y1 = 0;
    if (scaledOut[i].y2 > frameH-1)
      scaledOut[i].y2 = frameH-1;
  }
}


float YoloV8_Class::_getBboxOverlapRatio(BoundingBox &boxA, BoundingBox &boxB)
{
  int iouX = max(boxA.x1, boxB.x1);
  int iouY = max(boxA.y1, boxB.y1);
  int iouW = min(boxA.x2, boxB.x2) - iouX;
  int iouH = min(boxA.y2, boxB.y2) - iouY;
  iouW = max(iouW, 0);
  iouH = max(iouH, 0);

  if (boxA.getArea() == 0)
    return 0;

  float iouArea = iouW * iouH;
  float ratio = iouArea / (float)boxA.getArea();

  return ratio;
}


void YoloV8_Class::_bboxMerging(BoundingBox &bboxA, BoundingBox &bboxB, int label, BoundingBox &bboxMerge)
{
  int newX1 = 0;
  int newY1 = 0;
  int newX2 = 0;
  int newY2 = 0;

  vector<BoundingBox> tmpBboxList;
  tmpBboxList.push_back(bboxA);
  tmpBboxList.push_back(bboxB);

  std::vector<std::vector<Point>> cornerPointList;
  for (int i=0; i<(int)tmpBboxList.size(); i++)
  {
    cornerPointList.push_back(tmpBboxList[i].getCornerPoint());
  }

  // 0:TL, 1:TR, 2:BL, 3:BR
  vector<int> x1List;
  vector<int> y1List;
  vector<int> x2List;
  vector<int> y2List;
  for (int i=0; i<(int)cornerPointList.size(); i++)
  {
    x1List.push_back(cornerPointList[i][0].x);
    y1List.push_back(cornerPointList[i][0].y);
    x2List.push_back(cornerPointList[i][3].x);
    y2List.push_back(cornerPointList[i][3].y);
  }

  vector<int>::iterator x1It;
  vector<int>::iterator x2It;
  vector<int>::iterator y1It;
  vector<int>::iterator y2It;

  x1It = std::min_element(x1List.begin(), x1List.end());
  x2It = std::max_element(x2List.begin(), x2List.end());

  y1It = std::min_element(y1List.begin(), y1List.end());
  y2It = std::min_element(y2List.begin(), y2List.end());

  // newX1 = int(((*x1It_min) + (*x1It_max)) * 0.5);
  // newX2 = int(((*x2It_min) + (*x2It_max)) * 0.5);
  newX1 = (*x1It);
  newX2 = (*x2It);

  newY1 = (*y1It);
  newY2 = (*y2It);

  bboxMerge.x1 = newX1;
  bboxMerge.y1 = newY1;
  bboxMerge.x2 = newX2;
  bboxMerge.y2 = newY2;
  bboxMerge.label = label;
}

// ============================================
//                  Outputs
// ============================================

bool YoloV8_Class::getVehicleBoundingBox(
                                  vector<BoundingBox> &_outBboxList,
                                  float confidence,
                                  int videoWidth, 
                                  int videoHeight,
                                  BoundingBox &fcwROI)
{
  // auto m_logger = spdlog::get("YOLOv8");

  // Clear previous bounding boxes
  _outBboxList.clear();

  // m_logger->debug("Get vehicle box => m_numBox = {}", m_numBox);

  Point pROI_TL = fcwROI.getCornerPoint()[0];
  Point pROI_TR = fcwROI.getCornerPoint()[1];

  float wRatio = (float)m_inputWidth / (float)videoWidth;
  float hRatio = (float)m_inputHeight / (float)videoHeight;
 
  for(int i=0; i<m_numBox; i++)
  {
    v8xyxy box = m_yoloOut[i];
    if ((box.c == VEHICLE) && (box.c_prob >= confidence))
    {
      // m_logger->debug("Get vehicle box [{}] : ({}, {}, {}, {}, {}, {})", \
      //   i, box.x1, box.y1, box.x2, box.y2, box.c, box.c_prob);
      BoundingBox bbox(box.x1, box.y1, box.x2, box.y2, box.c);
      
      float bboxWidth = (float)bbox.getWidth()/wRatio;
      float bboxHeight = (float)bbox.getHeight()/hRatio;

      // filter out outliers
      if ((bboxWidth > (float)videoWidth*0.8) || (bboxHeight > (float)videoHeight*0.8))
      {
        // m_logger->debug("filter out outliers - (1)");
        // m_logger->debug("bboxWidth > videoWidth*0.8) || (bboxHeight > videoHeight*0.8)");
        continue;
      }
      else if (bboxWidth < 15 || bboxHeight < 10)
      {
        // m_logger->debug("filter out outliers - (2)");
        // m_logger->debug("bboxWidth < 15 || bboxHeight < 10");
        continue;
      }
      else if (bbox.getAspectRatio() < 0.4)
      {
        // m_logger->debug("filter out outliers - (3)");
        // m_logger->debug("bboxA.getAspectRatio() < 0.4");
        continue;
      }

      // filter out vehicles that out of ROI
      // Point cp = bbox.getCenterPoint();
      // if (cp.x/wRatio < pROI_TL.x || cp.x/wRatio > pROI_TR.x)
      // {
      //   m_logger->debug("cp = ({}, {})", cp.x, cp.y);
      //   m_logger->debug("pROI_TL = ({}, {})", pROI_TL.x, pROI_TL.y);
      //   m_logger->debug("pROI_TR = ({}, {})", pROI_TR.x, pROI_TR.y);
      //   m_logger->debug("filter out outliers - (3)");
      //   m_logger->debug("Out of FCW ROI");
      //   continue;
      // }


      bbox.confidence = box.c_prob;
      _outBboxList.push_back(bbox);
    }
    // else if (box.c_prob < confidence)
    // {
    //   m_logger->debug("Get vehicle box [{}] : ({}, {}, {}, {}, {}, {})", \
    //     i, box.x1, box.y1, box.x2, box.y2, box.c, box.c_prob);
    // }
  }

  return true;
}


bool YoloV8_Class::getMotorbikeBoundingBox(
                                  vector<BoundingBox> &_outBboxList,
                                  float confidence,
                                  int videoWidth, 
                                  int videoHeight,
                                  BoundingBox &fcwROI)
{
  // auto m_logger = spdlog::get("YOLOv8");

  // Clear previous bounding boxes
  _outBboxList.clear();

  //
  // m_logger->debug("Get rider box => m_numBox = {}", m_numBox);

  Point pROI_TL = fcwROI.getCornerPoint()[0];
  Point pROI_TR = fcwROI.getCornerPoint()[1];

  float wRatio = (float)m_inputWidth / (float)videoWidth;
  float hRatio = (float)m_inputHeight / (float)videoHeight;

  //
  vector<BoundingBox> tmpBboxList;

  for(int i=0; i<m_numBox; i++)
  {
    v8xyxy box = m_yoloOut[i];
    if ((box.c == MOTORBIKE) && (box.c_prob >= confidence))  // Rider class
    {
      // m_logger->debug("Get rider box [{}] : ({}, {}, {}, {}, {}, {})", \
      //   i, box.x1, box.y1, box.x2, box.y2, box.c, box.c_prob);

      BoundingBox bbox(box.x1, box.y1, box.x2, box.y2, box.c);

      // filter out vehicles that out of ROI
      // Point cp = bbox.getCenterPoint();
      // if (cp.x/wRatio < pROI_TL.x || cp.x/wRatio > pROI_TR.x)
      // {
      //   m_logger->debug("filter out outliers - (1)");
      //   m_logger->debug("Out of FCW ROI");
      //   continue;
      // }

      // TODO: Aspect Ratio

      BoundingBox bboxRider(box.x1, box.y1, box.x2, box.y2, box.c);

      // Merge human box
      for (int j=0; j<m_numBox; j++)
      {
        if (j!=i)
        {
          v8xyxy boxB = m_yoloOut[j];
          BoundingBox bboxB(boxB.x1, boxB.y1, boxB.x2, boxB.y2, boxB.c);

          if (boxB.c == HUMAN)
          {
            int areaB = bboxB.getArea();
            float overlapRatio = _getBboxOverlapRatio(bbox, bboxB);

            if (overlapRatio > 0.1)
            {
              _bboxMerging(bbox, bboxB, 3, bboxRider); //TODO:
            }
          }
        }
      }
      bboxRider.confidence = box.c_prob;
      tmpBboxList.push_back(bboxRider);
    }
  }


  for(int i=0; i<tmpBboxList.size(); i++)
  {
    int areaA = tmpBboxList[i].getArea();

    bool keepThisRider = true;

    for (int j=0; j<tmpBboxList.size(); j++)
    {
      if (j!=i)
      {
        int areaB = tmpBboxList[j].getArea();

        float overlapRatio = _getBboxOverlapRatio(tmpBboxList[i], tmpBboxList[j]);

        if ((overlapRatio > 0.1) && (areaB < areaA))
          keepThisRider = false;
      }
    }

    if (keepThisRider)
      _outBboxList.push_back(tmpBboxList[i]);
  }

  return true;
}


bool YoloV8_Class::getBikeBoundingBox(
                            vector<BoundingBox> &_outBboxList,
                            float confidence,
                            int videoWidth, 
                            int videoHeight,
                            BoundingBox &fcwROI)
{
  // auto m_logger = spdlog::get("YOLOv8");

  // Clear previous bounding boxes
  _outBboxList.clear();

  //
  // m_logger->debug("Get rider box => m_numBox = {}", m_numBox);

  Point pROI_TL = fcwROI.getCornerPoint()[0];
  Point pROI_TR = fcwROI.getCornerPoint()[1];

  float wRatio = (float)m_inputWidth / (float)videoWidth;
  float hRatio = (float)m_inputHeight / (float)videoHeight;

  //
  vector<BoundingBox> tmpBboxList;

  for(int i=0; i<m_numBox; i++)
  {
    v8xyxy box = m_yoloOut[i];
    if ((box.c == MOTORBIKE) && (box.c_prob >= confidence))  // Rider class
    {
      // m_logger->debug("Get rider box [{}] : ({}, {}, {}, {}, {}, {})", \
      //   i, box.x1, box.y1, box.x2, box.y2, box.c, box.c_prob);

      BoundingBox bbox(box.x1, box.y1, box.x2, box.y2, box.c);

      // filter out vehicles that out of ROI
      // Point cp = bbox.getCenterPoint();
      // if (cp.x/wRatio < pROI_TL.x || cp.x/wRatio > pROI_TR.x)
      // {
      //   m_logger->debug("filter out outliers - (1)");
      //   m_logger->debug("Out of FCW ROI");
      //   continue;
      // }

      // TODO: Aspect Ratio

      BoundingBox bboxRider(box.x1, box.y1, box.x2, box.y2, box.c);

      // Merge human box
      for (int j=0; j<m_numBox; j++)
      {
        if (j!=i)
        {
          v8xyxy boxB = m_yoloOut[j];
          BoundingBox bboxB(boxB.x1, boxB.y1, boxB.x2, boxB.y2, boxB.c);

          if (boxB.c == HUMAN)
          {
            int areaB = bboxB.getArea();
            float overlapRatio = _getBboxOverlapRatio(bbox, bboxB);

            if (overlapRatio > 0.1)
            {
              _bboxMerging(bbox, bboxB, 3, bboxRider); //TODO:
            }
          }
        }
      }
      bboxRider.confidence = box.c_prob;
      tmpBboxList.push_back(bboxRider);
    }
  }


  for(int i=0; i<tmpBboxList.size(); i++)
  {
    int areaA = tmpBboxList[i].getArea();

    bool keepThisRider = true;

    for (int j=0; j<tmpBboxList.size(); j++)
    {
      if (j!=i)
      {
        int areaB = tmpBboxList[j].getArea();

        float overlapRatio = _getBboxOverlapRatio(tmpBboxList[i], tmpBboxList[j]);

        if ((overlapRatio > 0.1) && (areaB < areaA))
          keepThisRider = false;
      }
    }

    if (keepThisRider)
      _outBboxList.push_back(tmpBboxList[i]);
  }

  return true;
}


bool YoloV8_Class::getHumanBoundingBox(
                                vector<BoundingBox> &_outBboxList,
                                float confidence,
                                int videoWidth, 
                                int videoHeight,
                                BoundingBox &fcwROI
                               )
{
//   auto m_logger = spdlog::get("YOLOv8");

  // Clear previous bounding boxes
  _outBboxList.clear();

  // m_logger->debug("Get human box => m_numBox = {}", m_numBox);

  Point pROI_TL = fcwROI.getCornerPoint()[0];
  Point pROI_TR = fcwROI.getCornerPoint()[1];

  float wRatio = (float)m_inputWidth / (float)videoWidth;
  float hRatio = (float)m_inputHeight / (float)videoHeight;

  for(int i=0; i<m_numBox; i++)
  {
    v8xyxy box = m_yoloOut[i];
    if ((box.c == HUMAN) && (box.c_prob >= confidence))
    {
      //m_logger->debug("Get human box [{}] : ({}, {}, {}, {}, {}, {})", \
        i, box.x1, box.y1, box.x2, box.y2, box.c, box.c_prob);

      BoundingBox bbox(box.x1, box.y1, box.x2, box.y2, box.c);

      // filter out vehicles that out of ROI
      // Point cp = bbox.getCenterPoint();
      // if (cp.x/wRatio < pROI_TL.x || cp.x/wRatio > pROI_TR.x)
      // {
      //   m_logger->debug("filter out outliers - (1)");
      //   m_logger->debug("Out of FCW ROI");
      //   continue;
      // }

      // Aspect Ratio TODO:

      bbox.confidence = box.c_prob;
      _outBboxList.push_back(bbox);
    }
  }
  return true;
}
// ============================================
//                Load Inputs
// ============================================





// ============================================
//                  Others
// ============================================

void YoloV8_Class::debugON()
{
  m_debugMode = true;
}


void YoloV8_Class::showProcTime()
{
  m_estimateTime = true;
}