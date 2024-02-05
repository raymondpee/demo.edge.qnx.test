#include "snpe.h"

#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

PPYOLOE_SNPE_ENGINE::PPYOLOE_SNPE_ENGINE() {}

int PPYOLOE_SNPE_ENGINE::init(zdl::DlSystem::Runtime_t runtime_t, const std::string &model_path) {
    int ret = setRuntime(runtime_t);
    
    if(ret != 0) {
        cout << "setRuntime error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    }

    std::unique_ptr<zdl::DlContainer::IDlContainer> container;
    container = zdl::DlContainer::IDlContainer::open(model_path);
    if (container == nullptr) {
        cout << "load model error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    }

    zdl::SNPE::SNPEBuilder snpe_builder(container.get());
    m_engine= snpe_builder
            .setOutputLayers({})
            .setRuntimeProcessorOrder(_runtime_list)
            .build();

    if (m_engine== nullptr) {
        cout << "build engine error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    }

    const auto& inputTensorNamesRef =m_engine->getInputTensorNames();
    const auto& inputTensorNames = *inputTensorNamesRef;

    m_inputs.resize(inputTensorNames.size());
    for (int i = 0; i < inputTensorNames.size(); ++i) {
        const auto& inputShape_opt =m_engine->getInputDimensions(inputTensorNames.at(i));
        std::cout<<inputTensorNames.at(i)<<std::endl;
        const auto& inputShape = *inputShape_opt;
        m_inputs[i] = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
        m_input_tensor_map.add(inputTensorNames.at(i), m_inputs[i].get());
    }
    
    cout << "init success..." << endl;
    return 0;
}


PPYOLOE_SNPE_ENGINE::~PPYOLOE_SNPE_ENGINE() {
   m_engine.reset();
    cout << "deinit success..." << endl;
}

int PPYOLOE_SNPE_ENGINE::setRuntime(zdl::DlSystem::Runtime_t runtime_t){
    const char* runtime_string = zdl::DlSystem::RuntimeList::runtimeToString(runtime_t);

    if (!zdl::SNPE::SNPEFactory::isRuntimeAvailable(runtime_t) ||
        ((runtime_t == zdl::DlSystem::Runtime_t::GPU_FLOAT16 || runtime_t == zdl::DlSystem::Runtime_t::GPU_FLOAT32_16_HYBRID)
        && !zdl::SNPE::SNPEFactory::isGLCLInteropSupported())) {
        cout << "SNPE runtime " <<  runtime_string << " not support" << endl;
        return -1;
    }

    cout << "SNPE model init, using runtime " <<  runtime_string << endl;

    _runtime_list.add(runtime_t);
    return 0;
}

 int PPYOLOE_SNPE_ENGINE::setInputData(const cv::Mat& batchedInputTensor) {
    const auto& inputTensorNamesRef = m_engine->getInputTensorNames();
    const auto& inputTensorNames = *inputTensorNamesRef;

    for (int i = 0; i < inputTensorNames.size(); ++i) {
        const auto& inputShape_opt = m_engine->getInputDimensions(inputTensorNames.at(i));
        const auto& inputShape = *inputShape_opt;

        std::vector<int> expectedInputDims = {1, IMG_WIDTH, IMG_HEIGHT, 3};
        if (inputShape.rank() != expectedInputDims.size()) {
            std::cerr << "Input tensor rank does not match the expected dimensions." << std::endl;
            return -1;
        }
        for (size_t j = 0; j < expectedInputDims.size(); ++j) {
            if (inputShape[j] != expectedInputDims[j]) {
                std::cerr << "Input tensor dimensions do not match the expected dimensions." << std::endl;
                return -1;
            }
        }

        zdl::DlSystem::ITensor* inputTensor = m_input_tensor_map.getTensor(inputTensorNames.at(i));
        size_t inputTensorSize = inputTensor->getSize();
        
        std::vector<float> inputBuffer(inputTensorSize);
        const float* batchedInputData = batchedInputTensor.ptr<float>();
        std::memcpy(inputBuffer.data(), batchedInputData, inputTensorSize * sizeof(float));
        float* inputTensorData = inputBuffer.data();
        std::memcpy(inputTensorData, inputBuffer.data(), inputTensorSize * sizeof(float));
    }

    return 0;
}

int PPYOLOE_SNPE_ENGINE::inference(const cv::Mat& input_mat,  std::vector<std::pair<float, int>>& results) {
    
    cv::Mat resized_input, rgbImage;
    cv::resize(input_mat, rgbImage, cv::Size(IMG_WIDTH, IMG_HEIGHT));

    unsigned long int in_size = 1;
    const zdl::DlSystem::TensorShape i_tensor_shape = m_engine->getInputDimensions();
    const zdl::DlSystem::Dimension *shapes = i_tensor_shape.getDimensions();
    size_t img_size = rgbImage.channels() * rgbImage.cols * rgbImage.rows;
    for(int i=1; i<i_tensor_shape.rank(); i++) {
      in_size *= shapes[i];
    }

    if(in_size != img_size) {
        std::cout<<"Input Size mismatch!"<<std::endl;
        std::cout<<"Expected: "<<in_size<<std::endl;
        std::cout<<"Got: "<<img_size<<std::endl;
        exit(0);
    }

    std::unique_ptr<zdl::DlSystem::ITensor> input_tensor = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(m_engine->getInputDimensions());
    zdl::DlSystem::ITensor *tensor_ptr = input_tensor.get();
    if(tensor_ptr == nullptr) {
        std::cerr << "Could not create SNPE input tensor" << std::endl;
        exit(0);
    }

    //Preprocess
    float *tensor_ptr_fl = reinterpret_cast<float *>(&(*input_tensor->begin()));
    for(auto i=0; i<img_size; i++) {
        tensor_ptr_fl[i] = (static_cast<float>(rgbImage.data[i]) - 128.f )/ 128.f;
        //tensor_ptr_fl[i] = 0;
    }

    zdl::DlSystem::TensorMap input_tensor_map;
    input_tensor_map.add("input",tensor_ptr);

    bool ret = m_engine->execute(input_tensor_map, m_output_tensor_map);

    if (!ret) {
        cerr << "Engine inference error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    } else {
        cout << "Engine inference success..." << endl;
    }

    const auto& outputTensorNamesRef = m_engine->getOutputTensorNames();
    const auto& outputTensorNames = *outputTensorNamesRef;

    for (int i = 0; i < outputTensorNames.size(); ++i) {
        zdl::DlSystem::ITensor* outputTensor = m_output_tensor_map.getTensor(outputTensorNames.at(i));
        std::vector<float> outputData { reinterpret_cast<float *>(&(*outputTensor->begin())), reinterpret_cast<float *>(&(*outputTensor->end()))};
        size_t output_data_size = outputTensor->getSize();
        int k = 5; // Number of top elements to find

        // Create a min-heap to store the top k elements and their indices
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> minHeap;

        for (int i = 0; i < outputData.size(); ++i) {
            if (minHeap.size() < k) {
                minHeap.push(std::make_pair(outputData[i], i));
            } else {
                if (outputData[i] > minHeap.top().first) {
                    minHeap.pop();
                    minHeap.push(std::make_pair(outputData[i], i));
                }
            }
        }

        // Retrieve and print the top k values and their indices
        std::cout << "Top " << k << " Values and Indices:" << std::endl;
        while (!minHeap.empty()) {
            results.emplace_back(minHeap.top());
            minHeap.pop();
        }

        // Print the top k values and their indices
        for (int i = k - 1; i >= 0; --i) {
            std::cout << "Value #" << k - i << ": " << results[i].first << " (Index: " << results[i].second << ")" << std::endl;
        }   
    }
    return 0;
}


