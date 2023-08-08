#pragma once
#include "graphicscontrol_global.h"
class Node
{
public:
	std::string key;
	//value
	bool hasChild();
	bool hasParent();
	Node* GetParent();
	Node* GetChild();
private:
	bool m_hasParent;
	bool m_hasChild;
	Node* m_pParent;
	Node* m_pChild;
};

