#include "UmbralMesh.h"
#include "../ResourceManager.h"
#include "PtrStream.h"
#include "StdStream.h"
#include "D3DShaderDisassembler.h"

//#define _USE_GAME_SHADERS

#ifdef _USE_GAME_SHADERS
#include "Dx11UmbralEffectProvider.h"
#include "athena/win32/Dx11GraphicDevice.h"
#endif

CUmbralMesh::TextureMap CUmbralMesh::m_textures;

static uint16 ByteSwap16(uint16 value)
{
	return
		(((value & 0xFF00) >>  8) <<  0) |
		(((value & 0x00FF) >>  0) <<  8);
}

static float HalfToFloat(uint16 half)
{
	uint32 result = 0;
	if(half & 0x8000) result |= 0x80000000;
	int8 exponent = ((half >> 10) & 0x1F);
	//Flush denormal to zero
	if(exponent == 0) return *reinterpret_cast<float*>(&result);
	exponent -= 0xF;	//Convert to absolute exponent
	exponent += 0x7F;	//Then to float exponent
	uint32 mantissa = (half & 0x3FF);
	result |= exponent << 23;
	result |= mantissa << 13;
	return *reinterpret_cast<float*>(&result);
}

static CVector2 ConvertVec2FromHalf(const uint8* rawData)
{
	const uint16* data = reinterpret_cast<const uint16*>(rawData);
	CVector2 result;
	result.x = HalfToFloat(ByteSwap16(data[0]));
	result.y = HalfToFloat(ByteSwap16(data[1]));
	return result;
}

static CVector3 ConvertVec3FromUint8(const uint8* rawData)
{
	CVector3 result;
	result.x = static_cast<float>(rawData[0] / 255.f);
	result.y = static_cast<float>(rawData[1] / 255.f);
	result.z = static_cast<float>(rawData[2] / 255.f);
	return result;
}

static CVector3 ConvertVec3FromInt16(const uint8* rawData)
{
	const int16* data = reinterpret_cast<const int16*>(rawData);
	CVector3 result;
	result.x = static_cast<float>(static_cast<int16>(ByteSwap16(data[0]))) / 32768.f;
	result.y = static_cast<float>(static_cast<int16>(ByteSwap16(data[1]))) / 32768.f;
	result.z = static_cast<float>(static_cast<int16>(ByteSwap16(data[2]))) / 32768.f;
	return result;
}

static CVector3 ConvertVec3FromUint16(const uint8* rawData)
{
	const uint16* data = reinterpret_cast<const uint16*>(rawData);
	CVector3 result;
	result.x = static_cast<float>(ByteSwap16(data[0])) / 65535.f;
	result.y = static_cast<float>(ByteSwap16(data[1])) / 65535.f;
	result.z = static_cast<float>(ByteSwap16(data[2])) / 65535.f;
	return result;
}

CUmbralMesh::CUmbralMesh(const MeshChunkPtr& meshChunk, const ShaderSectionPtr& shaderSection)
: m_shaderSection(shaderSection)
{
	SetupGeometry(meshChunk);
	SetupEffect();
	SetupTextures();
}

CUmbralMesh::~CUmbralMesh()
{

}

void CUmbralMesh::SetLocalTexture(const ResourceNodePtr& texture)
{
	m_localTexture = texture;
	SetupTextures();
}

void CUmbralMesh::SetupGeometry(const MeshChunkPtr& meshChunk)
{
	auto streamChunks = meshChunk->SelectNodes<CStreamChunk>();
	assert(streamChunks.size() == 2);
	auto indexStream = streamChunks[0];
	auto vertexStream = streamChunks[1];

	uint32 indexCount = indexStream->GetVertexCount();
	uint32 vertexCount = vertexStream->GetVertexCount();

	auto positionElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_POSITION);
	auto normalElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_NORMAL);
	auto uv1Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV1);
	auto colorElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_COLOR);
	assert(positionElement != nullptr);

	uint32 vertexFlags = Athena::VERTEX_BUFFER_HAS_POS;
	if(normalElement != nullptr) vertexFlags |= Athena::VERTEX_BUFFER_HAS_NRM;
	if(uv1Element != nullptr) vertexFlags |= Athena::VERTEX_BUFFER_HAS_UV0;
	if(colorElement != nullptr) vertexFlags |= Athena::VERTEX_BUFFER_HAS_COLOR;

	auto bufferDesc = Athena::GenerateVertexBufferDescriptor(vertexCount, indexCount, vertexFlags);

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
		assert(positionElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_INT16);
		assert(!normalElement || normalElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_BYTE);
		assert(!colorElement || colorElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_BYTE);
		assert(!uv1Element || uv1Element->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_HALF);
		const uint8* srcVertices = vertexStream->GetData();
		uint8* dstVertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
		for(unsigned int i = 0; i < vertexCount; i++)
		{
			auto position = ConvertVec3FromInt16(srcVertices + positionElement->offsetInVertex);
			*reinterpret_cast<CVector3*>(dstVertices + bufferDesc.posOffset) = position;
			if(normalElement)
			{
				auto normal = ConvertVec3FromUint8(srcVertices + normalElement->offsetInVertex);
				*reinterpret_cast<CVector3*>(dstVertices + bufferDesc.nrmOffset) = normal;
			}
			if(uv1Element)
			{
				auto uv1 = ConvertVec2FromHalf(srcVertices + uv1Element->offsetInVertex);
				*reinterpret_cast<CVector2*>(dstVertices + bufferDesc.uv0Offset) = uv1;
			}
			if(colorElement)
			{
				*reinterpret_cast<uint8*>(dstVertices + bufferDesc.colorOffset + 0) = *(srcVertices + colorElement->offsetInVertex + 0);
				*reinterpret_cast<uint8*>(dstVertices + bufferDesc.colorOffset + 1) = *(srcVertices + colorElement->offsetInVertex + 1);
				*reinterpret_cast<uint8*>(dstVertices + bufferDesc.colorOffset + 2) = *(srcVertices + colorElement->offsetInVertex + 2);
				*reinterpret_cast<uint8*>(dstVertices + bufferDesc.colorOffset + 3) = *(srcVertices + colorElement->offsetInVertex + 3);
			}
			srcVertices += vertexStream->GetVertexSize();
			dstVertices += bufferDesc.GetVertexSize();
		}
		m_vertexBuffer->UnlockVertices();
	}
}

void CUmbralMesh::SetupEffect()
{
#ifdef _USE_GAME_SHADERS
	FileChunkPtr vertexShaderFile;
	FileChunkPtr pixelShaderFile;
	auto fileChunks = m_shaderSection->SelectNodes<CFileChunk>();
	for(const auto& fileChunk : fileChunks)
	{
		auto fileName = fileChunk->GetName();
		if(!vertexShaderFile && fileName.find(".vpo") != std::string::npos)
		{
			vertexShaderFile = fileChunk;
		}
		if(!pixelShaderFile && fileName.find(".fpo") != std::string::npos)
		{
			pixelShaderFile = fileChunk;
		}
	}

	assert(vertexShaderFile && pixelShaderFile);

	auto makeShaderStreamFromFile = 
		[] (const FileChunkPtr& file)
		{
			const auto compiledShader = file->GetCompiledShader();
			uint32 compiledShaderLength = file->GetCompiledShaderLength();
			return Framework::CPtrStream(compiledShader, compiledShaderLength);
		};

	Framework::CPtrStream vertexShaderStream = makeShaderStreamFromFile(vertexShaderFile);
	Framework::CPtrStream pixelShaderStream = makeShaderStreamFromFile(pixelShaderFile);

//	CD3DShader vertexShader(Framework::CStdStream("D:\\Projects\\SeventhUmbral\\tools\\WorldEditor\\data\\standard.vso", "rb"));
//	CD3DShader pixelShader(Framework::CStdStream("D:\\Projects\\SeventhUmbral\\tools\\WorldEditor\\data\\standard.pso", "rb"));

	CD3DShader vertexShader(vertexShaderStream);
	CD3DShader pixelShader(pixelShaderStream);

#if 0
	auto shaderType = vertexShader.GetType();
	std::string shaderCode;
	for(const auto& instruction : vertexShader.GetInstructions())
	{
		auto mnemonic = CD3DShaderDisassembler::GetInstructionMnemonic(shaderType, instruction);
		auto operands = CD3DShaderDisassembler::GetInstructionOperands(shaderType, instruction);
		shaderCode += string_format("%s %s\r\n", mnemonic.c_str(), operands.c_str());
	}
#endif

	auto& graphicDevice = static_cast<Athena::CDx11GraphicDevice&>(Athena::CGraphicDevice::GetInstance());
	SetEffectProvider(std::make_shared<CDx11UmbralEffectProvider>(graphicDevice.GetDevice(), graphicDevice.GetDeviceContext(), vertexShader, pixelShader));
#endif
}

void CUmbralMesh::SetupTextures()
{
	auto pramChunk = m_shaderSection->SelectNode<CPramChunk>();
	assert(pramChunk);
	Athena::TexturePtr texture;
	auto localTextureSections = m_localTexture ? m_localTexture->SelectNodes<CTextureSection>() : decltype(m_localTexture->SelectNodes<CTextureSection>())();
	for(const auto& sampler : pramChunk->GetSamplers())
	{
		if(sampler.name == "_sampler_00")
		{
			for(const auto& string : sampler.strings)
			{
				texture = GetTexture(string);
				if(texture) break;
				for(const auto& textureSection : localTextureSections)
				{
					const auto& sectionName = textureSection->GetResourceId();
					if(string.find(sectionName) != std::string::npos)
					{
						auto textureDataInfo = textureSection->SelectNode<CGtexData>();
						texture = CreateTextureFromGtex(textureDataInfo);
						break;
					}
				}
				if(texture) break;
			}
			break;
		}
	}

	GetMaterial()->SetTexture(0, texture);
	GetMaterial()->SetTextureAddressModeU(0, Athena::TEXTURE_ADDRESS_REPEAT);
	GetMaterial()->SetTextureAddressModeV(0, Athena::TEXTURE_ADDRESS_REPEAT);
}

Athena::TexturePtr CUmbralMesh::GetTexture(const std::string& textureName)
{
	auto textureIterator = m_textures.find(textureName);
	if(textureIterator != std::end(m_textures)) return textureIterator->second;

	auto textureResource = CResourceManager::GetInstance().GetResource(textureName);
	if(!textureResource) return Athena::TexturePtr();

	auto textureDataInfo = textureResource->SelectNode<CGtexData>();
	assert(textureDataInfo);

	auto texture = CreateTextureFromGtex(textureDataInfo);

	m_textures.insert(std::make_pair(textureName, texture));
	return texture;
}

Athena::TexturePtr CUmbralMesh::CreateTextureFromGtex(const GtexDataPtr& textureDataInfo)
{
	auto textureFormat = textureDataInfo->GetTextureFormat();
	auto textureWidth = textureDataInfo->GetTextureWidth();
	auto textureHeight = textureDataInfo->GetTextureHeight();
	auto textureData = textureDataInfo->GetTextureData();
	Athena::TEXTURE_FORMAT specTextureFormat = Athena::TEXTURE_FORMAT_UNKNOWN;
	switch(textureFormat)
	{
	case CGtexData::TEXTURE_FORMAT_DXT1:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT1;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT3:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT3;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT5:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT5;
		break;
	case CGtexData::TEXTURE_FORMAT_A8R8G8B8:
	case CGtexData::TEXTURE_FORMAT_X8R8G8B8:
		specTextureFormat = Athena::TEXTURE_FORMAT_RGBA8888;
		break;
	default:
		assert(0);
		break;
	}

	auto texture = Athena::CGraphicDevice::GetInstance().CreateTextureFromRawData(textureData, specTextureFormat, textureWidth, textureHeight);
	return texture;
}
