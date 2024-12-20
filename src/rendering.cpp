
#include <druck/window.hpp>
#include <druck/logging.hpp>
#include <cstring>
#include <string>
#include <iostream>
#include <cassert>


namespace druck::rendering {

    using namespace druck::math;
    namespace logging = druck::logging;
    

    Surface::Surface(int width, int height) {
        if(width <= 0) {
            logging::error(
                std::string("Surface width must be larger than 0 (given was ") 
                    + std::to_string(width) + ")"
            );
        }
        if(height <= 0) {
            logging::error(
                std::string("Surface height must be larger than 0 (given was ") 
                    + std::to_string(height) + ")"
            );
        }
        this->width = width;
        this->height = height;
        this->color = new Color[width * height];
        this->depth = new float[width * height];
        this->clear();
    }

    Surface::Surface(
        const Color* color, const float* depth, int width, int height
    ) {
        if(width <= 0) {
            logging::error(
                std::string("Surface width must be larger than 0 (given was ") 
                    + std::to_string(width) + ")"
            );
        }
        if(height <= 0) {
            logging::error(
                std::string("Surface height must be larger than 0 (given was ") 
                    + std::to_string(height) + ")"
            );
        }
        assert(color != nullptr);
        this->width = width;
        this->height = height;
        this->color = new Color[width * height];
        if(depth == nullptr) {
            this->depth = nullptr;
        } else {
            this->depth = new float[width * height];
        }
        for(int y = 0; y < height; y += 1) {
            for(int x = 0; x < width; x += 1) {
                int offset = y * width + x;
                this->color[offset] = color[offset];
                if(depth != nullptr) {
                    this->depth[offset] = depth[offset];
                }
            }
        }
    }

    Surface::Surface(Surface&& other) {
        this->color = other.color;
        this->depth = other.depth;
        this->width = other.width;
        this->height = other.height;
        other.color = nullptr;
        other.depth = nullptr;
        other.width = 0;
        other.height = 0;
    }

    Surface& Surface::operator=(Surface&& other) noexcept {
        if(this == &other) { return *this; }
        delete[] this->color;
        if(this->depth != nullptr) {
            delete[] this->depth;
        }
        this->color = other.color;
        this->depth = other.depth;
        this->width = other.width;
        this->height = other.height;
        other.color = nullptr;
        other.depth = nullptr;
        other.width = 0;
        other.height = 0;
        return *this;
    }

    Surface::~Surface() {
        if(this->color != nullptr) {
            delete[] this->color;
            this->color = nullptr;
        }
        if(this->depth != nullptr) {
            delete[] this->depth;
            this->depth = nullptr;
        }
    }

    bool Surface::contains(int x, int y) const {
        return x >= 0 && x < this->width
            && y >= 0 && y < this->height;
    }

    bool Surface::contains(const Vec<2>& pixel) const {
        return this->contains(pixel.x(), pixel.y());
    }

    Color Surface::get_color_at(int x, int y) const {
        if(!this->contains(x, y)) { return BLACK; }
        return this->color[y * this->width + x];
    }

    void Surface::set_color_at(int x, int y, Color c) {
        if(!this->contains(x, y)) { return; }
        this->color[y * this->width + x] = c;
    }

    double Surface::get_depth_at(int x, int y) const {
        if(this->depth == nullptr || !this->contains(x, y)) { return INFINITY; }
        return this->depth[y * this->width + x];
    }

    void Surface::set_depth_at(int x, int y, double d) {
        if(this->depth == nullptr || !this->contains(x, y)) { return; }
        this->depth[y * this->width + x] = d;
    }

    Vec<4> Surface::sample(const Vec<2>& uv) const {
        // normalise uv coordinates
        double u = fmod(uv.x(), 1.0); // u=0 -> left, u=1 -> right
        if(u < 0.0) { u += 1.0; }
        double v = fmod(uv.y(), 1.0); // v=0 -> bottom, v=1 -> top
        if(v < 0.0) { v += 1.0; }
        // convert to pixels (note that y needs to be flipped)
        int x_px = static_cast<int>(u * this->width);
        if(x_px >= this->width) { x_px = this->width - 1; }
        int y_px = this->height - static_cast<int>(v * this->height);
        if(y_px >= this->height) { y_px = this->height - 1; }
        // read the color and return as normalized vector
        Color color = this->color[y_px * this->width + x_px];
        return Vec<4>(
            color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255
        );
    }

    void Surface::resize(int width, int height) {
        if(width == this->width && height == this->height) { return; }
        if(width <= 0) {
            logging::error(
                std::string("Surface width must be larger than 0 (given was ") 
                    + std::to_string(width) + ")"
            );
        }
        if(height <= 0) {
            logging::error(
                std::string("Surface height must be larger than 0 (given was ") 
                    + std::to_string(height) + ")"
            );
        }
        this->width = width;
        this->height = height;
        if(this->color != nullptr) {
            delete[] this->color;
        }
        this->color = new Color[width * height];
        if(this->depth != nullptr) {
            delete[] this->depth;
            this->depth = new float[width * height];
        }
        this->clear();
    }

    void Surface::resize(const Vec<2>& size) {
        this->resize(size.x(), size.y());
    }

    void Surface::clear() {
        for(int i = 0; i < this->width * this->height; i += 1) {
            this->color[i] = BLACK;
        }
        if(this->depth == nullptr) { return; }
        for(int i = 0; i < this->width * this->height; i += 1) {
            this->depth[i] = INFINITY;
        }
    }

    void Surface::blit_buffer(
        const Surface& src, 
        int dest_pos_x, int dest_pos_y,
        int dest_width, int dest_height
    ) {
        int dest_end_x = dest_pos_x + dest_width;
        int dest_end_y = dest_pos_y + dest_height;
        for(int dest_x = dest_pos_x; dest_x < dest_end_x; dest_x += 1) {
            for(int dest_y = dest_pos_y; dest_y < dest_end_y; dest_y += 1) {
                float perc_x = (float) (dest_x - dest_pos_x) / dest_width;
                float perc_y = (float) (dest_y - dest_pos_y) / dest_height;
                int src_x = (int) (perc_x * src.width);
                int src_y = (int) (perc_y * src.height);
                Color pixel = src.get_color_at(src_x, src_y);
                this->set_color_at(dest_x, dest_y, pixel);
            }
        }
    }

}
