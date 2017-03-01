#include "stdafx.h"
#include "MzSubMesh.h"

BEGIN_TEE

MzSubMesh::MzSubMesh() :	m_boneCount(0),
												m_indexCount(0),
												m_indexStart(0),
												m_vertexCount(0),
												m_vertexStart(0),
												m_pBoneIndex(nullptr)
{

}

MzSubMesh::~MzSubMesh()
{
	SAFE_DELETE(m_pBoneIndex);
}

void*	MzSubMesh::mallocBoneIndexData(size_t count)
{
	SAFE_DELETE(m_pBoneIndex);

	m_pBoneIndex = new ubyte[count];
	return m_pBoneIndex;
}

END_TEE