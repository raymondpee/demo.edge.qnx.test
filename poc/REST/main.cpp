#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <vector>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

// Define a structure to represent an item
struct Item {
    int id;
    std::wstring name;
};

// Initialize a vector to store items
std::vector<Item> items;

// Function to handle GET requests to retrieve all items
void handle_get_request(http_request request) {
    json::value response;
    
    for (const auto& item : items) {
        json::value item_json;
        item_json[U("id")] = json::value::number(item.id);
        item_json[U("name")] = json::value::string(item.name);
        response[U("items")].as_array().push_back(item_json);
    }
    
    request.reply(status_codes::OK, response);
}

// Function to handle POST requests to add a new item
void handle_post_request(http_request request) {
    request.extract_json()
        .then([&request](json::value body) {
            if (!body.has_field(U("name"))) {
                request.reply(status_codes::BadRequest, U("Invalid request body"));
                return;
            }

            Item new_item;
            new_item.id = items.size() + 1;
            new_item.name = body[U("name")].as_string();
            items.push_back(new_item);

            request.reply(status_codes::Created);
        })
        .wait();
}

int main() {
    http_listener listener(L"http://localhost:8080/api");

    listener.support(methods::GET, handle_get_request);
    listener.support(methods::POST, handle_post_request);

    try {
        listener.open().wait();

        std::wcout << L"Listening for requests at: " << listener.uri().to_string() << std::endl;

        // Keep the program running
        while (true);
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error: " << e.what() << std::endl;
    }

    return 0;
}
