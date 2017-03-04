#include "stdafx.h"
#include "ShareGeometryDX11.h"

using namespace Glyph3;

BEGIN_TEE

ShareGeometryDX11::ShareGeometryDX11()																
{

}

void ShareGeometryDX11::Execute(PipelineManagerDX11* pPipeline, IParameterManager* pParamManager)
{
	pPipeline->InputAssemblerStage.ClearDesiredState();

	// Set the Input Assembler state, then perform the draw call.
	int layout = GetInputLayout(pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.ShaderProgram.GetState());
	if (layout == -1)
		return;
	pPipeline->InputAssemblerStage.DesiredState.InputLayout.SetState(layout);
	pPipeline->InputAssemblerStage.DesiredState.PrimitiveTopology.SetState(m_ePrimType);

	pPipeline->InputAssemblerStage.DesiredState.VertexBuffers.SetState(0, m_VB->m_iResource);
	pPipeline->InputAssemblerStage.DesiredState.VertexBufferStrides.SetState(0, m_iVertexSize);
	pPipeline->InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState(0, 0);

	pPipeline->InputAssemblerStage.DesiredState.IndexBuffer.SetState(m_IB->m_iResource);
	pPipeline->InputAssemblerStage.DesiredState.IndexBufferFormat.SetState(DXGI_FORMAT_R32_UINT);

	pPipeline->ApplyInputResources();

	for (size_t i = 0; i <1 /*m_mzMesh->m_subMeshList.size()*/; ++i)
	{
		const MzSubMeshPtr& subMesh = m_mzMesh->m_subMeshList[i];
		pPipeline->DrawIndexed(subMesh->m_indexCount, subMesh->m_indexStart, subMesh->m_vertexStart);
	}
	
}

END_TEE