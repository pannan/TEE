#pragma once
#include "KeyFrames.h"
#include "Vector3f.h"
#include "Quaternion.h"
#include "Matrix4f.h"

BEGIN_TEE

struct ModelKeyframeTranslation
{
	int time;			/// 时刻
	float v[3];			/// 平移量
};

/// 关键帧旋转参数
struct ModelKeyframeRotation
{
	int time;			/// 时刻
	float q[4];			/// 旋转量
};

/// 关键帧缩放参数
struct ModelKeyframeScale
{
	int time;			/// 时刻
	float v[3];			/// 缩放量
};

struct BoneData
{
	size_t getMemorySize()
	{
		return sizeof(*this) + translation.getMemorySize() + rotation.getMemorySize() + scale.getMemorySize();
	}

	BoneData()
		:initialMatrix(Glyph3::Matrix4f::Identity())
		, initialMatrixInverse(Glyph3::Matrix4f::Identity())
	{
		objectId = -1;
		parentId = -1;

		translation.setStaticData(Glyph3::Vector3f(0.0f, 0.0f, 0.0f));
		rotation.setStaticData(Glyph3::Quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f));
		scale.setStaticData(Glyph3::Vector3f(1.0f, 1.0f, 1.0f));
		flags = 0;

		pivotPoint = Glyph3::Vector3f(0.0f, 0.0f, 0.0f);
	}

	//骨骼位于初始状态的矩阵，位移部分跟pivotPoint一致，为了方便起见，保留pivotPoint成员
	//此矩阵也就是Maya中的骨骼初始状态下的inclusiveMatrix
	Glyph3::Matrix4f	initialMatrix;
	Glyph3::Matrix4f	initialMatrixInverse;

	//tchar name[MODEL_NAME_NODE_SIZE + 1];	/// 骨骼名
	std::string m_name;

	int objectId;							/// 骨骼ID
	int parentId;							/// 父骨骼ID

	KeyFrames<Glyph3::Vector3f> translation;			/// 平移关键帧
	KeyFrames<Glyph3::Quaternion<float>> rotation;			/// 旋转关键帧
	KeyFrames<Glyph3::Vector3f> scale;				/// 缩放关键帧
	Glyph3::Vector3f pivotPoint;						/// 轴点

	uint flags;                             /// 骨点的标记信息
	enum BDFlags
	{
		BDF_DontInheritTranslation = 0x0001,   /// 不继承平移
		BDF_DontInheritRotation = 0x0002,   /// 不继承旋转
		BDF_DontInheritScaling = 0x0004,   /// 不继承缩放
		BDF_Billboarded = 0x0008,   /// 有公告板
		BDF_BillboardedLockX = 0x0010,
		BDF_BillboardedLockY = 0x0020,
		BDF_BillboardedLockZ = 0x0040,
		BDF_CameraAnchored = 0x0080,   /// 是否计算相机锚点
		BDF_PivotRotation = 0x0100,
	};
};

enum VertexElementType
{
	VET_None = -1,
	VET_Float1 = 0,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_Color,
	VET_UByte4,
	VET_Short2,
	VET_Short4,
	VET_UByte4N,
	VET_Short2N,			// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Short4N,
	VET_PackedNormal,		// FPackedNormal
	VET_Half2 = 15,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4 = 16,			//这么做是为了和D3D中的一致，不用转换，OpenGL就需要转一下，但至少不用2个都转
	VET_MAX
};

struct VertexElement
{
	BYTE StreamIndex;
	BYTE Offset;
	BYTE Type;
	BYTE Usage;
	BYTE UsageIndex;
	bool bUseInstanceIndex;
	uint NumVerticesPerInstance;

	VertexElement() {}
	VertexElement(BYTE InStreamIndex, BYTE InOffset, BYTE InType, BYTE InUsage, BYTE InUsageIndex, bool bInUseInstanceIndex = false, uint InNumVerticesPerInstance = 0) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		Usage(InUsage),
		UsageIndex(InUsageIndex),
		bUseInstanceIndex(bInUseInstanceIndex),
		NumVerticesPerInstance(InNumVerticesPerInstance)
	{}
	/**
	* Suppress the compiler generated assignment operator so that padding won't be copied.
	* This is necessary to get expected results for code that zeros, assigns and then CRC's the whole struct.
	*/
	void operator=(const VertexElement& Other)
	{
		StreamIndex = Other.StreamIndex;
		Offset = Other.Offset;
		Type = Other.Type;
		Usage = Other.Usage;
		UsageIndex = Other.UsageIndex;
		bUseInstanceIndex = Other.bUseInstanceIndex;
		NumVerticesPerInstance = Other.NumVerticesPerInstance;
	}

	int getSize() const
	{
		switch (Type)
		{
		case VET_Float1:
		case VET_Color:
		case VET_UByte4:
		case VET_Short2:
		case VET_UByte4N:
		case VET_Short2N:
		case VET_PackedNormal:
		case VET_Half2:
			return 4;
		case VET_Float2:
		case VET_Short4:
		case VET_Short4N:
		case VET_Half4:
			return 8;
		case VET_Float3:
			return 12;
		case VET_Float4:
			return 16;
		default:
			return 0;
		}
	}
};

enum IndexType
{
	IT_16BIT,
	IT_32BIT,
	IT_OFFSET = 0x10
};

END_TEE