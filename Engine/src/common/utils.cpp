#include "common/utils.h"
#include <sstream>
#include <glm/gtx/string_cast.hpp>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <glm/mat4x4.hpp>

void __CheckEngineError(const char* msg, const char* file, int line)
{
    printf("Engine error file '%s' line '%d': %s\n", file, line, msg);
    return;
}

std::string myutils::GetFileExtension(const std::string& fileName)
{
    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != std::string::npos)
    {
        return fileName.substr(dotPos + 1);
    }
    return fileName; // Full string if no extension found
}

char* myutils::GetFileFromPath(const char* cPath)
{
    std::string sPath = cPath;
    std::string baseFilename = sPath.substr(sPath.find_last_of("/\\") + 1);
    char* cBaseFileName = new char[baseFilename.size() + 1];
    strcpy_s(cBaseFileName, baseFilename.size() + 1, baseFilename.c_str());
    return cBaseFileName;
}

void myutils::SplitString(const std::string& stringIn,
    char delimiter,
    std::vector<std::string>& vecOut)
{
    std::istringstream iss(stringIn.c_str());
    std::string token;

    vecOut.clear();
    while (std::getline(iss, token, delimiter)) {
        vecOut.push_back(token);
    }
}

template <typename T>
std::string myutils::ArrayToString(char delimiter, const std::vector<T>& arr) {
    std::ostringstream oss;

    if (arr.empty()) {
        return "";
    }

    oss << arr[0];
    for (size_t i = 1; i < arr.size(); ++i) {
        oss << delimiter << arr[i];
    }

    return oss.str();
}

std::string myutils::BoolToString(bool input) {
    return input ? "1" : "0";
}

int myutils::Sign(int val)
{
    return (0 < val) - (val < 0);
}

float myutils::GetRandFloat(float a, float b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

float myutils::GetRandFloat(float a, float b, float c, float d)
{
    if (c >= d || a >= b || c >= b || d <= a) {
        // Invalid ranges
        return 0.0f;
    }

    float random;
    float r;

    do {
        random = ((float)rand()) / (float)RAND_MAX;
        float diff = b - a;
        r = random * diff + a;
    } while (r >= c && r <= d);

    return r;
}

void myutils::WrapMinMax(int min, int max, int& numOut)
{
    if (numOut < min)
    {
        numOut = max;
    }
    else if (numOut > max)
    {
        numOut = min;
    }
}

void myutils::WrapMinMax(float min, float max, float& numOut)
{
    numOut = std::min(std::max(numOut, 0.0f), max - 1);
}

glm::vec4 myutils::StringToVec4(const std::string& glmstr)
{
    glm::vec4 result;

    std::string values = glmstr.substr(glmstr.find('(') + 1, glmstr.find(')') - glmstr.find('(') - 1);
    std::stringstream ss(values);
    ss >> result.x >> result.y >> result.z >> result.w;

    return result;
}

glm::vec3 myutils::StringToVec3(const std::string& glmstr)
{
    glm::vec3 result;

    std::string values = glmstr.substr(glmstr.find('(') + 1, glmstr.find(')') - glmstr.find('(') - 1);
    std::stringstream ss(values);
    ss >> result.x >> result.y >> result.z;

    return result;
}

std::string myutils::VecToString(glm::vec3 glmVec)
{
    std::string result = glm::to_string(glmVec);

    // Find and replace "vec3(" with "["
    size_t startPos = result.find("vec3(");
    if (startPos != std::string::npos) {
        result.replace(startPos, 5, "[");
    }

    // Find and replace ")" with "]"
    size_t endPos = result.find(")");
    if (endPos != std::string::npos) {
        result.replace(endPos, 1, "]");
    }

    return result;
}

std::string myutils::VecToString(glm::vec4 glmVec)
{
    std::string result = glm::to_string(glmVec);

    // Find and replace "vec3(" with "["
    size_t startPos = result.find("vec4(");
    if (startPos != std::string::npos) {
        result.replace(startPos, 5, "[");
    }

    // Find and replace ")" with "]"
    size_t endPos = result.find(")");
    if (endPos != std::string::npos) {
        result.replace(endPos, 1, "]");
    }

    return result;
}

double myutils::FormatFloatWithPrecision(float value, int decimalPlaces)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimalPlaces) << value;
    return std::stod(stream.str());
}

bool myutils::TxtToPointVector(std::string filename, char one, char zero, glm::vec3 minXY, glm::vec3 maxXY, std::vector<glm::vec3>& vecOut)
{
    vecOut.clear();

    // Open the file
    std::ifstream file(filename);

    if (!file.is_open()) {
        printf("Matrix '%s' not found!\n", filename.c_str());
        return false;
    }

    // Read the file line by line
    std::vector<std::vector<int>> grid;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char ch : line) {
            if (ch == one) {
                row.push_back(1);
            }
            else if (ch == zero)
            {
                row.push_back(0);
            }
        }
        grid.push_back(row);
    }

    // Close the file
    file.close();

    // Insert only '1' positions in (minXY, maxXY) in vector
    std::vector<glm::vec2> positions;
    int offset = 50;
    size_t gridCols = grid.size();
    size_t gridRows = grid[gridCols - 1].size();
    float scaleX = (static_cast<float>(maxXY.x - minXY.x) / gridRows);
    float scaleY = (static_cast<float>(maxXY.y - minXY.y) / gridCols) + 1;

    for (int i = 0; i < gridCols; ++i) {
        for (int j = 0; j < gridRows; ++j) {
            if (grid[i][j] == 1) {
                glm::mat4 rotationMatrix(1.0f); // Identity matrix
                // Convert position to 'world position' and push to vector
                glm::vec3 position(minXY.x + i * scaleX, minXY.y + j * scaleY, 0.0);
                // Apply a 90-degree rotation around the X-axis
                rotationMatrix = glm::rotate(rotationMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                // Apply the rotation to the point
                glm::vec3 rotatedPoint = glm::vec3(rotationMatrix * glm::vec4(position, 1.0f));
                rotatedPoint.y += maxXY.y + offset;

                vecOut.push_back(rotatedPoint);
            }
        }
    }

    return true;
}