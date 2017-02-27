#pragma once

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "ShaderResourceViewDX11.h"

#include "noise.h"

typedef unsigned int uint;
typedef unsigned char ubyte;
typedef unsigned short ushort;

#define BEGIN_TEE namespace TEE{
#define END_TEE }

#define  MaxVertexElementCount 16