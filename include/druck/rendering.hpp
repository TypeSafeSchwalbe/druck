
#pragma once

#include <raylib.h>
#include <vector>
#include <tuple>
#include <cstdint>
#include "math.hpp"

#include <cassert>


namespace druck::rendering {

    using namespace druck::math;
    
    
    static double triangle_area(Vec<2> a, Vec<2> b, Vec<2> c) {
        return 0.5 * fabs(
            a.x() * (b.y() - c.y())
                + b.x() * (c.y() - a.y())
                + c.x() * (a.y() - b.y())
        );
    }

    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        static Color from_floats(const Vec<4>& color) {
            return {
                static_cast<uint8_t>(color.r() * 255.0),
                static_cast<uint8_t>(color.g() * 255.0),
                static_cast<uint8_t>(color.b() * 255.0),
                static_cast<uint8_t>(color.a() * 255.0)
            };
        }
    };

    template<typename V>
    struct Mesh {
        std::vector<V> vertices;
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> elements;

        Mesh() {}
        
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&& other) {
            this->vertices = std::move(other.vertices);
            this->elements = std::move(other.elements);
        }

        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) noexcept {
            this->vertices = std::move(other.vertices);
            this->elements = std::move(other.elements);
            return *this;
        }

        uint32_t add_vertex(V vertex) {
            uint32_t idx = this->vertices.size();
            this->vertices.push_back(vertex);
            return idx;
        }

        void add_element(uint32_t a, uint32_t b, uint32_t c) {
            this->elements.push_back(std::make_tuple(a, b, c));
        }
    };

    template<typename V, typename S>
    struct VertexStates {
        S a_state; // shader after running 'vertex' for A
        double a_idepth; // inverse of depth of A
        double a_bc; // barycentric coordinate for A
        S b_state; // shader after running 'vertex' for B
        double b_idepth ; // inverse of depth of B
        double b_bc; // barycentric coordinate for B
        S c_state; // shader after running 'vertex' for C
        double c_idepth; // inverse of depth of C
        double c_bc; // barycentric coordinate for C
        double depth; // current depth
    };

    template<typename V, typename S>
    struct Shader {
        virtual Vec<4> vertex(V vertex) = 0;
        virtual Vec<4> fragment() = 0;

        private:
        const VertexStates<V, S>* vertex_states;

        public:
        void set_vertex_states(const VertexStates<V, S>* states) {
            this->vertex_states = states;
        }

        void clear_vertex_states() {
            this->vertex_states = nullptr;
        }

        private:
        int64_t verify_property_pointer(void* p) {
            if(this->vertex_states == nullptr) {
                std::cout << "'interpolate' can only be called from 'fragment'!" 
                    << std::endl;
                std::abort();
            }
            int64_t offset = (char*) p - (char*) this;
            if(offset < 0 || (size_t) offset > sizeof(S)) {
                std::cout << "Interpolated value must be a shader property!" 
                    << std::endl;
                std::abort();
            }
            return offset;
        }

        public:
        template<typename T>
        void interpolate(T* property) {
            int64_t offset = this->verify_property_pointer(property);
            const VertexStates<V, S>* vs = this->vertex_states;
            // get values for all three vertices
            const T* a = (const T*) ((char*) &vs->a_state + offset);
            const T* b = (const T*) ((char*) &vs->b_state + offset);
            const T* c = (const T*) ((char*) &vs->c_state + offset);
            // interpolate them using the barycentric coordinates
            *property = (
                (*a * vs->a_idepth * vs->a_bc) + 
                (*b * vs->b_idepth * vs->b_bc) + 
                (*c * vs->c_idepth * vs->c_bc)
            ) * vs->depth;
        }

        template<typename T>
        void flat(T* property) {
            int64_t offset = this->verify_property_pointer(property);
            const VertexStates<V, S>* vs = this->vertex_states;
            const T* value = (const T*) ((char*) &vs->a_state + offset);
            *property = *value;
        }
    };

    struct Surface {
        int width;
        int height;
        Color* color;
        float* depth;

        Surface(int width, int height);
        Surface(const Color* color, const float* depth, int width, int height);
        Surface(const Surface&) = delete;
        Surface(Surface&&);
        Surface& operator=(const Surface& other) = delete;
        Surface& operator=(Surface&& other) noexcept;
        ~Surface();

        bool contains(int x, int y) const;
        bool contains(const Vec<2>& pixel) const;
        Color get_color_at(int x, int y) const;
        void set_color_at(int x, int y, Color c);
        double get_depth_at(int x, int y) const;
        void set_depth_at(int x, int y, double d);
        Vec<4> sample(const Vec<2>& uv) const;

        void resize(int width, int height);
        void resize(const Vec<2>& size);
        void clear();

        void blit_buffer(
            const Surface& src, 
            int dest_pos_x, int dest_pos_y,
            int dest_width, int dest_height
        );

        private: 
        template<typename V, typename S>
        void render_triangle_segment(
            Vec<3> a, Vec<3> b, Vec<3> c, double t_area,
            Vec<2> s_high, // top vertex of the segment
            Vec<2> s_low, // bottom vertex of the segment
            VertexStates<V, S>* vs,
            S& shader
        ) {
            Vec<2> t_high = a.swizzle<2>("xy"); // top vertex of the triangle
            Vec<2> t_low = c.swizzle<2>("xy"); // bottom vertex of the triangle
            Vec<2> s_line = s_low - s_high; // vector from top to bottom of segment
            Vec<2> t_line = t_low - t_high; // vector from top to bottom of triangle
            for(int y = std::max((int) s_high.y() + 1, 0); y < s_low.y(); y += 1) {
                if(y > this->height) { break; }
                double s_progress = (y - s_high.y()) / s_line.y();
                Vec<2> r_point = s_line * s_progress + s_high;
                double t_progress = (y - t_high.y()) / t_line.y();
                Vec<2> l_point = t_line * t_progress + t_high;
                if(l_point.x() > r_point.x()) { std::swap(l_point, r_point); }
                for(int x = std::max((int) l_point.x() + 1, 0); x <= r_point.x(); x += 1) {
                    if(x > this->width) { break; }
                    Vec<2> p = Vec<2>(x, y);
                    vs->a_bc = triangle_area(p, b.swizzle<2>("xy"), c.swizzle<2>("xy")) / t_area;
                    vs->b_bc = triangle_area(p, c.swizzle<2>("xy"), a.swizzle<2>("xy")) / t_area;
                    vs->c_bc = triangle_area(p, a.swizzle<2>("xy"), b.swizzle<2>("xy")) / t_area;
                    double px_idepth = vs->a_bc * vs->a_idepth
                        + vs->b_bc * vs->b_idepth
                        + vs->c_bc * vs->c_idepth;
                    if(px_idepth == 0.0) { continue; }
                    vs->depth = 1.0 / px_idepth;
                    if(vs->depth <= 0.0) { continue; }
                    if(vs->depth >= this->get_depth_at(x, y)) { continue; }
                    Color color = Color::from_floats(shader.fragment());
                    this->set_color_at(x, y, color);
                    this->set_depth_at(x, y, vs->depth);
                }
            }
        }

        template<typename V, typename S>
        void draw_triangle(V vertex_a, V vertex_b, V vertex_c, S& shader) {
            VertexStates<V, S> vs;
            // get positions from vertex shader
            vs.a_state = shader;
            Vec<4> a_clip = vs.a_state.vertex(vertex_a);
            if(a_clip.w() <= 0 || a_clip.z() == 0) { return; }
            vs.a_idepth = 1.0 / a_clip.z();
            vs.b_state = shader;
            Vec<4> b_clip = vs.b_state.vertex(vertex_b);
            if(b_clip.w() <= 0 || b_clip.z() == 0) { return; }
            vs.b_idepth = 1.0 / b_clip.z();
            vs.c_state = shader;
            Vec<4> c_clip = vs.c_state.vertex(vertex_c);
            if(c_clip.w() <= 0 || c_clip.z() == 0) { return; }
            vs.c_idepth = 1.0 / c_clip.z();
            // perform perspective division
            Vec<3> a_ndc = a_clip.swizzle<3>("xyz") / a_clip.w();
            Vec<3> b_ndc = b_clip.swizzle<3>("xyz") / b_clip.w();
            Vec<3> c_ndc = c_clip.swizzle<3>("xyz") / c_clip.w();
            // convert vertices to pixel space
            Mat<3> to_pixel_space
                = Mat<3>::translate(Vec<2>(0, this->height))
                * Mat<3>::scale(Vec<2>(this->width / 2, this->height / 2 * -1))
                * Mat<3>::translate(Vec<2>(1, 1));
            Vec<3> a = to_pixel_space * a_ndc;
            Vec<3> b = to_pixel_space * b_ndc;
            Vec<3> c = to_pixel_space * c_ndc;
            // sort vertex pixel positions (a, b, c in order of ascending y)
            if(a.y() > b.y()) {
                std::swap(a, b);
                std::swap(vs.a_state, vs.b_state);
                std::swap(vs.a_idepth, vs.b_idepth);
            } 
            if(b.y() > c.y()) { 
                std::swap(b, c);
                std::swap(vs.b_state, vs.c_state);
                std::swap(vs.b_idepth, vs.c_idepth);
            } 
            if(a.y() > b.y()) { 
                std::swap(a, b); 
                std::swap(vs.a_state, vs.b_state);
                std::swap(vs.a_idepth, vs.b_idepth);
            }
            // compute size of triangle
            double t_area = triangle_area(
                a.swizzle<2>("xy"), b.swizzle<2>("xy"), c.swizzle<2>("xy")
            );
            if(t_area == 0.0) { return; }
            // draw traingle segments
            shader.set_vertex_states(&vs);
            // segment: high -> mid (top half)
            render_triangle_segment(
                a, b, c, t_area,
                a.swizzle<2>("xy"), b.swizzle<2>("xy"), &vs, shader
            );
            // segment: mid -> low (bottom half)
            render_triangle_segment(
                a, b, c, t_area,
                b.swizzle<2>("xy"), c.swizzle<2>("xy"), &vs, shader
            );
            shader.clear_vertex_states();
        }

        public:
        template<typename V, typename S>
        void draw_mesh(const Mesh<V>& mesh, S& shader) {
            static_assert(std::is_base_of<Shader<V, S>, S>(), "Must be a shader!");
            static_assert(std::is_copy_constructible<S>(), "Must be copyable!");
            for(uint16_t elem_i = 0; elem_i < mesh.elements.size(); elem_i += 1) {
                auto indices = mesh.elements[elem_i];
                this->draw_triangle(
                    mesh.vertices[std::get<0>(indices)],
                    mesh.vertices[std::get<1>(indices)],
                    mesh.vertices[std::get<2>(indices)],
                    shader
                );
            } 
        }

    };

}