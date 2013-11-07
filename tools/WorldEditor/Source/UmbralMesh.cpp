#include "UmbralMesh.h"
#include "StreamChunk.h"

static uint16 ByteSwap16(uint16 value)
{
	return
		(((value & 0xFF00) >>  8) <<  0) |
		(((value & 0x00FF) >>  0) <<  8);
}

static CVector3 ConvertFromInt16(const uint8* rawData)
{
	const int16* data = reinterpret_cast<const int16*>(rawData);
	CVector3 result;
	result.x = static_cast<float>(static_cast<int16>(ByteSwap16(data[0]))) / 32768.f;
	result.y = static_cast<float>(static_cast<int16>(ByteSwap16(data[1]))) / 32768.f;
	result.z = static_cast<float>(static_cast<int16>(ByteSwap16(data[2]))) / 32768.f;
	return result;
}

static CVector3 ConvertFromUint16(const uint8* rawData)
{
	const uint16* data = reinterpret_cast<const uint16*>(rawData);
	CVector3 result;
	result.x = static_cast<float>(ByteSwap16(data[0])) / 65535.f;
	result.y = static_cast<float>(ByteSwap16(data[1])) / 65535.f;
	result.z = static_cast<float>(ByteSwap16(data[2])) / 65535.f;
	return result;
}

CUmbralMesh::CUmbralMesh(const MeshChunkPtr& meshChunk)
{
	auto streamChunks = meshChunk->SelectNodes<CStreamChunk>();
	assert(streamChunks.size() == 2);
	auto indexStream = streamChunks[0];
	auto vertexStream = streamChunks[1];

	uint32 indexCount = indexStream->GetVertexCount();
	uint32 vertexCount = vertexStream->GetVertexCount();

	auto bufferDesc = Athena::GenerateVertexBufferDescriptor(vertexCount, indexCount, Athena::VERTEX_BUFFER_HAS_POS);

	m_primitiveType = Athena::PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = indexCount / 3;
	m_vertexBuffer = Athena::CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	{
		const uint16* srcIndices = reinterpret_cast<const uint16*>(indexStream->GetData());
		uint16* dstIndices = m_vertexBuffer->LockIndices();
		for(unsigned int i = 0; i < indexCount; i++)
		{
			dstIndices[i] = ByteSwap16(srcIndices[i]);
		}
		m_vertexBuffer->UnlockIndices();
	}

	{
		auto positionElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_POSITION);
		assert(positionElement != nullptr);
		assert(positionElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_INT16);
		const uint8* srcVertices = vertexStream->GetData();
		uint8* dstVertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
		for(unsigned int i = 0; i < vertexCount; i++)
		{
			auto position = ConvertFromInt16(srcVertices + positionElement->offsetInVertex);
			*reinterpret_cast<CVector3*>(dstVertices + bufferDesc.posOffset) = position;
			srcVertices += vertexStream->GetVertexSize();
			dstVertices += bufferDesc.GetVertexSize();
		}
		m_vertexBuffer->UnlockVertices();
	}
}

CUmbralMesh::~CUmbralMesh()
{

}