#include "color_generator.hpp"

#include "color.hpp"
#include "math.hpp"
#include "random.hpp"

#include <algorithm>
#include <cmath>

namespace crust {
    ColorGenerator::ColorGenerator(Random *random) :
        random_(random)
    { }

    Color3 ColorGenerator::generateColor()
    {
        float redAngle = M_PI / 3.0f;
        float greenAngle = M_PI;
        float blueAngle = 5.0f * M_PI / 3.0f;

        float hue = 0.3f + mix(random_->getFloat() - 0.5f, 0.0f, 0.98f);
        float saturation = mix(random_->getFloat(), 0.7f, 0.8f);
        float lightness = mix(random_->getFloat(), 0.7f, 0.7f);

        float hueAngle = 2.0f * M_PI * hue;

        float hueRed = std::max(0.0f, std::cos(hueAngle - redAngle));
        float hueGreen = std::max(0.0f, std::cos(hueAngle - greenAngle));
        float hueBlue = std::max(0.0f, std::cos(hueAngle - blueAngle));
        
        float red = lightness * mix(1.0f, hueRed, saturation);
        float green = lightness * mix(1.0f, hueGreen, saturation);
        float blue = lightness * mix(1.0f, hueBlue, saturation);

        // red = green = blue = (red + blue + green) / 3.0f;
        
        return Color3(std::min(255, int(red * 256.0)),
                      std::min(255, int(green * 256.0)),
                      std::min(255, int(blue * 256.0)));
    }
}
