namespace octet {
	namespace shaders {
		class elio_shader : public shader {
			GLuint modelToProjectionIndex_;
			GLuint colorIndex_;

		public:
			void init() {

				const char vertex_shader[] = SHADER_STR(	
					attribute vec4 pos;
					uniform mat4 modelToProjection;

					void main() { 
						gl_Position = modelToProjection * pos;
					}
				);

				const char fragment_shader[] = SHADER_STR(

					uniform vec4 color;

					void main() { 
						vec2 p = gl_FragCoord.xy / vec2(750, 750);
						vec3 col = vec3(0.4, 0.8, 0.1) /** p.y*/;
						gl_FragColor = color; 
					}
				);

				shader::init(vertex_shader, fragment_shader);

				colorIndex_ = glGetUniformLocation(program(), "color");
				modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
			}

			void render(const mat4t &modelToProjection, const vec4 &color) {
				shader::render();
				glUniform4fv(colorIndex_, 1, color.get());
				glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
			}
		};
	}
}
