#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

#include "Helper.h"

using namespace DirectX::SimpleMath;

class Model;

/// �ִϸ��̼� ���� ����
///	��� ���� ����ü�� �ΰ�, Node�� Ž���ϸ鼭 �ش� ��� �ȿ��� ó���ϵ��� ����

struct FrameKey
{
	Matrix FrameTransform;
	float FrameDuration;
};

struct NodeAnimation
{
	std::string NodeName;
	std::vector<FrameKey> Frames;

	~NodeAnimation()
	{
		Frames.clear();
	}
};

struct Animation
{
	std::string	AnimationName;
	float AnimationDuration;
	std::vector<NodeAnimation*> NodeAnimations;

	~Animation()
	{
		for(auto& animation : NodeAnimations)
		{
			SAFE_DELETE(animation);
		}
		NodeAnimations.clear();
	}
};

  