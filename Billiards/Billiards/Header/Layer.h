#pragma once
#include <vector>
#include <map>
#include <string>

using namespace std;

class LayerSetting
{
private:
	map<string, int> layerNoList;
	vector<vector<bool>> layer;
protected:
	LayerSetting();
public:
	virtual ~LayerSetting();

	static LayerSetting& Instance()
	{
		static LayerSetting inst;
		return inst;
	}

	void AddLayer(const string& layerName);

	void SetRelationship(const string& tag1, const string& tag2, bool flg);

	bool CheckRelationship(const string& tag1, const string& tag2);
};