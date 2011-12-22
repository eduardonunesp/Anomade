#include "config.h"

char markdown_version[] = "1.2.2"
#if DL_TAG_EXTENSION
		" DL_TAG"
#endif
#if PANDOC_HEADER
		" HEADER"
#endif
#if 4 != 4
		" TAB=4"
#endif
#if USE_AMALLOC
		" DEBUG"
#endif
		;
