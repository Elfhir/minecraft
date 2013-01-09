#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <minecraft/shader_tools.hpp>
#include <minecraft/GraphicEngine.hpp>
#include <minecraft/Light.hpp>

namespace minecraft {
	GraphicEngine::~GraphicEngine() {
		typedef std::map<std::string,Cube*>::iterator ItType;
		for(ItType iterator = m_gameObjects.begin(); iterator != m_gameObjects.end(); iterator++) {
			delete iterator->second;
		}
	}
	
	void GraphicEngine::StartGL() throw(std::runtime_error) {		
		// OpenGL features
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// Shaders
		GLuint program = minecraft::loadProgram("shaders/transform.vs.glsl", "shaders/onlyCoords.fs.glsl");
		if(!program)
			throw std::runtime_error("Can't load shaders");
		glUseProgram(program);
		
		m_uniformTransformLocation = glGetUniformLocation(program,"uMVPMatrix");
		m_uniform2dMode = glGetUniformLocation(program,"u2dMode");
		m_uniformLightening = glGetUniformLocation(program,"uLightening"); // Whether enlightment need to be processed
		glUniform1i(glGetUniformLocation(program, "uTexture"), 0);
		
		// Sun
		m_uniformSunDirection = glGetUniformLocation(program,"sunDirection");
		m_uniformSunAmbient = glGetUniformLocation(program,"sunAmbient");
		m_uniformSunIntensity = glGetUniformLocation(program,"sunIntensity");
		
		// PointLight
		m_uniformLightPosition = glGetUniformLocation(program,"lightPosition");
		m_uniformLightIntensity = glGetUniformLocation(program,"lightIntensity");
	}
	
	void GraphicEngine::Initialize(size_t windowWidth, size_t windowHeight) {
		// Relative to the camera
		m_perspectiveMatrix = glm::perspective(50.f, windowWidth / (float) windowHeight, 0.0001f, 1000.f);
		
		// Create sunlight
		struct DirectionalLight sun;
		sun.direction=glm::vec3(0.2,-1,0.3);
		sun.ambient=0.1;
		sun.intensity=0.8;
		// Uniform variables linked to the sun
		glUniform3f(m_uniformSunDirection, sun.direction.x, sun.direction.y, sun.direction.z);
		glUniform1f(m_uniformSunAmbient, sun.ambient);
		glUniform1f(m_uniformSunIntensity, sun.intensity);
		
		// Create point light
		struct PointLight light;
		light.position=glm::vec3(1.8,2.8,1.8);
		light.intensity=1;
		light.decay=glm::vec3(0.5,0.5,0.5); // Decay : constant,linear,quadratic
		// Uniform variables linked to the light
		glUniform3f(m_uniformLightPosition, light.position.x, light.position.y, light.position.z);
		glUniform1f(m_uniformLightIntensity, light.intensity);
		glUniform3f(m_uniformLightDecay, light.decay.x, light.decay.y, light.decay.z);
		
		// Init the game objects
		m_gameObjects[std::string("SkyBoxCube")] = new SkyBoxCube();
		m_gameObjects[std::string("CloudCube")] = new CloudCube();
		m_gameObjects[std::string("CrystalCube")] = new CrystalCube();
		m_gameObjects[std::string("RockCube")] = new RockCube();
		// Init and assign the shapes
		m_shapeMgr.LoadShapes();
		m_gameObjects[std::string("SkyBoxCube")]->SetVAOId(m_shapeMgr.GetShapeVAO(std::string("skybox")));
		m_gameObjects[std::string("SkyBoxCube")]->SetNbVertices(m_shapeMgr.GetShapeNbVertices(std::string("skybox")));
		m_gameObjects[std::string("CloudCube")]->SetVAOId(m_shapeMgr.GetShapeVAO(std::string("cube")));
		m_gameObjects[std::string("CloudCube")]->SetNbVertices(m_shapeMgr.GetShapeNbVertices(std::string("cube")));
		m_gameObjects[std::string("CrystalCube")]->SetVAOId(m_shapeMgr.GetShapeVAO(std::string("cube")));
		m_gameObjects[std::string("CrystalCube")]->SetNbVertices(m_shapeMgr.GetShapeNbVertices(std::string("cube")));
		m_gameObjects[std::string("RockCube")]->SetVAOId(m_shapeMgr.GetShapeVAO(std::string("cube")));
		m_gameObjects[std::string("RockCube")]->SetNbVertices(m_shapeMgr.GetShapeNbVertices(std::string("cube")));
		// Init and assign the textures
		m_textureMgr.LoadTexture("SkyBox","./skybox.jpg");
		m_textureMgr.LoadTexture("Cloud", "./cloud.jpg");
		m_textureMgr.LoadTexture("Crystal", "./cloud.jpg");
		m_textureMgr.LoadTexture("Rock", "./rock.jpg");
		m_textureMgr.LoadTexture("Cursor","./cursor.png");
<<<<<<< HEAD
		m_textureMgr.LoadTexture("Inventory","./inventory.png");
		m_gameObjects[std::string("CloudCube")]->SetTexId(m_textureMgr.GetTextureId((char*)"Cloud"));
		m_gameObjects[std::string("CrystalCube")]->SetTexId(m_textureMgr.GetTextureId((char*)"Crystal"));
		m_gameObjects[std::string("RockCube")]->SetTexId(m_textureMgr.GetTextureId((char*)"Rock"));
=======
		m_gameObjects[std::string("SkyBoxCube")]->SetTexId(m_textureMgr.GetTextureId("SkyBox"));
		m_gameObjects[std::string("CloudCube")]->SetTexId(m_textureMgr.GetTextureId("Cloud"));
		m_gameObjects[std::string("CrystalCube")]->SetTexId(m_textureMgr.GetTextureId("Crystal"));
		m_gameObjects[std::string("RockCube")]->SetTexId(m_textureMgr.GetTextureId("Rock"));
>>>>>>> 74878555921fae9a3cee608444fa9574e83fcc24
	}
	
	void GraphicEngine::RefreshDisplay() throw(std::logic_error) {
		if( NULL == m_character || NULL == m_world )
			throw std::logic_error("Can't display game without setting the map and the character");
		
		m_transformStack.Push();
			m_transformStack.Set(m_perspectiveMatrix*m_character->GetPointOfView());
			m_world->Draw(m_transformStack,m_uniformTransformLocation);
			DrawSkyBox();
		m_transformStack.Pop();
		DrawCursor();
		DrawInventory();
	}
	
	void GraphicEngine::DrawCursor() {
		glUniform1i(m_uniform2dMode, 1); // Tell the shader it's 2D
		glUniform1i(m_uniformLightening, 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,m_textureMgr.GetTextureId("Cursor"));
		glBindVertexArray(m_shapeMgr.GetShapeVAO(std::string("cursor")));
		glDrawArrays(GL_TRIANGLES, 0, m_shapeMgr.GetShapeNbVertices(std::string("cursor")));
		glBindVertexArray(0);
		
		glUniform1i(m_uniform2dMode, 0);
		glUniform1i(m_uniformLightening, 1);
	}
	
	void GraphicEngine::DrawInventory() {
		glUniform1i(m_uniform2dMode, 1); // Tell the shader it's 2D
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,m_textureMgr.GetTextureId((char*)"Inventory"));
		glBindVertexArray(m_shapeMgr.GetShapeVAO(std::string("inventory")));
		glDrawArrays(GL_TRIANGLES, 0, m_shapeMgr.GetShapeNbVertices(std::string("inventory")));
		glBindVertexArray(0);
		
		glUniform1i(m_uniform2dMode, 0);
			 
	}

	std::string GraphicEngine::GetCubeType(Cube* cube){

		for (std::map<std::string,Cube*>::iterator ItGameObjects = m_gameObjects.begin(); ItGameObjects != m_gameObjects.end(); ++ItGameObjects){
			if(ItGameObjects->second == cube){
				return ItGameObjects->first;
			}

		}
	}
	
	void GraphicEngine::DrawSkyBox() {
		SkyBoxCube* skyBox = (SkyBoxCube*)m_gameObjects[std::string("SkyBoxCube")];
		glm::vec3 cameraPos = m_character->HeadPosition();
		
		glUniform1i(m_uniformLightening, 0);
		m_transformStack.Push();
			m_transformStack.Translate(cameraPos);
			m_transformStack.Scale(glm::vec3(std::max(m_world->GetSizeW(),std::max(m_world->GetSizeH(),m_world->GetSizeD()))*2));
			glUniformMatrix4fv(m_uniformTransformLocation, 1, GL_FALSE, glm::value_ptr(m_transformStack.Top()));
			skyBox->Draw();
		m_transformStack.Pop();
		glUniform1i(m_uniformLightening, 1);

	}

	void GraphicEngine::AddInInventory(Cube* cube) throw(std::invalid_argument){
		if (cube == NULL)
			throw std::invalid_argument("Non recognized type of cube");

		std::string cubeType;
		cubeType = GetCubeType(cube);

		/*If there's not corresponding cube*/	
		if(m_inventory.find(cubeType) == m_inventory.end()){
			m_inventory.insert ( std::pair<std::string,int>(cubeType,1) );
		}
		/*If there is already the same type of cube */
		else{
			m_inventory.find(cubeType)->second ++;
		}
	}
 
	/*void Inventory::RemoveFromInventory(std::string cubeType) throw(std::invalid_argument){

		if(m_inventory.find(cubeType) == m_inventory.end()){
			throw std::invalid_argument("Your inventory doesn't contain this sort of cube");
		}
		/*Case there are many elements in the inventory
		else if(m_inventory.find(cubeType)->second != 1 ){
			m_inventory.find(cubeType)->second --;	

		/*Case it's the last one of this type	
		}else{
			m_inventory.erase(m_inventory.find(CubeType));
		}
	}*/
}
