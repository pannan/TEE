#include "stdafx.h"
#include "SubGeometryDX11.h"

using namespace Glyph3;

BEGIN_TEE

SubGeometryDX11::SubGeometryDX11() : m_ePrimType(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED),
																m_iVertexSize(0),
																m_iVertexCount(0)
{

}

void SubGeometryDX11::Execute(PipelineManagerDX11* pPipeline, IParameterManager* pParamManager)
{
	pPipeline->InputAssemblerStage.ClearDesiredState();

	// Set the Input Assembler state, then perform the draw call.
	int layout = GetInputLayout(pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.ShaderProgram.GetState());
	pPipeline->InputAssemblerStage.DesiredState.InputLayout.SetState(layout);
	pPipeline->InputAssemblerStage.DesiredState.PrimitiveTopology.SetState(m_ePrimType);

	pPipeline->InputAssemblerStage.DesiredState.VertexBuffers.SetState(0, m_VB->m_iResource);
	pPipeline->InputAssemblerStage.DesiredState.VertexBufferStrides.SetState(0, m_iVertexSize);
	pPipeline->InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState(0, 0);

	pPipeline->InputAssemblerStage.DesiredState.IndexBuffer.SetState(m_IB->m_iResource);
	pPipeline->InputAssemblerStage.DesiredState.IndexBufferFormat.SetState(DXGI_FORMAT_R32_UINT);

	pPipeline->ApplyInputResources();

	pPipeline->DrawIndexed(m_indexCount, m_indexStart, m_vertexStart);
}

END_TEE