#pragma once

BEGIN_TEE

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

class MZLoader
{
public:

	static void load();

protected:

	static bool readMzVersion(std::ifstream& fin, uint dataSize);

	static bool readMzSharedGeometry(std::ifstream& fin, uint dataSize);

	static bool readMzGeometry(std::ifstream& fin, uint dataSize);

private:
};

END_TEE