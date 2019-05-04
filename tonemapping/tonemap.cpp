#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cassert>
#include "vec3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "camera.h"

float luminance(vec3);
vec3 change_luminance(vec3 c_in, float l_out);
float clamp(float, float, float);
vec3 clamp(vec3, float, float);
float lerp(float, float, float);

float gamma_correct(float f)
{
    if (f <= 0.0031308f) {
        return f * 12.92f;
    } else {
        return 1.055f * std::pow(f, 1.0f / 2.4f) - 0.055f;
    }
}

vec3 clamp(vec3 v, float min, float max)
{
    return vec3(
        clamp(v.r(), min, max),
        clamp(v.g(), min, max),
        clamp(v.b(), min, max)
    );
}

vec3 reinhard(vec3 v)
{
    return v / (1.0f + v);
}

vec3 reinhard_extended(vec3 v, float max_white)
{
    vec3 numerator = v * (1.0f + (v / vec3(max_white * max_white)));
    return numerator / (1.0f + v);
}

vec3 reinhard_extended_luminance(vec3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numerator = l_old * (1.0f + (l_old / (max_white_l * max_white_l)));
    float l_new = numerator / (1.0f + l_old);
    return change_luminance(v, l_new);
}

vec3 uncharted2_tonemap_partial(vec3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 uncharted2_filmic(vec3 v)
{
    float exposure_bias = 2.0f;
    vec3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    vec3 W = vec3(11.2f);
    vec3 white_scale = vec3(1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

static const std::array<vec3, 3> aces_input_matrix =
{
    vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)
};

static const std::array<vec3, 3> aces_output_matrix =
{
    vec3( 1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f,  1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f,  1.07602f)
};

vec3 mul(const std::array<vec3, 3>& m, const vec3& v)
{
    float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[1] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[1] + m[2][1] * v[1] + m[2][2] * v[2];
    return vec3(x, y, z);
}

vec3 rtt_and_odt_fit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 aces_fitted(vec3 v)
{
    v = mul(aces_input_matrix, v);
    v = rtt_and_odt_fit(v);
    v = mul(aces_output_matrix, v);
    return v;
}

vec3 aces_approx(vec3 v)
{
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

float camera_get_intensity(float f, float iso)
{
    f = clamp(f, 0.0f, iso); // Clamp to [0, iso]
    f /= iso; // Convert to [0, 1]

    // Returns 1.0 if the index is out-of-bounds
    auto get_or_one = [](const auto& arr, size_t index)
    {
        return index < arr.size() ? arr[index] : 1.0;
    };

    // std::upper_bound uses a binary search to find the position of f in camera_irradiance
    auto upper = std::upper_bound(camera_irradiance.begin(), camera_irradiance.end(), f);
    size_t idx = std::distance(camera_irradiance.begin(), upper);

    double low_irradiance = camera_irradiance[idx];
    double high_irradiance = get_or_one(camera_irradiance, idx + 1);
    double lerp_param = (f - low_irradiance) / (high_irradiance - low_irradiance);

    double low_val = camera_intensity[idx];
    double high_val = get_or_one(camera_intensity, idx + 1);

    // LERPing isn't really necessary for RGB8 (as the curve is sampled with 1024 points)
    return clamp(lerp((float)low_val, (float)high_val, (float)lerp_param), 0.0f, 1.0f);
}

vec3 camera_tonemap(vec3 v, float iso)
{
    float r = camera_get_intensity(v.r(), iso);
    float g = camera_get_intensity(v.g(), iso);
    float b = camera_get_intensity(v.b(), iso);
    return vec3(r, g, b);
}

float luminance(vec3 v)
{
    return dot(v, vec3(0.2126f, 0.7152f, 0.0722f));
}

vec3 change_luminance(vec3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

float clamp(float f, float low, float high)
{
    return std::max(std::min(f, high), low);
}

float lerp(float a, float b, float t)
{
    assert(t <= 1.0);
    return a * (1.0f - t) + b * t;
}

uint8_t float_to_byte(float f)
{
    f = gamma_correct(f);
    return uint8_t(std::min(std::max(f, 0.0f), 1.0f) * 255.99f);
}

vec3 tonemap(vec3 v)
{
    // return clamp(v, 0.0f, 1.0f);
    // return reinhard(v);
    // return reinhard_extended(v, 764.0f);
    // return reinhard_extended_luminance(v, 622.0f);
    // return uncharted2_filmic(v);
    // return aces_fitted(v * 1.8f);
    // return aces_approx(v * 1.8f);
    return camera_tonemap(v, 6.0f);
}

int main()
{
    int x, y, n;
    float *data = stbi_loadf("memorial.hdr", &x, &y, &n, 0);
    assert(data && n == 3);

    std::cout << "width: " << x << ", height: " << y << std::endl;

    std::vector<uint8_t> out;
    out.reserve(x * y * 3);

    float max_luminance = 0.0, max_component = 0.0;

    for (float *f = data; f < data + (x * y * 3); f += 3) {
        vec3 v_in(f[0], f[1], f[2]);
        vec3 v_out = tonemap(v_in);
        out.push_back(float_to_byte(v_out.r()));
        out.push_back(float_to_byte(v_out.g()));
        out.push_back(float_to_byte(v_out.b()));

        max_luminance = std::max(max_luminance, luminance(v_in));
        max_component = std::max(max_component, std::max(v_in.r(), std::max(v_in.g(), v_in.b())));
    }

    std::cout << "tonemapped " << out.size() / 3 << " pixels\n";
    std::cout << "max luminance: " << max_luminance << std::endl;
    std::cout << "max component: " << max_component << std::endl;

    if (!stbi_write_png("./out.png", x, y, 3, out.data(), x * 3)) {
        std::cerr << "error writing png file";
    }

    stbi_image_free(data);
}