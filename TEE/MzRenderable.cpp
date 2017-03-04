#include "stdafx.h"
#include "MzRenderable.h"

using namespace Glyph3;

BEGIN_TEE

GeometryPtr	MzRenderable::createGeometry()
{
	const size_t vertexCount = 4;
	// create the vertex element streams
	//add vertex color
	VertexElementDX11* pPositions = new VertexElementDX11(3, vertexCount);
	pPositions->m_SemanticName = VertexElementDX11::PositionSemantic;
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	// add vertex color
	VertexElementDX11* pVertexColor = new VertexElementDX11(4, vertexCount);
	pVertexColor->m_SemanticName = "COLOR";// VertexElementDX11::PositionSemantic;
	pVertexColor->m_uiSemanticIndex = 0;
	pVertexColor->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pVertexColor->m_uiInputSlot = 0;
	pVertexColor->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pVertexColor->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pVertexColor->m_uiInstanceDataStepRate = 0;

	Vector3f* pPos = (Vector3f*)((*pPositions)[0]);
	Vector4f* pColor = (Vector4f*)((*pVertexColor)[0]);

	pPos[0] = Vector3f(-1.0f, -1.0f, 5.0f);
	pPos[1] = Vector3f(-1.0f, 1.0f, 5.0f);
	pPos[2] = Vector3f(1.0f, 1.0f, 5.0f);
	pPos[3] = Vector3f(1.0f, -1.0f, 5.0f);

	pColor[0] = Vector4f(1.0, 0, 0, 1.0);
	pColor[1] = Vector4f(0.0, 1, 0, 1.0);
	pColor[2] = Vector4f(0.0, 0, 1, 1.0);
	pColor[3] = Vector4f(1.0, 1, 0, 1.0);


	GeometryPtr MeshPtr = GeometryPtr(new GeometryDX11());

	MeshPtr->AddIndex(0);
	MeshPtr->AddIndex(1);
	MeshPtr->AddIndex(2);

	MeshPtr->AddIndex(2);
	MeshPtr->AddIndex(3);
	MeshPtr->AddIndex(0);

	MeshPtr->AddElement(pPositions);
	MeshPtr->AddElement(pVertexColor);

	MeshPtr->LoadToBuffers();
	MeshPtr->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return MeshPtr;
}

ShareGeometryPtr MzRenderable::createMzGeometry(MzMeshPtr& mzMesh)
{
	ShareGeometryPtr geometryPtr = ShareGeometryPtr(new ShareGeometryDX11());

	if (mzMesh->m_indexCount == 0 || mzMesh->m_vertexCount == 0)
		return geometryPtr;

	const size_t vertexCount = mzMesh->m_vertexCount;
	VertexElementDX11* pPositions = new VertexElementDX11(3, vertexCount);
	pPositions->m_SemanticName = VertexElementDX11::PositionSemantic;
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	// add vertex color
	VertexElementDX11* pVertexColor = new VertexElementDX11(4, vertexCount);
	pVertexColor->m_SemanticName = "COLOR";// VertexElementDX11::PositionSemantic;
	pVertexColor->m_uiSemanticIndex = 0;
	pVertexColor->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pVertexColor->m_uiInputSlot = 0;
	pVertexColor->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pVertexColor->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pVertexColor->m_uiInstanceDataStepRate = 0;

	Vector3f* pPos = (Vector3f*)((*pPositions)[0]);
	Vector4f* pColor = (Vector4f*)((*pVertexColor)[0]);

	byte* vertexBuffer = (byte*)mzMesh->m_pVertexData;
	const size_t posSize = sizeof(float) * 3;
	for (size_t i = 0; i < vertexCount; ++i)
	{
		pPos[i] = *(Vector3f*)vertexBuffer;
		vertexBuffer += mzMesh->m_vertexSize;
		pColor[i] = Vector4f(1, 0, 0, 1);
	}

	if (mzMesh->m_indexType == IT_16BIT)
	{
		unsigned short* indexBuffer = (unsigned short*)mzMesh->m_pIndexData;
		for (size_t i = 0; i < mzMesh->m_indexCount; ++i)
		{
			unsigned short index = indexBuffer[i];
			geometryPtr->AddIndex(index);
		}
	}
	else
	{
		unsigned int* indexBuffer = (unsigned int*)mzMesh->m_pIndexData;
		for (size_t i = 0; i < mzMesh->m_indexCount; ++i)
		{
			unsigned int index = indexBuffer[i];
			geometryPtr->AddIndex(index);
		}
	}

	geometryPtr->AddElement(pPositions);
	geometryPtr->AddElement(pVertexColor);

	geometryPtr->LoadToBuffers();
	geometryPtr->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	geometryPtr->m_mzMesh = mzMesh;

	return geometryPtr;
}

END_TEE