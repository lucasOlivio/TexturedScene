#pragma once

#include "scene/SceneView.h"
#include <string>
#include <rapidjson/document.h>

class SceneParserJSON
{
public:
	SceneParserJSON();
	~SceneParserJSON();

	bool ParseFromJsonObj(rapidjson::Value& jsonObject, iScene* pSceneOut);
	bool ParseToJsonObj(SceneView* pScene,
						rapidjson::Value& jsonObjectOut,
						rapidjson::Document::AllocatorType& allocator);
};