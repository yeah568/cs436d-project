#include "TextRenderer.hpp"


TextRenderer::TextRenderer(std::string font_name, int size) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		printf("ERROR::FREETYPE: Could not init FreeType Library\n");
		exit(1);
	}

	FT_Face face;
	if (FT_New_Face(ft, font_path(font_name).c_str(), 0, &face)) {
		printf("ERROR::FREETYPE: Failed to load font:\n%s\n", font_name.c_str());
		exit(1);
	}

	FT_Set_Pixel_Sizes(face, 0, size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			printf("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			{ static_cast<float>(face->glyph->bitmap.width),  static_cast<float>(face->glyph->bitmap.rows) },
			{ static_cast<float>(face->glyph->bitmap_left),  static_cast<float>(face->glyph->bitmap_top) },
			static_cast<GLuint>(face->glyph->advance.x)
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_colour = { 1.f, 1.f, 1.f };
	m_scale = { 1.f, -1.f };
	m_position = { 0.f, 0.f };

	effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl"));
}

void TextRenderer::setColour(vec3 colour) {
	m_colour = colour;
}

void TextRenderer::setPosition(vec2 position) {
	m_position = position;
}

void TextRenderer::setScale(vec2 scale) {
	m_scale = scale;
}

void TextRenderer::renderString(const mat3& projection, std::string text) {

	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setting shaders
	glUseProgram(effect.program);

	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);


	// Activate corresponding render state	
	glUniform3f(glGetUniformLocation(effect.program, "textColor"), m_colour.x, m_colour.y, m_colour.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	float x = 0;
	float y = 0;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y);

		GLfloat w = ch.size.x;
		GLfloat h = ch.size.y;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vao);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::draw(const mat3& projection) {
	return;
}