#pragma once
#include <istream>
#include "Face.h"

Face::Face(std::istream& issLine)
{
	vertex_indices_  = { 0,0,0 };
	normal_indices_  = { 0,0,0 };
	texture_indices_ = { 0,0,0 };

	char c;
	for (int i = 0; i < 3; i++)
	{
		issLine >> std::ws >> vertex_indices_.at(i) >> std::ws;
		if (issLine.peek() != '/')
		{
			continue;
		}
		
		issLine >> c >> std::ws;
		if (issLine.peek() == '/')
		{
			issLine >> c >> std::ws >> normal_indices_.at(i);
			continue;
		}

		issLine >> texture_indices_.at(i);
		if (issLine.peek() != '/')
		{
			continue;
		}
		
		issLine >> c >> normal_indices_.at(i);
	}
	rand_color_[0] = (float)rand()/RAND_MAX;
	rand_color_[1] = (float)rand()/RAND_MAX;
	rand_color_[2] = (float)rand()/RAND_MAX;
}

int Face::GetVertexIndex(int internal_index) const
{
	return vertex_indices_[internal_index];
}

int Face::GetNormalIndex(int internal_index) const
{
	return normal_indices_[internal_index];
}

int Face::GetTextureIndex(int internal_index) const
{
	return texture_indices_[internal_index];
}

const glm::vec3 &Face::GetRandColor() const {
	return rand_color_;
}

