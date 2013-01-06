#include <minecraft/Cube.hpp>

namespace minecraft {	
	void Cube::Draw() const {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,m_textureId);
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, m_nbVertices);
		glBindVertexArray(0);
	}
}
