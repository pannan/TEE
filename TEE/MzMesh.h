#pragma once
#include "MzDefine.h"
#include "MzSubMesh.h"
BEGIN_TEE

class MzMesh
{
public:

	MzMesh();
	~MzMesh();

	void					setVertexCount(size_t count){ m_vertexCount = count; }

	void					setIndexCount(size_t count){ m_indexCount = count; }

	void					setVertexSize(size_t size){ m_vertexSize = size; }

	void					setIndexType(IndexType type){ m_indexType = type; }

	void*				mallocVertexBuffer(size_t bufferSize);

	void*				mallocIndexBuffer(size_t bufferSize);

	MzSubMeshPtr	addSubMesh();

//protected:

	size_t			m_vertexCount;
	size_t			m_indexCount;

	size_t			m_vertexSize;

	IndexType	m_indexType;

	void*			m_pVertexData;

	void*			m_pIndexData;

	std::vector<MzSubMeshPtr>		m_subMeshList;

private:
};

typedef std::shared_ptr<MzMesh> MzMeshPtr;

END_TEE