
#include "MeshPass.h"


namespace Atuin {




    
} // Atuin


bool operator< (const Atuin::RenderBatch &lhs, const Atuin::RenderBatch &rhs) {

    if ( lhs.sortKey == rhs.sortKey)
	{
		return lhs.objectIdx < rhs.objectIdx;
	}
			
	return lhs.sortKey < rhs.sortKey;
}
