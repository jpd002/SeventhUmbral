#pragma once

#include "BaseChunk.h"

class CPgrpChunk : public CBaseChunk
{
public:
	typedef std::vector<uint16> TriangleArray;

							CPgrpChunk();
	virtual					~CPgrpChunk();

	virtual void			Read(Framework::CStream&) override;

	const TriangleArray&	GetTriangles() const;
	std::string				GetName() const;

private:
	std::string				m_name;
	TriangleArray			m_triangles;
};
