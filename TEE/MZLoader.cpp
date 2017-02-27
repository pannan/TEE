#include "stdafx.h"
#include "MZLoader.h"
#include "FileSystem.h"
#include "Vector3f.h"
using namespace Glyph3;

BEGIN_TEE

#define READ_DATA(addres,size) fin.read((char*)(addres), size);

void MZLoader::load()
{
	FileSystem fileSys;
	std::wstring fileName = L"F:\\code\\hieroglyph3\\TEE\\Data\\Models\\player_zsm.Mz";// fileSys.GetModelsFolder() + L"player_zsm.Mz";

	std::ifstream fin;

	fin.open(fileName.c_str(), std::ios::binary);

	//计算文件大小
	fin.seekg(0, SEEK_END);
	size_t fileSize = fin.tellg();
	//fin.setstate(0, SEEK_SET);
	fin.seekg(0, SEEK_SET);


	//fin.read((char*)(&chunkType), sizeof(chunkType));
	//fin.read((char*)(&chunkSize), sizeof(chunkSize));

		
		
		uint chunkType = 0;
		uint chunkSize = 0;
		while (fileSize - fin.tellg() >= 8)
		{
			//stream->read(&chunkType, sizeof(chunkType));
			READ_DATA(&chunkType, sizeof(chunkType));
			//stream->read(&chunkSize, sizeof(chunkSize));
			READ_DATA(&chunkSize, sizeof(chunkSize));

			if (chunkSize == 0)
			{
				continue;
			}

			bool ret = true;
			uint startPos = fin.tellg();
			switch (chunkType)
			{
			case 'MVER': // 模型文件的版本号
				ret = readMzVersion(fin, chunkSize);
				break;
			case 'MVTX': // 模型顶点数据 [use_vertex_color] + _VertexData[n]
				//ret = readMzVertices(model, stream, chunkSize);
				break;
			case 'MFAC': // 模型的面数据(索引缓冲)   facecount + [vtx[0]+vtx[1]+vtx[2]]...
				//ret = readMzFaces(model, stream, chunkSize);
				break;
			case 'MF32': // 模型的面数据，32位索引
				//ret = readMzFaces32(model, stream, chunkSize);
				break;
			case 'MSUB': // 子模型(斗篷)
				//ret = readMzSubModels(model, stream, chunkSize);
				break;
			case 'MS32': // 子模型，32位索引
				//ret = readMzSubModels32(model, stream, chunkSize);
				break;
			case 'MSRD': // 图元信息，共享
				ret = readMzSharedGeometry(fin, chunkSize);
				break;
			case 'MGEO': // 图元信息，代替原来的公共顶点和公共面片信息，实际上是私有的子模型
				ret = readMzGeometry(fin, chunkSize);
				break;
			case 'MMTX': // 模型的材质
				//ret = readMzMaterials(model, stream);
				break;
			case 'MANM': // 模型的动画信息：count + [namelen + namearray + starttime + endtime]...
				//ret = readMzAnimations(model, stream);
				break;
			case 'MBON': // 模型骨骼信息
				//ret = readMzBones(model, stream);
				break;
			case 'MBOX': // 模型包围盒信息
				//ret = readMzAABB(model, stream);
				break;
			case 'MUVS':
				//ret = readMzUVScale(model, stream);
				break;
			case 'MVLL':
				//ret = readMzLocalLight(model, stream);
				break;
			case 'MCOL': // 碰撞体数据
				//ret = readMzCollision(model, stream);
				break;
			case 'FXMS':
				//ret = readFxMaterialInstances(model, stream);
				break;
			case 'MPTS':
				//ret = readParticleSystems(model, stream);
				break;
			case 'FXMT':
			{
				//MERRORLN(_T("有旧FX材质，跳过，请重新保存.") << model->getFileName());
			}
			break;
#ifdef UsePhysicOGame
			case 'RAGD':
				ret = readRagdollParts(model, stream, chunkSize);
				break;
#endif
#if (defined UsePhysic2016) || (defined Temp2016Physics)
			case 'DOLL':
			{
				ret = readMzRagdoll(model, stream);
			}
#endif
			break;

			}

			/*int skipSize = chunkSize - (stream->getPosition() - startPos);
			if (skipSize > 0)
			{
				stream->seek(skipSize, SEEK_CUR);
			}*/
		}

	fin.close();
}

bool MZLoader::readMzVersion(std::ifstream& fin, uint dataSize)
{
	uint version;
	//stream->read(&version, sizeof(version));
	READ_DATA(&version, sizeof(version));
	if (version < 1 || version>999)
	{
		//MERRORLN(_T("检测mz ver<1||ver>999:") << model->getFileName() << _T("ver:") << version);
		return false;
	}
	//model->setVersion(version);

	// 下面这个字节原本已经废弃，现在拿来用作模型标记，最低位保留
	ubyte modelFlag = 0;
	//stream->read(&modelFlag, sizeof(modelFlag));
	READ_DATA(&modelFlag, sizeof(modelFlag));

	return true;
}

bool MZLoader::readMzSharedGeometry(std::ifstream& fin, uint dataSize)
{
	// 顶点格式
	ushort vtxElemCount;
	//stream->read(&vtxElemCount, sizeof(vtxElemCount));
	READ_DATA(&vtxElemCount, sizeof(vtxElemCount));
	if (vtxElemCount > MaxVertexElementCount)
	{
		//MERRORLN(_T("顶点格式元素太多:") << model->getFileName() << _T(",elements:") << vtxElemCount << _T(",max:") << MaxVertexElementCount);
		return false;
	}

	VertexElement vtxElements[MaxVertexElementCount];
	for (int i = 0; i < vtxElemCount; i++)
	{
		VertexElement& vtxElem = vtxElements[i];
		/*stream->read(&vtxElem.StreamIndex, sizeof(vtxElem.StreamIndex));
		stream->read(&vtxElem.Offset, sizeof(vtxElem.Offset));
		stream->read(&vtxElem.Type, sizeof(vtxElem.Type));
		stream->read(&vtxElem.Usage, sizeof(vtxElem.Usage));
		stream->read(&vtxElem.UsageIndex, sizeof(vtxElem.UsageIndex));*/
		READ_DATA(&vtxElem.StreamIndex, sizeof(vtxElem.StreamIndex));
		READ_DATA(&vtxElem.Offset, sizeof(vtxElem.Offset));
		READ_DATA(&vtxElem.Type, sizeof(vtxElem.Type));
		READ_DATA(&vtxElem.Usage, sizeof(vtxElem.Usage));
		READ_DATA(&vtxElem.UsageIndex, sizeof(vtxElem.UsageIndex));
	}

	//model->addVertexElements(vtxElements, vtxElemCount);

	// 顶点数据
	int vtxCount;
	//stream->read(&vtxCount, sizeof(vtxCount));
	READ_DATA(&vtxCount, sizeof(vtxCount));
	ushort streamIdx;
	//stream->read(&streamIdx, sizeof(streamIdx));
	READ_DATA(&streamIdx, sizeof(streamIdx));
	ushort vtxSize;
	//stream->read(&vtxSize, sizeof(vtxSize));
	READ_DATA(&vtxSize, sizeof(vtxSize));
	uint vtxDataLen = vtxSize * vtxCount;

	byte* vertexBuffer = new byte[vtxDataLen];
	if (vtxDataLen > 0)
	{
		//void* vtxData = model->addVertices(NULL, vtxDataLen, vtxSize);
		//stream->read(vtxData, vtxDataLen);
		READ_DATA(vertexBuffer, vtxDataLen);
	}

	delete[] vertexBuffer;

	// 面片数据
	uint idxCount;
	//stream->read(&idxCount, sizeof(idxCount));
	READ_DATA(&idxCount, sizeof(idxCount));
	ubyte idxType;
	//stream->read(&idxType, sizeof(idxType));
	READ_DATA(&idxType, sizeof(idxType));
	int idxSize = (idxType == IT_16BIT) ? 2 : 4;
	uint idxDataLen = idxSize * idxCount;

	byte* indexBuffer = new byte[idxDataLen];
	if (idxDataLen > 0)
	{
		//void* idxData = model->addFaces(NULL, idxDataLen, (IndexType)idxType);
		//stream->read(idxData, idxDataLen);
		READ_DATA(indexBuffer, idxDataLen);
	}

	delete[] indexBuffer;

	return true;
}

//地区submesh信息
bool MZLoader::readMzGeometry(std::ifstream& fin, uint dataSize)
{
	int subModelIdx = 0;
	while (dataSize > 0)
	{
		//SubModel* subModel = new SubModel;
		uint subDataLen;
		//stream->read(&subDataLen, sizeof(subDataLen));
		READ_DATA(&subDataLen, sizeof(subDataLen));
		if (subDataLen > dataSize)
		{
			//delete subModel;
			//MERRORLN(_T("无效的模型:") << model->getFileName());
			return false;
		}

		// 名称
		//stream->read_str<ubyte>(subModel->m_name);
		//read string lenght
		READ_DATA(&subDataLen, sizeof(ubyte));
		//read string
		char subMeshName[256];
		READ_DATA(subMeshName, subDataLen);
		subMeshName[subDataLen] = 0;

		// 材质
		ushort mtlCount;
		//stream->read(&mtlCount, sizeof(mtlCount));
		READ_DATA(&mtlCount, sizeof(mtlCount));
		//subModel->m_matNumber = mtlCount;
		for (int i = 0; i < mtlCount; i++)
		{
			ushort mtlIdx;
			//stream->read(&mtlIdx, sizeof(mtlIdx));
			//read material index
			READ_DATA(&mtlIdx, sizeof(mtlIdx));
			/*if (i < eMaterial_Size)
			{
			subModel->m_matIndex[i] = mtlIdx;
			}*/
		}

		// 顶点格式
		ushort vtxElemCount;
		//stream->read(&vtxElemCount, sizeof(vtxElemCount));
		READ_DATA(&vtxElemCount, sizeof(vtxElemCount));
		if (vtxElemCount > 0)
		{
			if (vtxElemCount > MaxVertexElementCount)
			{
				//MERRORLN(_T("顶点格式元素太多:") << model->getFileName() << _T(",elements:") << vtxElemCount << _T(",max:") << MaxVertexElementCount);
				//safeDelete(subModel);
				return false;
			}

			//subModel->mVertexElementCount = vtxElemCount;

			for (int i = 0; i < vtxElemCount; i++)
			{
				/*VertexElement& vtxElem = subModel->mVertexElements[i];
				stream->read(&vtxElem.StreamIndex, sizeof(vtxElem.StreamIndex));
				stream->read(&vtxElem.Offset, sizeof(vtxElem.Offset));
				stream->read(&vtxElem.Type, sizeof(vtxElem.Type));
				stream->read(&vtxElem.Usage, sizeof(vtxElem.Usage));
				stream->read(&vtxElem.UsageIndex, sizeof(vtxElem.UsageIndex));

				if (vtxElem.Usage == VEU_BlendIndices || vtxElem.Usage == VEU_BlendWeight)
				{
				subModel->m_flag |= SMF_ANIMATED;
				}
				else if (vtxElem.Usage == VEU_Color)
				{
				subModel->mUseVertexColor = true;
				}*/
			}
		}

		// 顶点数据
		uint vtxStart;
		//stream->read(&vtxStart, sizeof(vtxStart));
		READ_DATA(&vtxStart, sizeof(vtxStart));
		uint vtxCount;
		READ_DATA(&vtxCount, sizeof(vtxCount));
		ushort streamIdx;
		READ_DATA(&streamIdx, sizeof(streamIdx));
		ushort vtxSize;
		READ_DATA(&vtxSize, sizeof(vtxSize));
		if (vtxElemCount > 0 && vtxSize * vtxCount > 0)
		{
			//subModel->mVertexSize = vtxSize;
			//subModel->mVertexData = new ubyte[vtxSize * vtxCount];
			byte* buffer = new byte[vtxSize * vtxCount];
			READ_DATA(buffer, vtxSize * vtxCount);
			delete[] buffer;
		}

		// 面片数据
		uint idxStart;
		READ_DATA(&idxStart, sizeof(idxStart));
		uint idxCount;
		READ_DATA(&idxCount, sizeof(idxCount));
		ubyte idxType;
		READ_DATA(&idxType, sizeof(idxType));
		/// 为了兼容以前的格式，这里加入一个掩码，用于标识部件是否需要便宜，主要用于公告板部件的旋转中心标记
		bool localed = false;
		if (idxType & 0xf0){
			localed = true;
			idxType &= 0x0f;
		}
		//subModel->mIndexType = (IndexType)idxType;
		int idxSize = (idxType == IT_16BIT) ? 2 : 4;
		if (vtxElemCount > 0 && idxSize * idxCount > 0)
		{
			//subModel->mIndexData = new ubyte[idxSize * idxCount];
			byte* buffer = new ubyte[idxSize * idxCount];
			READ_DATA(buffer, idxSize * idxCount);
			delete[] buffer;
		}

		// 包围盒
		Vector3f minPos, maxPos;
		READ_DATA(&minPos, sizeof(minPos));
		READ_DATA(&maxPos, sizeof(maxPos));
		//subModel->aabb.setExtents(minPos, maxPos);

		// 局部骨骼数量
		ubyte localBoneCount;
		READ_DATA(&localBoneCount, sizeof(localBoneCount));
		if (localBoneCount > 0)
		{
			//这里是骨骼数据
			ubyte localBones[256];
			READ_DATA(localBones, localBoneCount);
			for (int i = 0; i < localBoneCount; i++)
			{
				//subModel->BoneLocalToGlobal.push_back((int)localBones[i]);
			}
		}

		if (localBoneCount > 0)
		{
			//subModel->m_flag |= SMF_ANIMATED;
		}
		else{
			//subModel->m_flag &= ~SMF_ANIMATED;
		}
		if (localed)
		{
			Vector3f offset;
			READ_DATA(&offset, sizeof(Vector3f));
			//subModel->m_offset = offset;
		}

		//subModel->vertexStart = vtxStart;
		//subModel->vertexEnd = vtxStart + vtxCount - 1;
		//subModel->indexStart = idxStart;
		//subModel->indexCount = idxCount;

		if (vtxCount > 0 && idxCount > 0)
		{
			//model->addSubModel(subModel);
		}
		else
		{
			//safeDelete(subModel);
		}

		subModelIdx++;
		dataSize -= subDataLen;
	}
	return true;
}

END_TEE