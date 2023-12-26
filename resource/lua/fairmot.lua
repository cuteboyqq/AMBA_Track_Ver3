--[[
History:
  2022-08-10 - [Qiangqiang Zhang] created file

Copyright (c) 2022 Ambarella International LP

This file and its contents ("Software") are protected by intellectual
property rights including, without limitation, U.S. and/or foreign
copyrights. This Software is also the confidential and proprietary
information of Ambarella International LP and its licensors. You may not use, reproduce,
disclose, distribute, modify, or otherwise prepare derivative works of this
Software or any portion thereof except pursuant to a signed license agreement
or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
In the absence of such an agreement, you agree to promptly notify and return
this Software to Ambarella International LP.

This file includes sample code and is only for internal testing and evaluation.  If you
distribute this sample code (whether in source, object, or binary code form), it will be
without any warranty or indemnity protection from Ambarella International LP or its affiliates.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

--]]

_nn_arm_nms_config_ = {
    input_name = "image",   -- input
    heat_map = "hm",        -- output
    heat_map_max = "hmax",  -- output
    wh = "wh",              -- output
    reg = "reg",            -- output
    id = "id",              -- output
    top_k = 50,
    nms_threshold = 0.7,
    det_threshold = 0.45,
    det_min_area = 0.0038,

    measure_time = 0;             -- for Multi-Object Tracking
    mot_image_width = 1920;
    mot_image_height = 1080;
    embedding_reliable_use_iou = 0;
    embedding_reliable_distance = 0.2;
    embedding_cosine_distance = 0.35;
    overlap_reliable_use_iou = 1;
    overlap_reliable_distance = 0.3;
    overlap_iou_distance = 0.6;
    confirming_use_iou = 1;
    confirming_distance = 0.6;
    duplicate_iou_distance = 0.1;
    max_lost_age = 35;
    fuse_lambda = 0.99;
    use_match_priority = 0;
    merge_match_priority = 0;
    match_priority_thresh = 6;
    delta_time = 0.1;
    use_smooth_feature = 0;
    feature_buffer_size = 20;
    smooth_alpha = 0.5;
    tentative_thresh = 10;
    mot_verbose = 0;

    log_level = 2,               -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose.
}
return _nn_arm_nms_config_
