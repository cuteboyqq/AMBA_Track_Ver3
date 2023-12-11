/*******************************************************************************
 * test_yolov8.h
 *
 ******************************************************************************/
#include "yolov8_struct.h"
// #include "yolov8_utils/object.hpp"
// #include "opencv2/core.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/highgui.hpp"
// #include <opencv2/opencv.hpp>

//--------------- Copy include from yolov8.h---------------------------------
#include <chrono>
#include <iostream>
#include <string>

// SNPE SDK
//#include "CheckRuntime.hpp"
//#include "LoadContainer.hpp"
//#include "SetBuilderOptions.hpp"
//#include "DlSystem/DlError.hpp"
//#include "DlSystem/RuntimeList.hpp"
//#include "DlSystem/UserBufferMap.hpp"
//#include "DlSystem/IUserBuffer.hpp"
//#include "DlContainer/IDlContainer.hpp"
//#include "SNPE/SNPE.hpp"
//#include "SNPE/SNPEFactory.hpp"
//#include "DlSystem/ITensorFactory.hpp"
//#include "DlSystem/TensorMap.hpp"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// WNC
#include "point.hpp"
#include "object.hpp"
#include "bounding_box.hpp"
#include "yolov8_decoder.hpp"
#include "img_util.hpp"
#include "utils.hpp"
#include "dla_config.hpp"
#include "logger.hpp"

using namespace std;

#define FILE_MODE 0
#define MAX_YOLO_BBX  100

// --- YOLOv8-nano --- //
#define NUM_BBOX 8400
#define INPUT_WIDTH 640
#define INPUT_HEIGHT 640
#define NUM_DET_CLASSES 4


enum DetectionLabel
{
  HUMAN = 0,
  BIKE = 1,
  VEHICLE = 2,
  MOTORBIKE = 3
};


class YoloV8_Class
{
    public:
        // live_params_t *params;
        // live_ctx_t *live_ctx;
        live_params_t *params;
        live_ctx_t *live_ctx;
        // static void sig_stop(int a)
        // {
        //         (void)a;
        //         live_ctx.sig_flag = 1;
        // };
        YoloV8_Class(Config_S *config);

        YoloV8_Class(Config_S *config,
                        int argc, 
                        char **argv);

        ~YoloV8_Class();


        YoloV8_Class(int argc, 
                        char **argv);
                       
        YoloV8_Class(int argc, 
                        char **argv,
                        live_params_t *params,
                        live_ctx_t *live_ctx);

        int test_yolov8_init(int argc, 
                char **argv, 
                live_params_t *params,
                live_ctx_t *live_ctx);

        int init(int argc, 
                char **argv, 
                live_params_t *params,
                live_ctx_t *live_ctx);



        int init_param(int argc, 
                        char **argv, 
                        live_params_t *params);

        // Alister add 2023-12-11
        int init_param(int argc, 
                        char **argv, 
                        live_params_t *params, 
                        Config_S *config);


        int live_init(live_ctx_t *live_ctx, 
                    live_params_t *params);


        int test_yolov8_run_2(live_ctx_t *live_ctx, 
                        live_params_t *params);

        int test_yolov8_run();


        void yolov8_thread_join();


        cv::Mat Get_img();

        // Object test_yolov8_tracker(live_ctx_t *live_ctx, 
        //                 live_params_t *params);

        std::vector<BoundingBox> Get_yolov8_Bounding_Boxes(live_ctx_t *live_ctx, 
                        live_params_t *params,
                        std::vector<BoundingBox> bboxList);

        int Get_Yolov8_Bounding_Boxes(std::vector<BoundingBox> &bboxList,cv::Mat img);


        int Get_Yolov8_Bounding_Boxes_Ver2(v8xyxy bboxList[MAX_YOLO_BBX], int model_size);


        void Draw_Yolov8_Bounding_Boxes(std::vector<BoundingBox> bboxList,live_ctx_t *live_ctx, live_params_t *params);

        void Draw_Yolov8_Bounding_Boxes(std::vector<BoundingBox> &bboxList, 
                                        int c, 
                                        cv::Mat img);


        int tensor2mat_bgr2bgr(ea_tensor_t *tensor, 
                                        cv::Mat &bgr);


        int tensor2mat_rgb2bgr(ea_tensor_t *tensor,
                                cv::Mat &bgr);

        int tensor2mat_yuv2bgr_nv12(ea_tensor_t *tensor, 
                                        cv::Mat &bgr);

        int live_run_loop(live_ctx_t *live_ctx, 
                        live_params_t *params);

        void live_deinit(live_ctx_t *live_ctx, 
                        live_params_t *params);

        void test_yolov8_deinit(live_ctx_t *live_ctx, 
                        live_params_t *params);

        void test_yolov8_deinit();

        

        // Alister add 2023-11-29
         // Detection
        // Detection
       // Detection
        // I/O
      
        bool getHumanBoundingBox(
        vector<BoundingBox> &_outHumanBboxList,
        float confidenceHuman,
        int videoWidth,
        int videoHeight,
        BoundingBox &fcwROI);

        bool getBikeBoundingBox(
        vector<BoundingBox> &_outRiderBboxList,
        float confidenceRider,
        int videoWidth,
        int videoHeight,
        BoundingBox &fcwROI);

        bool getMotorbikeBoundingBox(
        vector<BoundingBox> &_outRiderBboxList,
        float confidenceRider,
        int videoWidth,
        int videoHeight,
        BoundingBox &fcwROI);

        bool getVehicleBoundingBox(
        vector<BoundingBox> &_outBboxList,
        float confidenceVehicle,
        int videoWidth,
        int videoHeight,
        BoundingBox &fcwROI);
        //BoundingBox &fcwROI);

        //==================Add Tracking======================================================
        int run();

        // Others
        // Debug
        // void getDebugLogs();
        void debugON();
        void showProcTime();
        
        //Alister add 2023-12-07
        bool fileExists(const std::string& path);
        

     private:
        int parse_param(int argc, char **argv, 
                        live_params_t *params);

        int check_params(live_params_t *params);
  
        void live_set_post_thread_params(live_params_t *params,
	                                post_thread_params_t *post_params);
        
        
        int cv_env_init(live_ctx_t *live_ctx, 
                        live_params_t *params);

        
        void cv_env_deinit(live_ctx_t *live_ctx);


        int live_update_net_output(live_ctx_t *live_ctx,
	                            vp_output_t **vp_output);
        
        int live_run_loop_dummy(live_ctx_t *live_ctx, 
                                live_params_t *params);

        int live_convert_yuv_data_to_bgr_data_for_postprocess(live_params_t *params, 
                                                            img_set_t *img_set);

        int live_run_loop_without_dummy(live_ctx_t *live_ctx, 
                                        live_params_t *params);

        
        // ---------------2023-11-29 Alister Copy from yolov8.cpp-----------------------------------------
        
        // Detection
        void _OD_postProcessing(vector<BoundingBox> bboxlist);

        void _OD_postProcessing();


        float _getBboxOverlapRatio(
        BoundingBox &boxA, BoundingBox &boxB);

        void _rescaleBoundingBox(
        int bbx_num,
        struct v8xyxy *out,
        struct v8xyxy *scaledOut,
        int inputW,
        int inputH,
        int frameW,
        int frameH
        );

        void _bboxMerging(
        BoundingBox &bboxA, BoundingBox &bboxB, int label, BoundingBox &bboxMerge);
                
        // I/O
        bool _initModelIO();
        // bool _loadImageFile(const std::string& inputFile);
        
        // bool _getITensor(float* yoloOutput, const zdl::DlSystem::ITensor* tensor);
        bool _getOutputTensor();

        ///////////////////////////
        /// Member Variables
        //////////////////////////

        // Mat
        cv::Mat m_img;
        cv::Mat m_imgResized;

        // Model Related
        // std::unique_ptr<zdl::SNPE::SNPE> m_snpe = nullptr;

        // I/O information

        // Input
        int m_inputChannel = 0;
        int m_inputHeight = 416;
        int m_inputWidth = 416;
        int m_detectionSize = 0;
        int m_detectionBoxSize = 0;
        int m_detectionConfSize = 0;
        int m_detectionClassSize = 0;


        // zdl::DlSystem::TensorShape m_inputTensorShape;
        // std::unique_ptr<zdl::DlSystem::ITensor> m_inputTensor;
        cv::Size inputSize;

        // Output Tensor
        // zdl::DlSystem::TensorMap m_outputTensorMap;
        BoundingBox* m_dummyBox;




        std::vector<std::string> m_outputTensorList = {
        "box",
        "conf",
        "cls"};

        // Inference
        bool m_inference = true;

        // Bounding Box
        float m_bboxExpandRatio = 1.0;
        struct v8xyxy m_yoloOut[MAX_YOLO_BBX];
        struct v8xyxy m_scaledOut[MAX_YOLO_BBX];
        int m_numBox = 0;

        // Threshold
        float confidenceThreshold = 0.2;
        float iouThreshold = 0.4;

        // debug
        bool m_debugMode = false;
        bool m_showMask = false;
        bool m_estimateTime = false;

        
};