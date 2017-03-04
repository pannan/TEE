//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "stdafx.h"
#include "App.h"
#include "Log.h"

#include <sstream>

#include "ViewPerspective.h"
#include "MaterialGeneratorDX11.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"

#include "WoodPattern.h"
#include "HeightMapPattern.h"
#include "MzLoader.h"
#include "MzMesh.h"
#include "MzRenderable.h"
#include "ShareGeometryDX11.h"

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

RendererDX11* g_pRenderer11 = nullptr;
TEE::HeightMapPattern heightMap;
using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
}

void	App::createNoiseTexture()
{
	Texture2dConfigDX11 FilteredConfig;
	FilteredConfig.SetColorBuffer(256, 256);
	FilteredConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE);
	FilteredConfig.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
	FilteredConfig.SetUsage(D3D11_USAGE_DYNAMIC);
	FilteredConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE );
	m_noiseTex = m_pRenderer11->CreateTexture2D(&FilteredConfig, 0);

	D3D11_MAPPED_SUBRESOURCE mapped;
	mapped = m_pRenderer11->pImmPipeline->MapResource(m_noiseTex->m_iResource, 0, D3D11_MAP_WRITE_DISCARD, 0);
	//memcpy(mapped.pData, &lights[0], sizeof(LightParams) * lights.size());
	DWORD* buffer = (DWORD*)mapped.pData;
	for (size_t i = 0; i < 255 * 255; ++i)
	{
		buffer[i] = 0xff0000ff;
	}
	//memset(mapped.pData, 0xffffffff, 256 * 256 * 4);
	m_pRenderer11->pImmPipeline->UnMapResource(m_noiseTex->m_iResource, 0);

	m_noiseTexSRVID = m_pRenderer11->CreateShaderResourceView(m_noiseTex->m_iResource, NULL);
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 1800;
	int height = 1080;

	if (!ConfigureRenderingEngineComponents(width, height, D3D_FEATURE_LEVEL_11_0))
		return(false);

	if (!ConfigureRenderingSetup()) 
		return(false);

	SetMultiThreadedMode(false);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( width );
	viewport.Height = static_cast< float >( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	int ViewPort = m_pRenderer11->CreateViewPort( viewport );
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState( 1 );
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState( 0, ViewPort );

	//createNoiseTexture();

	RequestEvent(SYSTEM_LBUTTON_DOWN);
	RequestEvent(SYSTEM_LBUTTON_UP);
	RequestEvent(SYSTEM_MOUSE_MOVE);

	// Setup ImGui binding
	ImGui_ImplDX11_Init(m_pWindow->GetHandle(), m_pRenderer11->GetDevice(), m_pRenderer11->pImmPipeline->GetDeviceContext());


	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	Actor* actor = new Actor();
	MaterialPtr material = MaterialGeneratorDX11::GenerateSolidColor(*m_pRenderer11);
	/*TEE::MzRenderable mzr;
	GeometryPtr m_testGeo = mzr.createGeometry();
	

	Entity3D*	pEntity = new Entity3D();
	pEntity->Visual.SetGeometry(m_testGeo);
	pEntity->Visual.SetMaterial(material);
	pEntity->Transform.Position() = Vector3f(0.0f,0.0f, 0.0f);

	actor->GetNode()->AttachChild(pEntity);
	m_pScene->AddActor(actor);
	m_actorList.push_back(actor);*/
	//load mz	
	TEE::MzLoader mzLoader;
	TEE::MzMeshPtr mzMeshPtr(new TEE::MzMesh);
	mzLoader.load(mzMeshPtr);

	//create mz renderable
	TEE::ShareGeometryPtr mzGeo = TEE::MzRenderable::createMzGeometry(mzMeshPtr);
	Entity3D* pEntity = new Entity3D();
	pEntity->Visual.SetGeometry(mzGeo);
	pEntity->Visual.SetMaterial(material);
	pEntity->Transform.Position() = Vector3f(0.0f, 0.0f, 0.0f);

	actor->GetNode()->AttachChild(pEntity);
	m_pScene->AddActor(actor);
	m_actorList.push_back(actor);
	

	

	//TEE::WoodPattern wp;
	//wp.make(256, 256, true);

	//heightMap.createNoiseTexture();
	//heightMap.make(256, 256, true);
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	ImGui_ImplDX11_NewFrame();

	ImGui::Text("Hello, world!");

	//ShaderResourceViewDX11& srv = m_pRenderer11->GetShaderResourceViewByIndex(m_noiseTexSRVID);
	//void* texID = (void*)srv.GetSRV();
	//ImGui::Image(texID, ImVec2(100, 100));
	//heightMap.render();

	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 0.2f,0.2f,0.2f,1.0f ), 1.0f );

	m_pScene->Update(m_pTimer->Elapsed());
	m_pScene->Render(m_pRenderer11);

	ImGui::Render();

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Print the framerate out for the log before shutting down.
	ImGui_ImplDX11_Shutdown();

	//for (auto var : m_actorList)
	//{
	//	SAFE_DELETE(var);
	//}

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
void App::TakeScreenShot()
{
	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, GetName() );
	}
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();
	}

	EvtWindowMsg* msg = (EvtWindowMsg*)pEvent._Get();
	ImGui_ImplDX11_WndProcHandler(msg->GetWindowHandle(),msg->GetMsg(), msg->GetWParam(), msg->GetLParam());

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
//--------------------------------------------------------------------------------