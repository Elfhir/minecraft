#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <minecraft/Map.hpp>
#include <minecraft/Cube.hpp>
#include <minecraft/GraphicEngine.hpp>
#include <vector>

#include <iostream>


namespace minecraft {	
	Cube* Map::Get(size_t x, size_t y, size_t z) {		
		ItCubeInstance res = m_data.find(MapCoords(x,y,z));
		if( res == m_data.end() )
			return NULL;

		return std::get<0>(res->second);
	}
	
	Cube* Map::GetByPixel(GLfloat x, GLfloat y, GLfloat z){
		GLfloat cubeSize=Cube::m_size;
		x+=cubeSize*0.5;
		y+=cubeSize*0.5;
		z+=cubeSize*0.5;
		ItCubeInstance res = m_data.find(MapCoords((size_t)(x/Cube::m_size),(size_t)(y/Cube::m_size),(size_t)(z/Cube::m_size)));
		if( res == m_data.end() )
			return NULL;
		
		return std::get<0>(res->second);
	}
	
	void Map::Set(size_t x, size_t y, size_t z, Cube* cube) {
		m_data[MapCoords(x,y,z)] = std::make_tuple(cube,true);
		m_refresh = true;
		UpdateVisibilities(x,y,z,true);
	}
	
	void Map::SetByPixel(GLfloat x,GLfloat y,GLfloat z, Cube* cube){	
		Set((size_t)(x/Cube::m_size),(size_t)(y/Cube::m_size),(size_t)(z/Cube::m_size),cube);
		m_refresh = true;
	}
	
	void Map::Del(size_t x, size_t y, size_t z) {
		m_data.erase(MapCoords(x,y,z));
		m_refresh = true;
		UpdateVisibilities(x,y,z,false);
	}
	
	void Map::DelByPixel(GLfloat x,GLfloat y,GLfloat z){
		GLfloat cubeSize=Cube::m_size;
		x+=cubeSize*0.5;
		y+=cubeSize*0.5;
		z+=cubeSize*0.5;
		Del((size_t)(x/Cube::m_size),(size_t)(y/Cube::m_size),(size_t)(z/Cube::m_size));
	}
	
	void Map::FakeCreation(GLfloat x,GLfloat y,GLfloat z){
			GLfloat cubeSize=Cube::m_size;
			x+=cubeSize*0.5;
			y+=cubeSize*0.5;
			z+=cubeSize*0.5;
			SetByPixel(x,y,z, new RockCube());
	}
	
	bool Map::Exists(size_t x, size_t y, size_t z){
		/*if( x > m_width || x < 0 ||
		y > m_height || y < 0 ||
		z > m_depth || z < 0 )
			throw std::out_of_range("Position is out of the map in Exists");*/
			
		if( m_data.find(MapCoords(x,y,z)) == m_data.end() ){
			return false;
		}else{
			return true;
		}
	}
	
	bool Map::ExistsByPixel(GLfloat x, GLfloat y, GLfloat z){			
		GLfloat cubeSize=Cube::m_size;
		x+=cubeSize*0.5;
		y+=cubeSize*0.5;
		z+=cubeSize*0.5;
		
		return Exists((size_t)(x/Cube::m_size),(size_t)(y/Cube::m_size),(size_t)(z/Cube::m_size));
	}
	
	/* Update the visibility of the cube x,y,z.
	 * add: true = adding a cube, false = removing */
	void Map::UpdateVisibility(size_t x, size_t y, size_t z) throw(std::out_of_range) {	
		ItCubeInstance cube, neighbour1, neighbour2, neighbour3, neighbour4, neighbour5, neighbour6, end;
		cube = m_data.find(MapCoords(x,y,z));
		end = m_data.end();
		if( cube == end )
			throw std::out_of_range("Cube is not in the map in UpdateVisibility");
		neighbour1 = m_data.find(MapCoords(x-1,y,z));
		neighbour2 = m_data.find(MapCoords(x+1,y,z));
		neighbour3 = m_data.find(MapCoords(x,y-1,z));
		neighbour4 = m_data.find(MapCoords(x,y+1,z));
		neighbour5 = m_data.find(MapCoords(x,y,z-1));
		neighbour6 = m_data.find(MapCoords(x,y,z+1));
		
		if( std::get<1>(cube->second) && neighbour1 != end && neighbour2 != end && neighbour3 != end
		&& neighbour4 != end && neighbour5 != end && neighbour6 != end ) {
			// It can be hidden
			Cube* pointer = std::get<0>(cube->second);
			cube->second = std::make_tuple(pointer,false);
		}
		else if( !std::get<1>(cube->second) ) {
			Cube* pointer = std::get<0>(cube->second);
			cube->second = std::make_tuple(pointer,true);
		}
	}
	
	/* Update the visibility of a cube x,y,z and its neighbourhood if we
	 * add or remove it */
	void Map::UpdateVisibilities(size_t x, size_t y, size_t z, bool adding) {
		if( adding )
			UpdateVisibility(x,y,z);
		
		/* Foreach neighbour too, it won't be in the map sometime, it's ok here*/
		try {
			UpdateVisibility(x-1,y,z);
		}catch( std::out_of_range e ) {}
		try {
			UpdateVisibility(x+1,y,z);
		}catch( std::out_of_range e ) {}
		try {
			UpdateVisibility(x,y-1,z);
		}catch( std::out_of_range e ) {}
		try {
			UpdateVisibility(x,y+1,z);
		}catch( std::out_of_range e ) {}
		try {
			UpdateVisibility(x,y,z-1);
		}catch( std::out_of_range e ) {}
		try {
			UpdateVisibility(x,y,z+1);
		}catch( std::out_of_range e ) {}
	}
	
	/* Draw all the cubes at 0,0,0. . . */
	void Map::Draw() const{
		for(ItCubeInstanceConst iterator = m_data.begin(); iterator != m_data.end(); iterator++) {
			if( std::get<1>(iterator->second) ) // If the cube is not hidden by other cubes
				std::get<0>(iterator->second)->Draw();
		}
	}
	
	/* Draw the cubes at the right places considering their size */
	/*void Map::Draw(MatrixStack& matrixStack, GLuint uniformLocation) const {
		GLfloat cubeSize = Cube::m_size;
		for(ItCubeInstanceConst iterator = m_data.begin(); iterator != m_data.end(); iterator++) {
			if( std::get<1>(iterator->second) ) { // If the cube is not hidden by other cubes
				matrixStack.Push();
					matrixStack.Scale(glm::vec3(cubeSize));
					matrixStack.Translate(
						glm::vec3(
							std::get<0>(iterator->first),
							std::get<1>(iterator->first),
							std::get<2>(iterator->first)
						)
					);
					glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrixStack.Top()));
					std::get<0>(iterator->second)->Draw();
				matrixStack.Pop();
			}
		}
	}*/

	/* Draw instances */
	void Map::Draw(MatrixStack& matrixStack, GLuint uniformLocation) const {
		GLfloat cubeSize = Cube::m_size;
		matrixStack.Push();
			matrixStack.Scale(glm::vec3(cubeSize));
			glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrixStack.Top()));
		matrixStack.Pop();
		
		for(size_t i=0; i<m_instanceDatas.size(); ++i) {
			glBindTexture(GL_TEXTURE_2D,std::get<1>(m_instanceDatas[i]));
			glBindVertexArray(std::get<0>(m_instanceDatas[i]));
			glDrawArraysInstanced(GL_TRIANGLES, 0, Cube::m_tmpNbVertices, std::get<2>(m_instanceDatas[i]));
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D,0);
			//std::cout << "instance["<<i<<"] = " << std::get<2>(instanceDatas[i]);
		}
	}

	/* Get position of all visibles cubes */
	std::vector<MapCoords> Map::GetPositions(const char* type) const {
		std::vector<MapCoords> visiblesCubesCoords;
		for(ItCubeInstanceConst iterator = m_data.begin(); iterator != m_data.end(); iterator++) {
			if( 1 && std::get<0>(iterator->second)->m_name == type) { // If the cube is not hidden by other cubes
				visiblesCubesCoords.push_back(std::make_tuple(
					std::get<0>(iterator->first),
					std::get<1>(iterator->first),
					std::get<2>(iterator->first)
				));
			}
		}
		return visiblesCubesCoords;
	}
}
