// 370Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "httplib.h"
#include "json.hpp"
#include <iostream>

// Shorthand so we can write "json" instead of "nlohmann::json".
using json = nlohmann::json;

int main()
{
    httplib::Server svr;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // Preflight requests
    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
        });

    // GET endpoint
    svr.Get("/api/hello", [](const httplib::Request&, httplib::Response& res) {
        json out = { {"message", "Hello from C++"} };
        res.set_content(out.dump(), "application/json");
        });

    // POST endpoint with JSON body
    svr.Post("/api/add", [](const httplib::Request& req, httplib::Response& res) {
        try {
            // Parse the raw request body string into a JSON object.
                 // Throws json::parse_error if the body is not valid JSON.
            json in = json::parse(req.body);

            // .at("key") throws if the key is missing.
            // .get<int>() throws if the value is not a number.
            int a = in.at("a").get<int>();
            int b = in.at("b").get<int>();

            // ---- Replace this line with a call into YOUR logic ----
            int sum = a + b;

            // Send the result back as JSON.
            res.set_content(json{ {"sum", sum} }.dump(), "application/json");
        }
        catch (const std::exception& e) {
            // Any parsing or validation failure becomes a 400 response with an
            // error message instead of crashing the handler.
            res.status = 400;
            res.set_content(json{ {"error", e.what()} }.dump(), "application/json");
        }
        });

    // Optionally serve your frontend files from the same server (avoids CORS entirely)
    svr.set_mount_point("/", "./public");


    std::cout << "Listening on http://localhost:8080...\n";
    if (!svr.listen("localhost", 8080)) {
        std::cerr << "Failed to bind to port 8080 (is something else using it?)\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
