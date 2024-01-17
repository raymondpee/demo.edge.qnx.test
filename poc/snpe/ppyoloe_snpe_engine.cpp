#include "ppyoloe_snpe_engine.h"

#include <iostream>

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

int PPYOLOE_SNPE_ENGINE::inference(const cv::Mat& input_mat, cv::Mat& output_mat) {
    
    cv::Mat resized_input, rgbImage;
    cv::resize(input_mat, resized_input, cv::Size(IMG_WIDTH, IMG_HEIGHT));
    cv::cvtColor(resized_input, rgbImage, cv::COLOR_BGR2RGB);
    auto tensor_img = m_input_tensor_map.getTensor("image");
    int mem_size = resized_input.rows * resized_input.cols * resized_input.channels();
    float* src = (float*) resized_input.data;
    std::copy(src, src + mem_size, tensor_img->begin()); 
    auto tensor_scale = m_input_tensor_map.getTensor("scale_factor");
    tensor_scale->getShape()[0] = 1.0;

    bool ret =m_engine->execute(m_input_tensor_map, m_output_tensor_map);
    if (!ret) {
        cerr << "engine inference error : " << zdl::DlSystem::getLastErrorString() << endl;
        return -1;
    } else {
        cout << "engine inference success..." << endl;
    }

    const auto& outputTensorNamesRef = m_engine->getOutputTensorNames();
    const auto& outputTensorNames = *outputTensorNamesRef;

    for (int i = 0; i < outputTensorNames.size(); ++i) {
        zdl::DlSystem::ITensor* outputTensor = m_output_tensor_map.getTensor(outputTensorNames.at(i));
        auto outputData = outputTensor->begin();
        size_t output_data_size = outputTensor->getSize();
        // Print the results
        std::cout << "Output Tensor Name: " << outputTensorNames.at(i) << std::endl;
        for (size_t j = 0; j < output_data_size; ++j) {
            if(outputData[j]>0)
                std::cout << "Result[" << j << "]: " << outputData[j] << std::endl;
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


