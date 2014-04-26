#include "D3DShaderConstantTable.h"

CD3DShaderConstantTable::CD3DShaderConstantTable()
{

}

CD3DShaderConstantTable::CD3DShaderConstantTable(Framework::CStream& stream)
{
	Read(stream);
}

CD3DShaderConstantTable::~CD3DShaderConstantTable()
{

}

const CD3DShaderConstantTable::ConstantArray& CD3DShaderConstantTable::GetConstants() const
{
	return m_constants;
}

void CD3DShaderConstantTable::Read(Framework::CStream& stream)
{
	stream.Read(&m_header, sizeof(D3DCONSTANTTABLE));

	m_constants.resize(m_header.constants);

	stream.Seek(m_header.constantInfos, Framework::STREAM_SEEK_SET);
	for(auto& constant : m_constants)
	{
		stream.Read(&constant.info, sizeof(D3DCONSTANTINFO));
	}

	for(auto& constant : m_constants)
	{
		stream.Seek(constant.info.name, Framework::STREAM_SEEK_SET);
		constant.name = stream.ReadString();
		stream.Seek(constant.info.typeInfo, Framework::STREAM_SEEK_SET);
		stream.Read(&constant.typeInfo, sizeof(D3DTYPEINFO));
	}
}
