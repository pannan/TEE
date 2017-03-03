#pragma once

#include "PipelineExecutorDX11.h"
#include "ResourceProxyDX11.h"
#include "VertexElementDX11.h"

BEGIN_TEE

class SubGeometryDX11 : public Glyph3::PipelineExecutorDX11
{
public:

	SubGeometryDX11();

	virtual ~SubGeometryDX11(){}

	virtual void Execute(Glyph3::PipelineManagerDX11* pPipeline, Glyph3::IParameterManager* pParamManager);

	Glyph3::ResourcePtr m_VB;
	Glyph3::ResourcePtr m_IB;

	int m_iVertexSize;
	int m_iVertexCount;

	//sub mesh 自己的数据
	size_t		m_vertexStart;
	size_t		m_vertexCount;

	size_t		m_indexStart;
	size_t		m_indexCount;

	// The type of primitives listed in the index buffer
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimType;
};

typedef std::shared_ptr<SubGeometryDX11> SubGeometryPtr;

END_TEE
