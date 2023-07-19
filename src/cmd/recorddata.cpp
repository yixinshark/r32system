//
// Created by yixin on 2023/7/19.
//

#include "recorddata.h"

RecordData *RecordData::instance()
{
    static RecordData *s_instance = nullptr;
    if (!s_instance) {
        s_instance = new RecordData();
    }

    return s_instance;
}

void RecordData::reset()
{
    m_recordData.clear();
    m_currentAddr = 0;
}

void RecordData::setCurrentAddr(int addr)
{
    m_currentAddr = addr;
}

int RecordData::currentAddr()
{
    return m_currentAddr;
}

RecordData::RecordData()
{

}

RecordData::~RecordData()
{

}
