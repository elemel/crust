#include "config_reader.hpp"

#include "config.hpp"

#include <cstring>
#include <boost/algorithm/string.hpp>

namespace crust {
    ConfigReader::ConfigReader(std::istream *source, Config *target) :
        source_(source),
        target_(target)
    { }
    
    void ConfigReader::read()
    {
        while (std::getline(*source_, line_)) {
            char const *separator = std::strchr(line_.c_str(), '=');
            if (separator) {
                key_.assign(line_.c_str(), separator);
                value_.assign(separator + 1);
                boost::algorithm::trim(key_);
                boost::algorithm::trim(value_);
                readValue();
            }
        }
    }

    void ConfigReader::readValue()
    {
        if (key_ == "fullscreen") {
            target_->fullscreen = parseBool(value_.c_str());
        }
        if (key_ == "multisampling") {
            target_->multisampling = parseBool(value_.c_str());
        }
        if (key_ == "supersampling") {
            target_->supersampling = parseBool(value_.c_str());
        }
        if (key_ == "window_width") {
            target_->windowWidth = parseInt(value_.c_str());
        }
        if (key_ == "window_height") {
            target_->windowHeight = parseInt(value_.c_str());
        }
        if (key_ == "vsync") {
            target_->vsync = parseBool(value_.c_str());
        }
        if (key_ == "camera_zoom") {
            target_->cameraZoom = parseFloat(value_.c_str());
        }
        if (key_ == "camera_scale") {
            target_->cameraScale = parseFloat(value_.c_str());
        }
        if (key_ == "camera_pan") {
            target_->cameraPan = parseFloat(value_.c_str());
        }
        if (key_ == "draw_fps") {
            target_->drawFps = parseBool(value_.c_str());
        }
        if (key_ == "fps") {
            target_->fps = parseInt(value_.c_str());
        }
    }

    bool ConfigReader::parseBool(char const *arg)
    {
        return std::strcmp(arg, "true") == 0;
    }

    int ConfigReader::parseInt(char const *arg)
    {
        return std::atoi(arg);
    }

    float ConfigReader::parseFloat(char const *arg)
    {
        return float(std::atof(arg));
    }
}
