#include "pch.h"
#include <shapefil.h>

#ifdef _DEBUG
#pragma comment(lib, "shpd.lib")
#else
#pragma comment(lib, "shp.lib")
#endif

void readShapeFile(const char* filename) {
    SHPHandle hSHP = SHPOpen(filename, "rb");
    if (hSHP == NULL) {
        printf("Failed to open SHP file: %s\n", filename);
        return;
    }

    int nEntities, nShapeType;
    SHPGetInfo(hSHP, &nEntities, &nShapeType, NULL, NULL);
    printf("File: %s\n", filename);
    printf("Number of entities: %d\n", nEntities);
    printf("Shape type: %d\n", nShapeType);

    for (int i = 0; i < nEntities; i++) {
        SHPObject* pShape = SHPReadObject(hSHP, i);
        if (pShape == NULL) {
            printf("Failed to read shape at index %d\n", i);
            continue;
        }

        printf("Entity %d:\n", i);
        printf("  Number of vertices: %d\n", pShape->nVertices);
        printf("  Bounding box: (%f, %f) to (%f, %f)\n", pShape->dfXMin, pShape->dfYMin, pShape->dfXMax, pShape->dfYMax);

        SHPDestroyObject(pShape);
    }

    SHPClose(hSHP);
}

void readDBFFile(const char* filename) {
    DBFHandle hDBF = DBFOpen(filename, "rb");
    if (hDBF == NULL) {
        printf("Failed to open DBF file: %s\n", filename);
        return;
    }

    int nRecords = DBFGetRecordCount(hDBF);
    int nFields = DBFGetFieldCount(hDBF);
    printf("File: %s\n", filename);
    printf("Number of records: %d\n", nRecords);
    printf("Number of fields: %d\n", nFields);

    for(int i=0; i<nFields; i++){
        char szFieldName[12];
        int nWidth, nDecimals;
        DBFFieldType enumType = DBFGetFieldInfo(hDBF, i, szFieldName, &nWidth, &nDecimals);
        printf("Field %d: Name = %s, Type = %d, Width = %d, Decimals = %d\n", i, szFieldName, enumType, nWidth, nDecimals);
    }

    DBFClose(hDBF);
}

int main()
{
    printf("Hello Shape!\n");

    const char* shp_filepath = "C:\\kmap_2023_120_korean_shp\\KOR_ADMIN_AS_국문.shp";
    const char* dbf_filepath = "C:\\kmap_2023_120_korean_shp\\KOR_ADMIN_AS_국문.dbf";

    readShapeFile(shp_filepath);
    readDBFFile(dbf_filepath);

    printf("Complete Shape!\n");
}
