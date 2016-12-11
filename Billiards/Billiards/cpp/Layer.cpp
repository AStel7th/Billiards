#include "../Header/Layer.h"

LayerSetting::LayerSetting()
{
	AddLayer("None");
	AddLayer("Ball");
	AddLayer("Table");
	AddLayer("Cues");
	AddLayer("Pocket");

	SetRelationship("Table", "Cues", false);
	SetRelationship("Table", "Pocket", false);
	SetRelationship("Cues", "Pocket", false);
}

LayerSetting::~LayerSetting()
{
}

void LayerSetting::AddLayer(const string & layerName)
{
	auto it = layerNoList.find(layerName);

	if (it != layerNoList.end())
		return;

	layerNoList[layerName] = layerNoList.size();

	vector<bool> temp;
	temp.reserve(layerNoList.size());

	for (unsigned int i = 0; i < layerNoList.size(); ++i)
	{
		temp.push_back(true);
	}

	for (auto&& elem : layer)
	{
		elem.push_back(true);
	}

	layer.push_back(temp);
}

void LayerSetting::SetRelationship(const string& tag1, const string& tag2, bool flg)
{
	if (auto it = layerNoList.find(tag1) == layerNoList.end())
		return;

	if (auto it = layerNoList.find(tag2) == layerNoList.end())
		return;

	layer[layerNoList[tag1]][layerNoList[tag2]] = flg;
	layer[layerNoList[tag2]][layerNoList[tag1]] = flg;
}

bool LayerSetting::CheckRelationship(const string & tag1, const string & tag2)
{
	return layer[layerNoList[tag1]][layerNoList[tag2]];
}

