#ifndef VISION_SNPE_PREDICTOR_H
#define VISION_SNPE_PREDICTOR_H

#include "DlContainer/IDlContainer.hpp"
#include "DlSystem/DlEnums.hpp"
#include "DlSystem/DlError.hpp"
#include "DlSystem/ITensorFactory.hpp"
#include "DlSystem/RuntimeList.hpp"
#include "SNPE/SNPEBuilder.hpp"
#include "SNPE/SNPEFactory.hpp"
#include "SNPE/SNPE.hpp"

#include "opencv2/opencv.hpp"

#define INF_BATCH 1
#define IMG_WIDTH 224
#define IMG_HEIGHT IMG_WIDTH
#define IMG_CHANNEL 3
#define DIM_SIZE 4
#define NUM_INPUT 2
#define INPUT_IMAGE "image"
#define INPUT_SCALE "scale_factor"

class PPYOLOE_SNPE_ENGINE {
public:
    PPYOLOE_SNPE_ENGINE();
    ~PPYOLOE_SNPE_ENGINE();

    int init(zdl::DlSystem::Runtime_t runtime_t, const std::string &model_path);
    int inference(const cv::Mat& input_mat,  std::vector<std::pair<float, int>>& results);

private:
    int setRuntime(zdl::DlSystem::Runtime_t runtime_t);
    int setInputData(const cv::Mat& batchedInputTensor);
    void build_tensor(const cv::Mat& mat);

    // snpe model
    std::vector<std::unique_ptr<zdl::DlSystem::ITensor>> m_inputs;
    std::unique_ptr<zdl::SNPE::SNPE> m_engine;
    std::unique_ptr<zdl::DlContainer::IDlContainer> m_container;
    zdl::DlSystem::TensorMap m_input_tensor_map;
    zdl::DlSystem::TensorMap m_output_tensor_map;

    // snpe builder config
    // _runtime_list : runtime order list
    zdl::DlSystem::RuntimeList _runtime_list;
};

#endif //VISION_SNPE_PREDICTOR_H
