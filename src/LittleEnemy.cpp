//
// Created by Sarah Jade on 2/21/18.
//

#include "LittleEnemy.hpp"


#include <cmath>

Texture LittleEnemy::little_enemy_texture;

bool LittleEnemy::init() {
    // Load shared texture
    if (!little_enemy_texture.is_valid()) {
        if (!little_enemy_texture.load_from_file(textures_path("enemy0.png"))) {
            fprintf(stderr, "Failed to load little enemy texture!");
            return false;
        }
    }

    // The position corresponds to the center of the texture
    float wr = little_enemy_texture.width * 0.5f;
    float hr = little_enemy_texture.height * 0.5f;

    TexturedVertex vertices[4];
    vertices[0].position = {-wr, +hr, -0.02f};
    vertices[0].texcoord = {0.f, 1.f};
    vertices[1].position = {+wr, +hr, -0.02f};
    vertices[1].texcoord = {1.f, 1.f};
    vertices[2].position = {+wr, -hr, -0.02f};
    vertices[2].texcoord = {1.f, 0.f};
    vertices[3].position = {-wr, -hr, -0.02f};
    vertices[3].texcoord = {0.f, 0.f};

    // counterclockwise as it's the default opengl front winding direction
    uint16_t indices[] = {0, 3, 1, 1, 3, 2};

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;

    // Setting initial values, scale is negative to make it face the opposite way
    // 1.0 would be as big as the original texture
    m_scale.x = -0.4f;
    m_scale.y = 0.4f;
    m_rotation = 0.f;

    return true;
}

// Call if init() was successful
// Releases all graphics resources
void LittleEnemy::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void LittleEnemy::update(float ms) {
    // Move fish along -X based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    //const float TURTLE_SPEED = 200.f;
    //float step = -TURTLE_SPEED * (ms / 1000);
    //m_position.x += step;

//
//        vec2 normalized_movement = m_movement_dir;
//        if (m_movement_dir.x != 0 && m_movement_dir.y != 0) {
//            normalized_movement = normalize(normalized_movement);
//        }
//
//        move({ normalized_movement.x * 10, normalized_movement.y * 10 });
//
//        // Set player to face mouse
//    //TODO set the particles to move toward player
//        float delta_x = m_mouse.x - m_position.x;
//        float delta_y = m_position.y - m_mouse.y;
//        float angle = (float)atan2(delta_y, delta_x);
//        set_rotation(3.14/2);
//
//    printf("updated enemy");
}

void LittleEnemy::draw(const mat3 &projection) {
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    transform_begin();
    transform_translate(m_position);
    transform_rotate(m_rotation);
    transform_scale(m_scale);
    transform_end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) 0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, little_enemy_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *) &transform);
    float color[] = {1.f, 1.f, 1.f};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *) &projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 LittleEnemy::get_position() const {
    return m_position;
}

void LittleEnemy::set_position(vec2 position) {
    m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the little enemy
vec2 LittleEnemy::get_bounding_box() const {
    // fabs is to avoid negative scale due to the facing direction
    return {std::fabs(m_scale.x) * little_enemy_texture.width, std::fabs(m_scale.y) * little_enemy_texture.height};
}

bool LittleEnemy::collides_with(const Bullet &bullet) {
    /*
    float dx = m_position.x - bullet.get_position().x;
    float dy = m_position.y - bullet.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(bullet.get_bounding_box().x, bullet.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
//    r *= 0.6f;
    printf("\nr*r = %f\n",r * r);
    printf("d_sq = %f\n",d_sq);
    printf("d_sq < r*r? %i\n", d_sq<r*r);
    if (d_sq < r * r) {
        printf("YES COLLISION: d_sq < r*r\n");
        return true;
    }
    return false;
     */

    printf("%f <= %f\n &&\n",get_position().x, bullet.get_position().x + get_bounding_box().x);
    printf("%f <= %f\n &&\n",bullet.get_position().x, get_position().x + get_bounding_box().x);
    printf("%f <= %f\n &&\n",get_position().y, bullet.get_position().y + get_bounding_box().y);
    printf("%f <= %f\n &&\n",bullet.get_position().y, get_position().y + get_bounding_box().y);

    printf("COLLISION RESULT: %i\n", (get_position().x <= bullet.get_position().x + get_bounding_box().x &&
                                      bullet.get_position().x <= get_position().x + get_bounding_box().x &&
                                      get_position().y <= bullet.get_position().y + get_bounding_box().y &&
                                      bullet.get_position().y <= get_position().y + get_bounding_box().y));

    return (get_position().x <= bullet.get_position().x + get_bounding_box().x &&
            bullet.get_position().x <= get_position().x + get_bounding_box().x &&
            get_position().y <= bullet.get_position().y + get_bounding_box().y &&
            bullet.get_position().y <= get_position().y + get_bounding_box().y);

}

void LittleEnemy::scale_by(float scale) {
    m_scale.x *= scale;
    m_scale.y *= scale;
}
