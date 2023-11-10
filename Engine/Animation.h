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
	Vector3 FramePosition;
	Vector3 FrameScale;
	Quaternion FrameQuaternion;
	float FrameTime;
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
	float AnimationSpeed = 1.f;

	~Animation()
	{
		for(auto& animation : NodeAnimations)
		{
			SAFE_DELETE(animation);
		}
		NodeAnimations.clear();
	}
};

  