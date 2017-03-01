#pragma once

BEGIN_TEE

class MzLoader;

class MzSubMesh
{
public:

	friend class MzLoader;

	MzSubMesh();
	~MzSubMesh();

	void*		mallocBoneIndexData(size_t count);

protected:

	size_t		m_vertexStart;
	size_t		m_vertexCount;

	size_t		m_indexStart;
	size_t		m_indexCount;

	size_t		m_boneCount;

	ubyte*		m_pBoneIndex;

	std::string		m_name;

private:
};

END_TEE