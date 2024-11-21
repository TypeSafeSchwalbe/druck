
#include "examples.hpp"


struct TriangleVertex {
    Vec<2> pos;
    Vec<3> color;
};

struct TriangleShader: rendering::Shader<TriangleVertex, TriangleShader> {
    Vec<4> vertex(TriangleVertex vertex) override {
        this->color = vertex.color;
        return vertex.pos.with(0.0).with(1.0);
    }

    Vec<3> color;

    Vec<4> fragment() override {
        this->interpolate(&this->color);
        return this->color.with(1.0);
    }
};


void render_triangle() {
    // open the window
    druck::init("Druck Triangle Example", WINDOW_SIZE, WINDOW_SIZE, FPS);
    // make a buffer to render on
    auto buffer = rendering::Surface(WINDOW_SIZE, WINDOW_SIZE);
    // instanicate the shader
    auto shader = TriangleShader();
    // construct the mesh containing the triangle
    auto triangle = rendering::Mesh<TriangleVertex>();
    triangle.add_vertex({ Vec<2>(-0.5,  0.5), Vec<3>(1.0, 0.0, 0.0) }); // top left
    triangle.add_vertex({ Vec<2>( 0.5,  0.5), Vec<3>(0.0, 1.0, 0.0) }); // top right
    triangle.add_vertex({ Vec<2>( 0.0, -0.5), Vec<3>(0.0, 0.0, 1.0) }); // bottom center
    triangle.add_element(0, 1, 2); // one triangle formed from the 3 vertices
    // rendering loop
    while(druck::is_running()) {
        buffer.clear();
        buffer.draw_mesh(triangle, shader);
        druck::display_buffer(&buffer);
    }
    druck::stop();
}