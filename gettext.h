#pragma once

#define ENABLE_NLS 1
#include <stdlib.h>
#include <libintl.h>
#include "gettext/gettext.h"

#define translate(msgid) msgid
#define ptranslate(msgctx, msgid) msgctx GETTEXT_CONTEXT_GLUE msgid
