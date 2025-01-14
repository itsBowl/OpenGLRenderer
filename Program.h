#pragma once
#include <GL/gl3w.h> 
#include <SDL/SDL.h>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>
#include <string>
#include <vector>



namespace Render
{
	class Program
	{
	public:

#pragma warning( suppress : 26495)
		Program(std::string directory) { makeProgram(directory); }

		void use() { glUseProgram(id); }

		void setFloat4(const std::string name, glm::vec4 value) const
		{
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1,
				glm::value_ptr(value));
		}

		void setMat4(const std::string name, glm::mat4 value) const
		{
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1,
				GL_FALSE, glm::value_ptr(value));
		}

		void setInt(const std::string name, int value) const
		{
			GLuint uniformLocation = glGetUniformLocation(id, name.c_str());
			glUniform1i(uniformLocation, (GLint)value);
		}
	private:
		GLuint id;
		void makeProgram(std::string);
		GLuint compileShader(GLenum, const std::vector<char>&);
		bool compileStatus(GLuint);
		

	};

}

