#include <iostream>
#include <string>
#include <curl/curl.h>

class HttpClient {
public:
    HttpClient() {
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
    }

    ~HttpClient() {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    std::string get(const std::string& url) {
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            return response;
        } else {
            std::cerr << "Failed to initialize libcurl." << std::endl;
            return "";
        }
    }

    std::string post(const std::string& url, const std::string& data) {
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            return response;
        } else {
            std::cerr << "Failed to initialize libcurl." << std::endl;
            return "";
        }
    }

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    CURL* curl;
    std::string response;
};

int main() {
    HttpClient httpClient;

    // GET example
    std::string getResponse = httpClient.get("http://192.168.2.1:8000/");
    std::cout << "GET Response:\n" << getResponse << std::endl;

    // POST example
    //std::string postData = "title=foo&body=bar&userId=1";
    //std::string postResponse = httpClient.post("https://jsonplaceholder.typicode.com/posts", postData);
    //std::cout << "POST Response:\n" << postResponse << std::endl;

    return 0;
}
