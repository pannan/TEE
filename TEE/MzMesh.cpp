#include "stdafx.h"
#include "MzMesh.h"
#include "MzSubMesh.h"

BEGIN_TEE

MzMesh::MzMesh() : m_pVertexData(nullptr), m_pIndexData(nullptr)
{

}

MzMesh::~MzMesh()
{
	SAFE_DELETE(m_pIndexData);
	SAFE_DELETE(m_pVertexData);

	for each (MzSubMesh* var in m_subMeshList)
	{
		SAFE_DELETE(var);
	}
}

void*	MzMesh::mallocVertexBuffer(size_t bufferSize)
{
	SAFE_DELETE(m_pVertexData);

	m_pVertexData = new byte[bufferSize];
	return m_pVertexData;
}

void*	MzMesh::mallocIndexBuffer(size_t bufferSize)
{
	SAFE_DELETE(m_pIndexData);

	m_pIndexData = new byte[bufferSize];
	return m_pVertexData;
}

MzSubMesh*	MzMesh::addSubMesh()
{
	MzSubMesh* subMesh = new MzSubMesh;
	m_subMeshList.push_back(subMesh);
	return subMesh;
}

END_TEE
