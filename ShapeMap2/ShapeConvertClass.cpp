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
    int key = 0;
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

        // 다중 파트를 처리하기 위해 각 파트를 개별적으로 분리
        for (int part = 0; part < pShape->nParts; part++) {
            int startIndex = pShape->panPartStart[part];
            int endIndex = (part == pShape->nParts - 1) ? pShape->nVertices : pShape->panPartStart[part + 1];

            std::vector<std::pair<float, float>> vecVertices;
            vecVertices.reserve(endIndex - startIndex);
            for (int j = startIndex; j < endIndex; j++) {
                vecVertices.emplace_back(static_cast<float>(pShape->padfX[j]), static_cast<float>(pShape->padfY[j]));
            }
            key++;
            // 폴리곤 타입에 따라 저장
            if (nShapeType == 1) {
                m_mapShapePoint.emplace(key, std::move(vecVertices));
            }
            else if (nShapeType == 3) {
                m_mapShapePolyLine.emplace(key, std::move(vecVertices));
            }
            else if (nShapeType == 5) {
                m_mapShapePolygon.emplace(key, std::move(vecVertices)); // 각 파트를 독립적으로 관리하기 위해 고유 키 사용
            }
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
