#include "opencv2/opencv.hpp"
#include <fstream>
#include <vector>
#include "snpe.h"

#define STR_SIZE 256
#define ARG_SIZE 4

using namespace std;

void removeChars(std::string& str, const std::string& chars) {
    for (char c : chars) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }
}

int read_label(std::string& filename, std::vector<std::string>& labels)
{
    // Open the file for reading
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return -1;
    }

    // Define a vector to store the lines
    std::vector<std::string> lines;

    // Read lines from the file
    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string value = line.substr(colonPos + 1); // Extract everything after the colon
            labels.push_back(value);
        }
    }

    // Close the file
    file.close();
    return 0;
}

cv::Mat normalize(cv::Mat& mat_src) {
    cv::Mat mat_src_float;
    mat_src.convertTo(mat_src_float, CV_32FC3);

    cv::Mat mat_mean;
    cv::Mat mat_stddev;
    cv::meanStdDev(mat_src_float, mat_mean, mat_stddev);
    cv::Mat mat_dst;

    if (mat_src.channels() == 1) {
        auto m = *((double*)mat_mean.data);
        auto s = *((double*)mat_stddev.data);
        mat_dst = (mat_src_float - m) / (s + 1e-6);
    } else {
        std::vector<cv::Mat> mats;
        cv::split(mat_src_float, mats);
        int c = 0;
        for (auto& mat : mats) {
            auto m = ((double *)mat_mean.data)[c];
            auto s = ((double *)mat_stddev.data)[c];
            mat = (mat - m) / (s + 1e-6);
            c++;
        }
        cv::merge(mats, mat_dst);
    }
    return mat_dst;
}

int main(int argc, char** argv)
{
    if (argc < ARG_SIZE) {
        std::cout << "parameters required: <infer_type> <model_name> <label_path> <img_path>" << std::endl;
        return -1;
    }

    string infer_type = argv[1];
    cout << "infer_type:" << infer_type << endl;
    string model_name = argv[2];
    cout << "model_name:" << model_name << endl;
    string label_path = argv[3];
    cout << "model label:" << label_path << endl;
    string img_path = argv[4];
    cout << "img_path path:" << img_path << endl;

    std::vector<std::string> class_names;
    if(read_label(label_path,class_names)!=0)
    {
        return 0;    
    }

    int runtime = std::stoi(infer_type);
    zdl::DlSystem::Runtime_t runtime_t;
    switch (runtime) {
        case 0:
            runtime_t = zdl::DlSystem::Runtime_t::CPU_FLOAT32;
        case 1:
            runtime_t = zdl::DlSystem::Runtime_t::GPU_FLOAT32_16_HYBRID;
        case 2:
            runtime_t = zdl::DlSystem::Runtime_t::DSP_FIXED8_TF;
        case 3:
            runtime_t = zdl::DlSystem::Runtime_t::GPU_FLOAT16;
        case 5:
            runtime_t = zdl::DlSystem::Runtime_t::AIP_FIXED8_TF;
        default:
            runtime_t = zdl::DlSystem::Runtime_t::CPU_FLOAT32;
    }

    PPYOLOE_SNPE_ENGINE* engine = new PPYOLOE_SNPE_ENGINE();
    engine->init(runtime_t, model_name);

    cv::Mat mat = cv::imread(img_path);
    int width = mat.cols;
    int height = mat.rows;

    std::vector<std::pair<float, int>> results;
    int ret = engine->inference(mat,results);
    if (ret) {
        delete engine;
        cerr << "inference failed!!!" << endl;
        return ret;
    }

    if (!results.empty()) {
        float maxFirstElement = results[0].first;
        int index = 0;

        for (const auto& pair : results) {
            if (pair.first > maxFirstElement) {
                maxFirstElement = pair.first;
                index = pair.second;
            }
        }
        std::cout<<"[RESULT] The detection result is: "<<class_names[index]<<std::endl;
    }
    
    delete engine;
}