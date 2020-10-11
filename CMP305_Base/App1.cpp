#include "App1.h"

App1::App1()
{
	m_Terrain = nullptr;
	shader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Load textures
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"white", L"res/DefaultDiffuse.png");

	// Create Mesh object and shader object
	m_Terrain = new TerrainMesh(renderer->getDevice(), renderer->getDeviceContext());
	shader = new LightShader(renderer->getDevice(), hwnd);
	
	// Initialise light
	light = new Light();
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, -0.7f, 0.0f);

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	m_Terrain->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light);
	shader->render(renderer->getDeviceContext(), m_Terrain->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::SliderInt("Terrain Resolution", &terrainResolution, 2, 1024);
	if (ImGui::Button("Regenerate Terrain")) {
		if (terrainResolution != m_Terrain->GetResolution()) {
			m_Terrain->Resize(terrainResolution);
		}
		m_Terrain->BuildHeightMap();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}

	if (ImGui::Button("Flat")) {
		if (terrainResolution != m_Terrain->GetResolution()) {
			m_Terrain->Resize(terrainResolution);
		}
		m_Terrain->BuildFlatHeightMap();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}
	
	if (ImGui::Button("rand")) {
		if (terrainResolution != m_Terrain->GetResolution()) {
			m_Terrain->Resize(terrainResolution);
		}
		m_Terrain->BuildRandHeightMap();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}
	
	if (ImGui::Button("smooth")) {
		m_Terrain->Smooth();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}

	ImGui::SliderFloat("Fault amount", &faultHeight, 0.1f, 20);
	
	if (ImGui::Button("fault")) {
		m_Terrain->Fault(faultHeight);
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}
	
	if (ImGui::Button("fault cascade")) {
		for (size_t i = 0; i < 3; i++)
		{
			m_Terrain->Fault(5);
		}
		m_Terrain->Smooth(5);

		for (size_t i = 0; i < 8; i++)
		{
			m_Terrain->Fault(2);
		}
		m_Terrain->Smooth();

		for (size_t i = 0; i < 16; i++)
		{
			m_Terrain->Fault(1);
		}
		m_Terrain->Smooth(5);

		for (size_t i = 0; i < 32; i++)
		{
			m_Terrain->Fault(0.5f);
		}
		m_Terrain->Smooth();

		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}





	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}



