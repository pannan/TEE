#pragma once

#include "GeometryDX11.h"
#include "MzMesh.h"
#include "SubGeometryDX11.h"
BEGIN_TEE


class MzRenderable
{
public:

	//for test
	Glyph3::GeometryPtr	createGeometry();

	static Glyph3::GeometryPtr createGeometry(MzMeshPtr& mzMesh);

	static SubGeometryPtr createSubGeometry(MzMeshPtr& mzMesh);

protected:


};

END_TEE