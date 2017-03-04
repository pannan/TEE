#pragma once

#include "GeometryDX11.h"
#include "MzMesh.h"
#include "ShareGeometryDX11.h"
BEGIN_TEE


class MzRenderable
{
public:

	//for test
	Glyph3::GeometryPtr	createGeometry();

	static ShareGeometryPtr createMzGeometry(MzMeshPtr& mzMesh);


protected:


};

END_TEE