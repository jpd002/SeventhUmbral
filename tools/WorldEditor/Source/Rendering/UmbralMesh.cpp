#include "UmbralMesh.h"
#include "../ResourceManager.h"
#include "GlobalResources.h"
#include "PtrStream.h"
#include "StdStream.h"
#include "D3DShaderDisassembler.h"
#include "Dx11UmbralEffect.h"
#include "Dx11UmbralEffectProvider.h"

#define _USE_GAME_SHADERS

#ifdef _USE_GAME_SHADERS
#include "string_format.h"
#endif

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
	uint8 exponent = ((half >> 10) & 0x1F);
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

static CVector4 ConvertVec4FromUint8(const uint8* rawData)
{
	CVector4 result;
	result.x = static_cast<float>(rawData[0] / 255.f);
	result.y = static_cast<float>(rawData[1] / 255.f);
	result.z = static_cast<float>(rawData[2] / 255.f);
	result.w = static_cast<float>(rawData[3] / 255.f);
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

Athena::EffectPtr CUmbralMesh::GetEffect() const
{
	return m_effect;
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

	auto bufferDesc = GenerateVertexBufferDescriptor(vertexStream, indexStream);

	auto positionElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_POSITION);
	auto normalElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_NORMAL);
	auto uv1Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV1);
	auto uv2Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV2);
	auto uv3Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV3);
	auto colorElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_COLOR);
	auto tangentElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_TANGENT);
	assert(positionElement != nullptr);

	assert(positionElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_INT16);
	assert(!normalElement || normalElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_BYTE);
	assert(!colorElement || colorElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_BYTE);
	assert(!uv1Element || uv1Element->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_HALF);
	assert(!uv2Element || uv2Element->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_HALF);
	assert(!uv3Element || uv3Element->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_HALF);
	assert(!tangentElement || tangentElement->dataFormat == CStreamChunk::ELEMENT_DATA_FORMAT_BYTE);

	m_primitiveType = Athena::PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = indexCount / 3;
	m_vertexBuffer = Athena::CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);
	const auto& posVertexItem = bufferDesc.GetVertexItem(Athena::VERTEX_ITEM_ID_POSITION);
	const auto& nrmVertexItem = bufferDesc.GetVertexItem(Athena::VERTEX_ITEM_ID_NORMAL);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(Athena::VERTEX_ITEM_ID_UV0);
	const auto& uv1VertexItem = bufferDesc.GetVertexItem(Athena::VERTEX_ITEM_ID_UV1);
	const auto& uv2VertexItem = bufferDesc.GetVertexItem(CDx11UmbralEffect::VERTEX_ITEM_ID_UV2);
	const auto& colorVertexItem = bufferDesc.GetVertexItem(Athena::VERTEX_ITEM_ID_COLOR);
	const auto& tangentVertexItem = bufferDesc.GetVertexItem(CDx11UmbralEffect::VERTEX_ITEM_ID_TANGENT);

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
		const uint8* srcVertices = vertexStream->GetData();
		uint8* dstVertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
		for(unsigned int i = 0; i < vertexCount; i++)
		{
			auto position = ConvertVec3FromInt16(srcVertices + positionElement->offsetInVertex);
			*reinterpret_cast<CVector3*>(dstVertices + posVertexItem->offset) = position;
			if(normalElement)
			{
				auto normal = ConvertVec3FromUint8(srcVertices + normalElement->offsetInVertex);
				*reinterpret_cast<CVector3*>(dstVertices + nrmVertexItem->offset) = normal;
			}
			if(uv1Element)
			{
				auto uv1 = ConvertVec2FromHalf(srcVertices + uv1Element->offsetInVertex);
				*reinterpret_cast<CVector2*>(dstVertices + uv0VertexItem->offset) = uv1;
			}
			if(uv2Element)
			{
				auto uv2 = ConvertVec2FromHalf(srcVertices + uv2Element->offsetInVertex);
				*reinterpret_cast<CVector2*>(dstVertices + uv1VertexItem->offset) = uv2;
			}
			if(uv3Element)
			{
				auto uv3 = ConvertVec2FromHalf(srcVertices + uv3Element->offsetInVertex);
				*reinterpret_cast<CVector2*>(dstVertices + uv2VertexItem->offset) = uv3;
			}
			if(colorElement)
			{
				*reinterpret_cast<uint8*>(dstVertices + colorVertexItem->offset + 0) = *(srcVertices + colorElement->offsetInVertex + 0);
				*reinterpret_cast<uint8*>(dstVertices + colorVertexItem->offset + 1) = *(srcVertices + colorElement->offsetInVertex + 1);
				*reinterpret_cast<uint8*>(dstVertices + colorVertexItem->offset + 2) = *(srcVertices + colorElement->offsetInVertex + 2);
				*reinterpret_cast<uint8*>(dstVertices + colorVertexItem->offset + 3) = *(srcVertices + colorElement->offsetInVertex + 3);
			}
			if(tangentElement)
			{
				auto tangent = ConvertVec4FromUint8(srcVertices + tangentElement->offsetInVertex);
				*reinterpret_cast<CVector4*>(dstVertices + tangentVertexItem->offset) = tangent;
			}
			srcVertices += vertexStream->GetVertexSize();
			dstVertices += bufferDesc.GetVertexSize();
		}
		m_vertexBuffer->UnlockVertices();
	}
}

Athena::VERTEX_BUFFER_DESCRIPTOR CUmbralMesh::GenerateVertexBufferDescriptor(const StreamChunkPtr& vertexStream, const StreamChunkPtr& indexStream)
{
	uint32 currentOffset = 0;
	unsigned int currentVertexItem = 0;

	auto positionElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_POSITION);
	auto normalElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_NORMAL);
	auto uv1Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV1);
	auto uv2Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV2);
	auto uv3Element = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_UV3);
	auto colorElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_COLOR);
	auto tangentElement = vertexStream->FindElement(CStreamChunk::ELEMENT_DATA_TYPE_TANGENT);
	assert(positionElement != nullptr);

	Athena::VERTEX_BUFFER_DESCRIPTOR result;
	result.vertexCount	= vertexStream->GetVertexCount();
	result.indexCount	= indexStream->GetVertexCount();

	if(positionElement)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = Athena::VERTEX_ITEM_ID_POSITION;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector3);
		currentOffset += vertexItem.size;
	}

	if(normalElement)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = Athena::VERTEX_ITEM_ID_NORMAL;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector3);
		currentOffset += vertexItem.size;
	}

	if(uv1Element)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = Athena::VERTEX_ITEM_ID_UV0;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector2);
		currentOffset += vertexItem.size;
	}

	if(uv2Element)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = Athena::VERTEX_ITEM_ID_UV1;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector2);
		currentOffset += vertexItem.size;
	}

	if(uv3Element)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = CDx11UmbralEffect::VERTEX_ITEM_ID_UV2;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector2);
		currentOffset += vertexItem.size;
	}

	if(colorElement)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = Athena::VERTEX_ITEM_ID_COLOR;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(uint32);
		currentOffset += vertexItem.size;
	}

	if(tangentElement)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = CDx11UmbralEffect::VERTEX_ITEM_ID_TANGENT;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector4);
		currentOffset += vertexItem.size;
	}

	return result;
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

	auto vertexShaderStream = makeShaderStreamFromFile(vertexShaderFile);
	auto pixelShaderStream = makeShaderStreamFromFile(pixelShaderFile);

//	CD3DShader vertexShader(Framework::CStdStream("D:\\Projects\\SeventhUmbral\\tools\\WorldEditor\\data\\standard.vso", "rb"));
//	CD3DShader pixelShader(Framework::CStdStream("D:\\Projects\\SeventhUmbral\\tools\\WorldEditor\\data\\standard.pso", "rb"));

	auto vertexShader = CD3DShader(vertexShaderStream);
	auto pixelShader = CD3DShader(pixelShaderStream);

#if 0
	auto shaderType = pixelShader.GetType();
	std::string shaderCode;
	for(const auto& instruction : pixelShader.GetInstructions())
	{
		auto mnemonic = CD3DShaderDisassembler::GetInstructionMnemonic(shaderType, instruction);
		auto operands = CD3DShaderDisassembler::GetInstructionOperands(shaderType, instruction);
		shaderCode += string_format("%s %s\r\n", mnemonic.c_str(), operands.c_str());
	}
#endif

	const auto& effectProvider = CGlobalResources::GetInstance().GetEffectProvider();
	m_effect = std::static_pointer_cast<CDx11UmbralEffectProvider>(effectProvider)->GetEffect(vertexShader, pixelShader);
	SetEffectProvider(effectProvider);
	
	const auto& pixelShaderConstantTable = pixelShader.GetConstantTable();
	for(const auto& constant : pixelShaderConstantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_SAMPLER) continue;
		m_samplerRegisters.insert(std::make_pair(constant.name, constant.info.registerIndex));
	}

	{
		auto material = GetMaterial();

		//material->SetAlphaBlendingMode(Athena::ALPHA_BLENDING_LERP);
		material->SetCullingMode(Athena::CULLING_CW);

		auto pramChunk = m_shaderSection->SelectNode<CPramChunk>();
		assert(pramChunk);

		//Copy parameters to effect parameters
		for(const auto& param : pramChunk->GetParameters())
		{
			Athena::CEffectParameter effectParam;
			switch(param.numValues)
			{
			case 1:
				effectParam.SetScalar(param.valueX);
				break;
			case 2:
				effectParam.SetVector2(CVector2(param.valueX, param.valueY));
				break;
			case 3:
				effectParam.SetVector3(CVector3(param.valueX, param.valueY, param.valueZ));
				break;
			case 4:
				effectParam.SetVector4(CVector4(param.valueX, param.valueY, param.valueZ, param.valueW));
				break;
			default:
				assert(0);
				break;
			}
			auto paramName = param.isPixelShaderParam ? ("ps_" + param.name) : ("vs_" + param.name);
			material->SetEffectParameter(paramName, effectParam);
		}
	}

#endif
}

void CUmbralMesh::SetupTextures()
{
	auto pramChunk = m_shaderSection->SelectNode<CPramChunk>();
	assert(pramChunk);
	
	auto localTextureSections = m_localTexture ? m_localTexture->SelectNodes<CTextureSection>() : decltype(m_localTexture->SelectNodes<CTextureSection>())();
	auto getTextureForSampler =
		[&localTextureSections] (const PramChunkPtr& pramChunk, const std::string& samplerName)
		{
			Athena::TexturePtr texture;
			for(const auto& sampler : pramChunk->GetSamplers())
			{
				if(sampler.name == samplerName)
				{
					for(const auto& string : sampler.strings)
					{
						texture = CGlobalResources::GetInstance().GetTexture(string);
						if(texture) break;
						for(const auto& textureSection : localTextureSections)
						{
							const auto& sectionName = textureSection->GetResourceId();
							if(string.find(sectionName) != std::string::npos)
							{
								auto textureDataInfo = textureSection->SelectNode<CGtexData>();
								texture = CGlobalResources::CreateTextureFromGtex(textureDataInfo);
								break;
							}
						}
						if(texture) break;
					}
				}
			}
			return texture;
		};

	auto sampler0 = getTextureForSampler(pramChunk, "_sampler_00");
	auto sampler1 = getTextureForSampler(pramChunk, "_sampler_01");
	auto sampler2 = getTextureForSampler(pramChunk, "_sampler_02");
	auto sampler3 = getTextureForSampler(pramChunk, "_sampler_03");
	auto sampler4 = getTextureForSampler(pramChunk, "_sampler_04");
	auto sampler5 = getTextureForSampler(pramChunk, "_sampler_05");
	auto sampler6 = getTextureForSampler(pramChunk, "_sampler_06");

	for(unsigned int i = 0; i < Athena::CMaterial::MAX_TEXTURE_SLOTS; i++)
	{
		GetMaterial()->SetTextureAddressModeU(i, Athena::TEXTURE_ADDRESS_REPEAT);
		GetMaterial()->SetTextureAddressModeV(i, Athena::TEXTURE_ADDRESS_REPEAT);
	}

#ifdef _USE_GAME_SHADERS
	auto setSampler =
		[&] (const std::string& samplerName, const Athena::TexturePtr& texture)
		{
			if(texture)
			{
				auto samplerRegisterIterator = m_samplerRegisters.find(samplerName);
				if(samplerRegisterIterator != std::end(m_samplerRegisters))
				{
					GetMaterial()->SetTexture(samplerRegisterIterator->second, texture);
				}
			}
		};

	setSampler("_sampler_00", sampler0);
	setSampler("_sampler_01", sampler1);
	setSampler("_sampler_02", sampler2);
	setSampler("_sampler_03", sampler3);
	setSampler("_sampler_04", sampler4);
	setSampler("_sampler_05", sampler5);
	setSampler("_sampler_06", sampler6);
	setSampler("lightDiffuseMap", CGlobalResources::GetInstance().GetDiffuseMapTexture());
	setSampler("reflectMap", CGlobalResources::GetInstance().GetSkyTexture());
	setSampler("shadowMap0", CGlobalResources::GetInstance().GetProxyShadowTexture());
#else
	GetMaterial()->SetTexture(0, sampler0);
#endif
}
