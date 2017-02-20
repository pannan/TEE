#include "stdafx.h"
#include "HeightMapPattern.h"
#include "noiseutils.h"
#include "imgui.h"

using namespace noise;
using namespace Glyph3;

extern RendererDX11* g_pRenderer11;

namespace TEE
{
	void HeightMapPattern::render()
	{
		ShaderResourceViewDX11& srv = g_pRenderer11->GetShaderResourceViewByIndex(m_noiseTexSRVID);
		void* texID = (void*)srv.GetSRV();
		ImGui::Image(texID, ImVec2(100, 100));
	}

	void	HeightMapPattern::createNoiseTexture()
	{
		Texture2dConfigDX11 FilteredConfig;
		FilteredConfig.SetColorBuffer(m_width, m_height);
		FilteredConfig.SetBindFlags(D3D11_BIND_SHADER_RESOURCE);
		FilteredConfig.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
		FilteredConfig.SetUsage(D3D11_USAGE_DYNAMIC);
		FilteredConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);
		m_noiseTex = g_pRenderer11->CreateTexture2D(&FilteredConfig, 0);

		D3D11_MAPPED_SUBRESOURCE mapped;
		mapped = g_pRenderer11->pImmPipeline->MapResource(m_noiseTex->m_iResource, 0, D3D11_MAP_WRITE_DISCARD, 0);
		//memcpy(mapped.pData, &lights[0], sizeof(LightParams) * lights.size());
		DWORD* buffer = (DWORD*)mapped.pData;
		for (size_t i = 0; i < m_width * m_height; ++i)
		{
			buffer[i] = 0xff0000ff;
		}
		//memset(mapped.pData, 0xffffffff, 256 * 256 * 4);
		g_pRenderer11->pImmPipeline->UnMapResource(m_noiseTex->m_iResource, 0);

		m_noiseTexSRVID = g_pRenderer11->CreateShaderResourceView(m_noiseTex->m_iResource, NULL);
	}

	void HeightMapPattern::make(int width, int height, bool seamless)
	{
		module::Perlin myModule;
		utils::NoiseMap heightMap;
		utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(myModule);
		heightMapBuilder.SetDestNoiseMap(heightMap);
		heightMapBuilder.SetDestSize(width, height);
		heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
		heightMapBuilder.Build();

		utils::RendererImage renderer;
		utils::Image image;
		renderer.SetSourceNoiseMap(heightMap);
		renderer.SetDestImage(image);
		renderer.Render();

		utils::WriterBMP writer;
		writer.SetSourceImage(image);
		writer.SetDestFilename("heightmap.bmp");
		writer.WriteDestFile();

		//fill to texture
		D3D11_MAPPED_SUBRESOURCE mapped;
		mapped = g_pRenderer11->pImmPipeline->MapResource(m_noiseTex->m_iResource, 0, D3D11_MAP_WRITE_DISCARD, 0);
		//memcpy(mapped.pData, &lights[0], sizeof(LightParams) * lights.size());
		DWORD* buffer = (DWORD*)mapped.pData;
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				utils::Color* c = image.GetSlabPtr(x, y);
				DWORD dw = 0;
				dw = c->red << 24;
				dw |= c->green << 16;
				dw |= c->blue << 8;
				dw |= c->alpha;
				buffer[y * m_width + x] = dw;
			}
		}
		
		//memset(mapped.pData, 0xffffffff, 256 * 256 * 4);
		g_pRenderer11->pImmPipeline->UnMapResource(m_noiseTex->m_iResource, 0);
	}
}