#include "stdafx.h"
#include "MZLoader.h"
#include "FileSystem.h"

BEGIN_TEE

using namespace Glyph3;

#define READ_DATA(addres,size) fin.read((char*)(addres), size);

void MZLoader::load()
{
	FileSystem fileSys;
	std::wstring fileName = L"E:\\hieroglyph3\\TEE\\Bin\\Models\\player_zsm.Mz";// fileSys.GetModelsFolder() + L"player_zsm.Mz";

	std::ifstream fin;

	fin.open(fileName.c_str(), std::ios::binary);

	//计算文件大小
	fin.seekg(0, SEEK_END);
	size_t fileSize = fin.tellg();
	fin.setstate(0, SEEK_SET);

	uint chunkType;
	uint chunkSize;
	//fin.read((char*)(&chunkType), sizeof(chunkType));
	//fin.read((char*)(&chunkSize), sizeof(chunkSize));

	READ_DATA(&chunkType, sizeof(chunkType));
	READ_DATA(&chunkSize, sizeof(chunkSize));

	if (chunkType == 'MVER')
	{
		uint ver = 0;
		//pStream->read(&ver, sizeof(ver));
		READ_DATA(&ver, sizeof(ver));
		//pStream->seek(0, SEEK_SET);
		
		
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
				//ret = readMzSharedGeometry(model, stream, chunkSize);
				break;
			case 'MGEO': // 图元信息，代替原来的公共顶点和公共面片信息，实际上是私有的子模型
				//ret = readMzGeometry(model, stream, chunkSize);
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
		}
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

END_TEE