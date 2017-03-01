#pragma once

#include "MzMesh.h"

BEGIN_TEE



class MzLoader
{
public:

	void load(MzMeshPtr& mzMeshPtr);

protected:

	bool readMzVersion(std::ifstream& fin, uint dataSize);

	bool readMzSharedGeometry(std::ifstream& fin, uint dataSize);

	bool readMzGeometry(std::ifstream& fin, uint dataSize);

	bool readMzBones(std::ifstream& fin, uint dataSize);

protected:

	MzMeshPtr	m_mzMeshPtr;

private:
};

END_TEE