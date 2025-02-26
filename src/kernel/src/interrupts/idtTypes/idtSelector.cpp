#include "idtSelector.h"

void IdtSelector::setSelector()
{
    this->attr.tableIndicator = GDT_INDICATOR;
    this->attr.privilegeLevel = SAME_PRIVILEGE_AS_DESCRIPTOR_TABLE;
    this->attr.index          = CODE_SEGMENT;
}