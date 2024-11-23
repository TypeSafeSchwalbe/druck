
#include "examples.hpp"


struct ModelShader: rendering::Shader<resources::ModelVertex, ModelShader> {
    Mat<4> projection;
    Mat<4> view;
    Mat<4> model;
    const rendering::Surface* tex;

    Vec<4> vertex(resources::ModelVertex vertex) override {
        this->uv = vertex.uv;
        return this->projection * this->view * this->model 
            * vertex.pos.with(1.0);
    }

    Vec<2> uv;

    Vec<4> fragment() override {
        this->interpolate(&this->uv);
        return this->tex->sample(uv);
    }
};


void render_car() {
    // open the window
    druck::init("Druck Triangle Example", WINDOW_SIZE, WINDOW_SIZE, FPS);
    // make a buffer to render on
    auto buffer = rendering::Surface(WINDOW_SIZE, WINDOW_SIZE);
    // load the car model and texture
    rendering::Mesh<resources::ModelVertex> car_mesh
        = resources::read_obj_model("res/car.obj");
    rendering::Surface car_tex = resources::read_texture("res/car.png");
    // instanicate and configure the shader
    auto shader = ModelShader();
    shader.tex = &car_tex;
    shader.projection = Mat<4>::perspective(
        pi / 2.0, WINDOW_SIZE, WINDOW_SIZE, 0.1, 1000.0
    );
    shader.view = Mat<4>::look_at(
        Vec<3>(0, 3, 6), // camera position
        Vec<3>(0, 0, 0), // look at the origin
        Vec<3>(0, 1, 0) // up is along the positive Y axis
    );
    // rendering loop
    double rotation = 0.0;
    while(druck::is_running()) {
        // rotate the car
        rotation += GetFrameTime();
        shader.model = Mat<4>::rotate_y(rotation);
        // render the car
        buffer.clear();
        buffer.draw_mesh(car_mesh, shader);
        druck::display_buffer(&buffer);
    }
    druck::stop();
}