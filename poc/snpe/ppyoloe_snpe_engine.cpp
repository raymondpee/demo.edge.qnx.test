#include "ppyoloe_snpe_engine.h"

#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

PPYOLOE_SNPE_ENGINE::PPYOLOE_SNPE_ENGINE() {}

int PPYOLOE_SNPE_ENGINE::init(zdl::DlSystem::Runtime_t runtime_t, const std::string &model_path) {
    // 1.set runtime
    int ret = setRuntime(runtime_t);
    
    if(ret != 0) {
        cout << "setRuntime error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    }

    // 2. load model
    std::unique_ptr<zdl::DlContainer::IDlContainer> container;
    container = zdl::DlContainer::IDlContainer::open(model_path);
    if (container == nullptr) {
        cout << "load model error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    }

    // 3. build engine
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

/*
    zdl::DlSystem::Dimension dims[DIM_SIZE];
    dims[0] = INF_BATCH;
    dims[1] = IMG_WIDTH;
    dims[2] = IMG_HEIGHT;
    dims[3] = IMG_CHANNEL;
    size_t size = DIM_SIZE; // fp32
    zdl::DlSystem::TensorShape tensorShapeImg(dims, size);
    m_input_tensors[0] = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(tensorShapeImg);
    m_input_tensor_map.add(INPUT_IMAGE, m_input_tensors[0]);
    std::cout<<m_engine->getInputDimensions(INPUT_IMAGE)<<std::endl;
    zdl::DlSystem::Dimension scale_dims[1];
    zdl::DlSystem::TensorShape tensorShapeScale(scale_dims, 1);
    m_input_tensors[1] = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(tensorShapeImg);
    m_input_tensor_map.add(INPUT_SCALE, m_input_tensors[1]);

    */
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

        // Ensure that the input tensor dimensions match the expected dimensions ([1, 224, 224, 3])
        // (This part may require customization based on your model's input requirements)
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
        
        // Create a temporary buffer for the input data
        std::vector<float> inputBuffer(inputTensorSize);

        // Copy input data from batchedInputTensor to the buffer
        const float* batchedInputData = batchedInputTensor.ptr<float>();
        std::memcpy(inputBuffer.data(), batchedInputData, inputTensorSize * sizeof(float));

        // Copy data from the buffer to inputTensorData
        float* inputTensorData = inputBuffer.data();
        std::memcpy(inputTensorData, inputBuffer.data(), inputTensorSize * sizeof(float));
    }

    return 0;
}

int PPYOLOE_SNPE_ENGINE::inference(const cv::Mat& input_mat, cv::Mat& output_mat) {
    
    cv::Mat resized_input, rgbImage;
    cv::resize(input_mat, rgbImage, cv::Size(IMG_WIDTH, IMG_HEIGHT));

    // Normalize the pixel values to [0, 1]
    //rgbImage.convertTo(rgbImage, CV_32FC3, 1.0f / 255.0f);

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
        // Print the results
        std::cout<<output_data_size<<std::endl;
        std::cout << "Output Tensor Name: " << outputTensorNames.at(i) << std::endl;
        for (size_t j = 0; j < output_data_size; ++j) {
            //if(outputData[j]>0.5 && outputData[j]!=std::numeric_limits<float>::infinity())
            //std::cout << "Result[" << j << "]: " << outputData[j] << std::endl;
        }
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
        std::vector<std::pair<float, int>> topKValues;
        while (!minHeap.empty()) {
            topKValues.push_back(minHeap.top());
            minHeap.pop();
        }

        // Print the top k values and their indices
        for (int i = k - 1; i >= 0; --i) {
            std::cout << "Value #" << k - i << ": " << topKValues[i].first << " (Index: " << topKValues[i].second << ")" << std::endl;
        }   
    }

    /*
    auto itensor_shape = itensor->getShape();
    auto* dims = itensor_shape.getDimensions();
    size_t dim_count = itensor_shape.rank();
    int output_len = 1;
    for (unsigned int i = 0; i < dim_count; i++) {
        output_len *= dims[i];
    }
    output_mat = cv::Mat(output_len, 1, CV_32FC1);
    float* output_data = (float *)output_mat.data;
    int i = 0;
    for(auto it = itensor->begin(); it!=itensor->end();it++) {
        output_data[i++] = *it;
    }
    */
    return 0;
}


