#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>

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
};

struct Animation
{
	std::string	AnimationName;
	float AnimationDuration;
	std::vector<NodeAnimation*> NodeAnimations;
};

  