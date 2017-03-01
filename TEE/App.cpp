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

#include "imgui.h"
#include "imgui_impl_dx11.h"

#include "WoodPattern.h"
#include "HeightMapPattern.h"
#include "MzLoader.h"
#include "MzMesh.h"

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
	bool windowed = true;

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow(); 
	m_pWindow->SetPosition( 0, 0 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( GetName() );
	m_pWindow->Initialize( this );

	
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();
	g_pRenderer11 = m_pRenderer11;

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_10_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0 ) )
		{
			ShowWindow( m_pWindow->GetHandle(), SW_HIDE );
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}


	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.

	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
	m_pWindow->SetSwapChain( m_iSwapChain );

	// We'll keep a copy of the render target index to use in later examples.

	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_iSwapChain );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );
	
	// Bind the swap chain render target and the depth buffer for use in 
	// rendering.  

	m_pRenderer11->pImmPipeline->ClearRenderTargets();
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
	m_pRenderer11->pImmPipeline->ApplyRenderTargets();


	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.

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

	createNoiseTexture();

	RequestEvent(SYSTEM_LBUTTON_DOWN);
	RequestEvent(SYSTEM_LBUTTON_UP);
	RequestEvent(SYSTEM_MOUSE_MOVE);

	// Setup ImGui binding
	ImGui_ImplDX11_Init(m_pWindow->GetHandle(), m_pRenderer11->GetDevice(), m_pRenderer11->pImmPipeline->GetDeviceContext());

	//TEE::WoodPattern wp;
	//wp.make(256, 256, true);

	heightMap.createNoiseTexture();
	heightMap.make(256, 256, true);

	TEE::MzLoader mzLoader;
	TEE::MzMeshPtr mzMeshPtr(new TEE::MzMesh);
	mzLoader.load(mzMeshPtr);

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	if ( m_pRenderer11 )
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	if ( m_pWindow )
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
	}
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
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

	// Clear the window to a time varying color.

	float fBlue = sinf( m_pTimer->Runtime() * m_pTimer->Runtime() ) * 0.25f + 0.5f;

	ImGui_ImplDX11_NewFrame();

	ImGui::Text("Hello, world!");

	//ShaderResourceViewDX11& srv = m_pRenderer11->GetShaderResourceViewByIndex(m_noiseTexSRVID);
	//void* texID = (void*)srv.GetSRV();
	//ImGui::Image(texID, ImVec2(100, 100));
	heightMap.render();

	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 0.2f,0.2f,0.2f,1.0f ), 1.0f );

	

	ImGui::Render();

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Print the framerate out for the log before shutting down.

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

	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
//--------------------------------------------------------------------------------