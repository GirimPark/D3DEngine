#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

class Model;

/// 애니메이션 관련 정보
///	기능 없는 구조체로 두고, Node를 탐색하면서 해당 노드 안에서 처리하도록 하자

struct FrameKey
{
	Matrix FrameTransform;
	float FrameDuration;
};

struct NodeAnimation
{
	std::string NodeName;
	std::vector<FrameKey> Frames;
};

struct Animation
{
	std::string	AnimationName;
	float AnimationDuration;
	std::vector<NodeAnimation*> NodeAnimations;
};

  