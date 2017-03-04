#pragma once

#include "PipelineExecutorDX11.h"
#include "ResourceProxyDX11.h"
#include "VertexElementDX11.h"
#include "GeometryDX11.h"
#include "MzMesh.h"

BEGIN_TEE

class ShareGeometryDX11 : public Glyph3::GeometryDX11
{
public:

	ShareGeometryDX11();

	virtual ~ShareGeometryDX11(){}

	virtual void Execute(Glyph3::PipelineManagerDX11* pPipeline, Glyph3::IParameterManager* pParamManager);


	MzMeshPtr	m_mzMesh;
};

typedef std::shared_ptr<ShareGeometryDX11> ShareGeometryPtr;

END_TEE
