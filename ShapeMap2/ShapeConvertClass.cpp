#include "pch.h"
#include "ShapeConvertClass.h"
#include <shapefil.h>

#ifdef _DEBUG
#pragma comment(lib, "shpd.lib")
#else
#pragma comment(lib, "shp.lib")
#endif

ShapeConvertClass::ShapeConvertClass()
{

}

ShapeConvertClass::~ShapeConvertClass()
{
    m_mapShapePoint.clear();
    m_mapShapePolyLine.clear();
    m_mapShapePolygon.clear();
    
}

void ShapeConvertClass::readShapeFile(const char* filename)
{
    SHPHandle hSHP = SHPOpen(filename, "rb");
    if (hSHP == NULL) {
        return;
    }

    int nEntities, nShapeType;
    SHPGetInfo(hSHP, &nEntities, &nShapeType, NULL, NULL);

    for (int i = 0; i < nEntities; i++) {
        SHPObject* pShape = SHPReadObject(hSHP, i);
        if (pShape == NULL) {
            continue;
        }

        std::vector<std::pair<float, float>> vecVertices;
        vecVertices.resize(pShape->nVertices);
        for (int j = 0; j < pShape->nVertices; j++) {
            vecVertices[j] = std::make_pair(static_cast<float>(pShape->padfX[j]), static_cast<float>(pShape->padfY[j]));
        }

        if (nShapeType == 1) {
            m_mapShapePoint.emplace(i, std::move(vecVertices));
        }
        else if (nShapeType == 3) {
            m_mapShapePolyLine.emplace(i, std::move(vecVertices));
        }
        else if (nShapeType == 5) {
            m_mapShapePolygon.emplace(i, std::move(vecVertices));
        }
        SHPDestroyObject(pShape);
    }

    SHPClose(hSHP);
}

void ShapeConvertClass::readDBFFile(const char* filename)
{
    DBFHandle hDBF = DBFOpen(filename, "rb");
    if (hDBF == NULL) {
        return;
    }

    int nRecordCount = DBFGetRecordCount(hDBF);
    int nFieldCount = DBFGetFieldCount(hDBF);

    for (int i = 0; i < nFieldCount; i++)
    {
        char szFieldName[12]{ 0 };
        int nWidth, nDecimals;
        DBFFieldType enumType = DBFGetFieldInfo(hDBF, i, szFieldName, &nWidth, &nDecimals);
    }

    DBFClose(hDBF);
}

void ShapeConvertClass::readShapeDirectory(const std::string& _strDirectoryPath)
{
    m_mapShapePoint.clear();
    m_mapShapePolyLine.clear();
    m_mapShapePolygon.clear();
    //std::vector<std::future<void>> vecFuture;

    for (const auto& entry : std::filesystem::directory_iterator(_strDirectoryPath)) {
        if (entry.is_regular_file()) {
            std::string fileFullPath = entry.path().string();
            if (entry.path().extension() == ".shp") {
                readShapeFile(fileFullPath.c_str());
                //vecFuture.emplace_back(std::async(std::launch::async, &ShapeConvertClass::readShapeFile, this, fileFullPath.c_str()));
            }
            else if (entry.path().extension() == ".dbf") {
                readDBFFile(fileFullPath.c_str());
                //vecFuture.emplace_back(std::async(std::launch::async, &ShapeConvertClass::readDBFFile, this, fileFullPath.c_str()));
            }
        }
    }

    //for (auto& future : vecFuture) {
    //    future.get();
    //}
}

std::unordered_map<int, std::vector<std::pair<float, float>>> ShapeConvertClass::getPoints()
{
    return std::move(m_mapShapePoint);
}

std::unordered_map<int, std::vector<std::pair<float, float>>> ShapeConvertClass::getPolyLines()
{
    return std::move(m_mapShapePolyLine);
}

std::unordered_map<int, std::vector<std::pair<float, float>>> ShapeConvertClass::getPolygon()
{
    return std::move(m_mapShapePolygon);
}
