#ifndef __GLSHAPEINSTANCE__
#define __GLSHAPEINSTANCE__

#include <GL/glew.h>
#include "../include/imac2gl3/shapes/Sphere.hpp"
#include "../include/imac2gl3/shapes/Cone.hpp"
#include "../include/imac2gl3/shapes/Cube.hpp"

class GLShapeInstance{

	public:
		GLShapeInstance(const imac2gl3::Sphere & mySphere);
		GLShapeInstance(const imac2gl3::Cone & myCone);
		GLShapeInstance(const imac2gl3::Cube & myCube);
		~GLShapeInstance();
		void draw();
	private:
		GLuint vbo;
		GLuint vao;
		int vertexCount;

};


#endif

