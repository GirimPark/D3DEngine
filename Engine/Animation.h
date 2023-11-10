#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

#include "Helper.h"

using namespace DirectX::SimpleMath;

class Model;

/// 애니메이션 관련 정보
///	기능 없는 구조체로 두고, Node를 탐색하면서 해당 노드 안에서 처리하도록 하자

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

  