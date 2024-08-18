#ifndef SHAPE_CONVERT_CLASS_H_
#define SHAPE_CONVERT_CLASS_H_

class ShapeConvertClass
{
private:
    std::unordered_map<int, std::vector<std::pair<float, float>>> m_mapShapePoint;
    std::unordered_map<int, std::vector<std::pair<float, float>>> m_mapShapePolyLine;
    std::unordered_map<int, std::vector<std::pair<float, float>>> m_mapShapePolygon;
    int m_shpCount = 0;
    int m_dbfCount = 0;

public:
    explicit ShapeConvertClass();
    virtual ~ShapeConvertClass();

    void readShapeFile(const char* filename);
    void readDBFFile(const char* filename);
    void readShapeDirectory(const std::string& _strDirectoryPath);
    std::unordered_map<int, std::vector<std::pair<float, float>>> getPoints();
    std::unordered_map<int, std::vector<std::pair<float, float>>> getPolyLines();
    std::unordered_map<int, std::vector<std::pair<float, float>>> getPolygon();
};

#endif			SHAPE_CONVERT_CLASS_H_