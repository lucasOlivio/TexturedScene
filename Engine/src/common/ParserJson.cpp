#include "common/ParserJSON.h"
#include "common/utils.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>

ParserJSON::ParserJSON()
{
}

ParserJSON::~ParserJSON()
{
}

bool ParserJSON::GetString(rapidjson::Value& jsonObject, std::string& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsString()) {
        return false;
    }

    valueOut = jsonObject.GetString();
    return true;

}

bool ParserJSON::SetString(rapidjson::Value& jsonObject, const std::string& valueIn, rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;

    jsonObject.SetString(valueIn.c_str(), static_cast<SizeType>(valueIn.length()), allocator);

    return true;
}

bool ParserJSON::GetFloat(rapidjson::Value& jsonObject, float& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsFloat()) {
        return false;
    }

    valueOut = jsonObject.GetFloat();
    return true;
}

bool ParserJSON::SetFloat(rapidjson::Value& jsonObject, float valueIn)
{
    using namespace rapidjson;

    jsonObject.SetFloat(valueIn);

    return false;
}

bool ParserJSON::GetInt(rapidjson::Value& jsonObject, int& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsInt()){
        return false;
    }

    valueOut = jsonObject.GetInt();
    return true;

}

bool ParserJSON::SetInt(rapidjson::Value& jsonObject, int valueIn)
{
    using namespace rapidjson;

    jsonObject.SetInt(valueIn);

    return false;
}

bool ParserJSON::GetBool(rapidjson::Value& jsonObject, bool& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsBool()) {
        return false;
    }

    valueOut = jsonObject.GetBool();
    return true;

}

bool ParserJSON::SetBool(rapidjson::Value& jsonObject, bool valueIn)
{
    using namespace rapidjson;

    jsonObject.SetBool(valueIn);

    return true;
}

bool ParserJSON::GetVec4(rapidjson::Value& jsonObject, glm::vec4& valueOut)
{
    using namespace rapidjson;
    using namespace myutils;

    if (!jsonObject.IsArray() || jsonObject.Size() != 4) {
        return false;
    }

    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        if (!jsonObject[i].IsNumber()) {
            return false;
        }
    }

    valueOut = glm::vec4(
        jsonObject[0].GetFloat(),
        jsonObject[1].GetFloat(),
        jsonObject[2].GetFloat(),
        jsonObject[3].GetFloat()
    );

    return true;
}

bool ParserJSON::SetVec4(rapidjson::Value& jsonObject,
                         const glm::vec4& valueIn,
                         rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    using namespace myutils;

    jsonObject.SetArray();
    jsonObject.PushBack(valueIn.x, allocator);
    jsonObject.PushBack(valueIn.y, allocator);
    jsonObject.PushBack(valueIn.z, allocator);
    jsonObject.PushBack(valueIn.w, allocator);
    
    return true;
}

bool ParserJSON::GetVec3(rapidjson::Value& jsonObject, glm::vec3& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsArray() || jsonObject.Size() != 3) {
        return false;
    }

    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        if (!jsonObject[i].IsNumber()) {
            return false;
        }
    }

    valueOut = glm::vec3(
        jsonObject[0].GetFloat(),
        jsonObject[1].GetFloat(),
        jsonObject[2].GetFloat()
    );

    return true;
}

bool ParserJSON::SetVec3(rapidjson::Value& jsonObject,
                         const glm::vec3& valueIn,
                         rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    using namespace myutils;

    jsonObject.SetArray();
    jsonObject.PushBack(valueIn.x, allocator);
    jsonObject.PushBack(valueIn.y, allocator);
    jsonObject.PushBack(valueIn.z, allocator);
    
    return true;
}

bool ParserJSON::GetVecStr(rapidjson::Value& jsonObject, std::vector<std::string>& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsArray()) {
        return false;
    }
    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        if (!jsonObject[i].IsString()) {
            return false;
        }
    }


    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        valueOut.push_back(jsonObject[i].GetString());
    }

    return true;
}

bool ParserJSON::SetVecStr(rapidjson::Value& jsonObject,
                           const std::vector<std::string>& valueIn,
                           rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    using namespace myutils;

    jsonObject.SetArray();

    for (SizeType i = 0; i < valueIn.size(); i++) {
        Value strVal;
        strVal.SetString(valueIn[i].c_str(), allocator);
        jsonObject.PushBack(strVal, allocator);
    }

    return true;
}

bool ParserJSON::GetVecFloat(rapidjson::Value& jsonObject, std::vector<float>& valueOut)
{
    using namespace rapidjson;

    if (!jsonObject.IsArray()) {
        return false;
    }
    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        if (!jsonObject[i].IsFloat()) {
            return false;
        }
    }


    for (SizeType i = 0; i < jsonObject.Size(); i++) {
        valueOut.push_back(jsonObject[i].GetFloat());
    }

    return true;
}

bool ParserJSON::SetVecFloat(rapidjson::Value& jsonObject, const std::vector<float>& valueIn, rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    using namespace myutils;

    jsonObject.SetArray();

    for (SizeType i = 0; i < valueIn.size(); i++) {
        Value floatVal;
        floatVal.SetFloat(valueIn[i]);
        jsonObject.PushBack(floatVal, allocator);
    }

    return true;
}

bool ParserJSON::GetMap(rapidjson::Value& jsonObject, std::map<std::string, std::string>& valueOut)
{
    using namespace rapidjson;
    using namespace std;

    if (!jsonObject.IsObject()) {
        return false;
    }

    for (Value::ConstMemberIterator mapMember = jsonObject.MemberBegin(); mapMember != jsonObject.MemberEnd(); ++mapMember)
    {
        string key = mapMember->name.GetString();
        string val = mapMember->value.GetString();

        valueOut[key] = val;
    }

    return true;
}

bool ParserJSON::SetMap(rapidjson::Value& jsonObject,
            const std::map<std::string, std::string>& valueIn,
            rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    using namespace std;

    jsonObject.SetObject();

    for (pair<string, string> pairStr : valueIn)
    {
        Value key(pairStr.first.c_str(), allocator);
        Value val(pairStr.second.c_str(), allocator);

        jsonObject.AddMember(key, val, allocator);
    }

    return true;
}

bool ParserJSON::SetValue(rapidjson::Value& jsonObject,
                                   sParameterInfo& valueIn,
                                   rapidjson::Document::AllocatorType& allocator)
{
    // Discover parameter type by setting 
    if (valueIn.parameterType == "string")
    {
        SetString(jsonObject, valueIn.parameterStrValue, allocator);
        return true;
    }
    if (valueIn.parameterType == "float")
    {
        SetFloat(jsonObject, valueIn.parameterFloatValue);
        return true;
    }
    if (valueIn.parameterType == "int")
    {
        SetInt(jsonObject, valueIn.parameterIntValue);
        return true;
    }
    if (valueIn.parameterType == "bool")
    {
        SetBool(jsonObject, valueIn.parameterBoolValue);
        return true;
    }
    if (valueIn.parameterType == "vec3")
    {
        SetVec3(jsonObject, valueIn.parameterVec3Value, allocator);
        return true;
    }
    if (valueIn.parameterType == "vec4")
    {
        SetVec4(jsonObject, valueIn.parameterVec4Value, allocator);
        return true;
    }
    if (valueIn.parameterType == "vecFloat")
    {
        SetVecFloat(jsonObject, valueIn.parameterVecFloatValue, allocator);
        return true;
    }
    if (valueIn.parameterType == "vecStr")
    {
        SetVecStr(jsonObject, valueIn.parameterVecStrValue, allocator);
        return true;
    }
    if (valueIn.parameterType == "map")
    {
        SetMap(jsonObject, valueIn.parameterMapValue, allocator);
        return true;
    }
    // Value type not found
    return false;
}

bool ParserJSON::GetValue(rapidjson::Value& jsonObject, sParameterInfo& valueOut)
{
    // Discover parameter type by getting 
    if (GetString(jsonObject, valueOut.parameterStrValue))
    {
        valueOut.parameterType = "string";
        return true;
    }
    if (GetFloat(jsonObject, valueOut.parameterFloatValue))
    {
        valueOut.parameterType = "float";
        return true;
    }
    if (GetInt(jsonObject, valueOut.parameterIntValue))
    {
        valueOut.parameterType = "int";
        return true;
    }
    if (GetBool(jsonObject, valueOut.parameterBoolValue))
    {
        valueOut.parameterType = "bool";
        return true;
    }
    if (GetVec3(jsonObject, valueOut.parameterVec3Value))
    {
        valueOut.parameterType = "vec3";
        return true;
    }
    if (GetVec4(jsonObject, valueOut.parameterVec4Value))
    {
        valueOut.parameterType = "vec4";
        return true;
    }
    if (GetVecFloat(jsonObject, valueOut.parameterVecFloatValue))
    {
        valueOut.parameterType = "vecFloat";
        return true;
    }
    if (GetVecStr(jsonObject, valueOut.parameterVecStrValue))
    {
        valueOut.parameterType = "vecStr";
        return true;
    }
    if (GetMap(jsonObject, valueOut.parameterMapValue))
    {
        valueOut.parameterType = "map";
        return true;
    }

    // Value type not found
    return false;
}
