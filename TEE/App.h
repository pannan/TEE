//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "RenderApplication.h"



using namespace Glyph3;

namespace Glyph3
{
	class ViewPerspective;
	class Actor;
}

class App : public RenderApplication
{

public:
	App();
	
public:
	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void TakeScreenShot();

	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:

	void		createNoiseTexture();

protected:

	//Win32RenderWindow*		m_pWindow;
	
	int						m_iSwapChain;
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ResourcePtr			m_noiseTex;
	int						m_noiseTexSRVID;

	ViewPerspective*	m_pRenderView;

	//Actor*						m_pActor;
	std::vector<Actor*>	m_actorList;
	//GeometryPtr			m_testGeo;
};
